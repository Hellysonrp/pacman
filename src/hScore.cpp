/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "hScore.h"
#include "Constants.h"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <sstream>
#include <sys/stat.h>
#include <vector>

namespace {
std::string ensureTrailingSlash(const std::string& path) {
    if (path.empty() || path.back() == '/') {
        return path;
    }
    return path + '/';
}

std::string joinPath(const std::string& base, const std::string& relative) {
    if (relative.empty()) {
        return base;
    }
    if (!relative.empty() && (relative[0] == '/'
#ifdef _WIN32
        || (relative.size() > 1 && relative[1] == ':')
#endif
        )) {
        return relative;
    }
    return ensureTrailingSlash(base) + relative;
}

bool fileExists(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    struct stat info {};
    return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

bool dirExists(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    struct stat info {};
    return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
}

bool ensureDirectory(const std::string& path) {
    if (path.empty() || dirExists(path)) {
        return true;
    }

    const auto slashPos = path.find_last_of('/');
    if (slashPos != std::string::npos) {
        const std::string parent = path.substr(0, slashPos);
        if (!ensureDirectory(parent)) {
            return false;
        }
    }

    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
}

std::vector<std::string> buildSearchPaths() {
    // FIXME: Procura arquivos nas mesmas pastas utilizadas para mapas, priorizando a cópia na pasta do usuário.
    std::vector<std::string> paths;
    if (const char* home = std::getenv("HOME")) {
        paths.emplace_back(std::string(home) + "/" HOME_CONF_PATH);
    }
    paths.emplace_back(".");
    paths.emplace_back(APP_PATH);
    return paths;
}

// Normaliza o nome garantindo letras maiúsculas e removendo placeholders como "---".
std::string normalizeName(const std::string& raw) {
    auto first = std::find_if_not(raw.begin(), raw.end(), [](unsigned char ch) {
        return std::isspace(static_cast<unsigned char>(ch)) != 0;
    });

    if (first == raw.end()) {
        return {};
    }

    auto lastIt = std::find_if_not(raw.rbegin(), raw.rend(), [](unsigned char ch) {
        return std::isspace(static_cast<unsigned char>(ch)) != 0;
    });
    auto last = lastIt.base();

    if (first >= last) {
        return {};
    }

    std::string normalized(first, last);
    if (normalized.size() > 3) {
        normalized.resize(3);
    }

    for (char& ch : normalized) {
        if (ch != '_' && ch != '-') {
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }

    if (normalized == "---") {
        normalized.clear();
    }

    return normalized;
}

bool hasContent(const hScore::ScoreEntry& entry) {
    return !entry.playerName.empty() || entry.playerScore != 0;
}

struct ScoreDescending {
    bool operator()(const hScore::ScoreEntry& lhs, const hScore::ScoreEntry& rhs) const {
        if (lhs.playerScore == rhs.playerScore) {
            return lhs.playerName < rhs.playerName;
        }
        return lhs.playerScore > rhs.playerScore;
    }
};
}

hScore::hScore()
    : entries(MAXENTRIES, {"", 0}) {
}

hScore::~hScore() = default;

void hScore::clear() {
    entries.assign(MAXENTRIES, {"", 0});
    const std::string target = resolveWritablePath();
    if (!target.empty()) {
        remove(target.c_str());
    }
}

int hScore::load() {
    if (filename.empty()) {
        entries.assign(MAXENTRIES, {"", 0});
        return 1;
    }

    const std::string writableCandidate = resolveWritablePath();
    const bool hasWritableFile = fileExists(writableCandidate);

    std::ifstream file;
    if (hasWritableFile) {
        file.open(writableCandidate.c_str(), std::ios::binary);
        if (file) {
            resolvedFilename = writableCandidate;
        }
    }

    if (!file) {
        resolvedFilename = resolveExistingPath();

        if (!resolvedFilename.empty()) {
            file.open(resolvedFilename.c_str(), std::ios::binary);
        }

        if (!file && fileExists(filename)) {
            file.clear();
            file.open(filename.c_str(), std::ios::binary);
            if (file) {
                resolvedFilename = filename;
            }
        }
    }

    if (!file) {
        entries.assign(MAXENTRIES, {"", 0});
        if (!writableCandidate.empty()) {
            resolvedFilename = writableCandidate;
        }
        return 1;
    }

    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    entries.clear();
    bool parsed = false;

    if (!data.empty()) {
        if (!parsed) {
            parsed = loadPlaintext(data);
        }
    }

    if (!parsed) {
        entries.assign(MAXENTRIES, {"", 0});
    }

    // Quando apenas o arquivo instalado existir, tratamos o conteúdo como template e começamos com o ranking zerado.
    if (!hasWritableFile && !writableCandidate.empty() && resolvedFilename != writableCandidate) {
        entries.assign(MAXENTRIES, {"", 0});
        resolvedFilename = writableCandidate;
    }

    compactEntries();
    sortEntries();
    ensureSize();
    return 0;
}

int hScore::save() {
    compactEntries();
    sortEntries();
    ensureSize();

    if (filename.empty()) {
        return 1;
    }

    std::string target = resolvedFilename.empty() ? resolveWritablePath() : resolvedFilename;
    if (target.empty()) {
        target = filename;
    }

    std::ofstream file(target.c_str(), std::ios::binary | std::ios::trunc);
    if (!file && target != filename) {
        file.clear();
        file.open(filename.c_str(), std::ios::binary | std::ios::trunc);
        if (file) {
            target = filename;
        }
    }

    if (!file) {
        return 1;
    }

    for (const auto& entry : entries) {
        if (hasContent(entry)) {
            file << entry.playerName << ' ' << entry.playerScore << '\n';
        }
        else {
            file << "--- 0\n";
        }
    }

    file.flush();
    if (!file.good()) {
        return 1;
    }

    resolvedFilename = target;
    return 0;
}

bool hScore::onlist(unsigned int sc) const {
    int i = 0;
    while (i < MAXENTRIES && sc < entries[i].playerScore) {
        ++i;
    }
    return i < MAXENTRIES;
}

void hScore::add(std::string n, unsigned int sc) {
    ScoreEntry sanitized{normalizeName(n), sc};

    // Manter apenas entradas significativas para evitar preencher o ranking com valores vazios.
    if (!hasContent(sanitized)) {
        return;
    }

    entries.push_back(sanitized);
    compactEntries();
    sortEntries();
    ensureSize();
}

void hScore::setfilename(std::string fn) {
    filename = fn;
    resolvedFilename.clear();
}

int hScore::getScore(int i) {
    if (i < 0 || i >= static_cast<int>(entries.size())) {
        return 0;
    }
    return static_cast<int>(entries[i].playerScore);
}

std::string hScore::getName(int i) {
    if (i < 0 || i >= static_cast<int>(entries.size())) {
        return {};
    }
    return entries[i].playerName;
}

std::vector<hScore::ScoreEntry> hScore::getEntries() const {
    return entries;
}

void hScore::sortEntries() {
    std::sort(entries.begin(), entries.end(), ScoreDescending{});
}

void hScore::compactEntries() {
    entries.erase(std::remove_if(entries.begin(), entries.end(), [](const ScoreEntry& entry) {
        return !hasContent(entry);
    }), entries.end());
}

void hScore::ensureSize() {
    if (entries.size() > MAXENTRIES) {
        entries.resize(MAXENTRIES);
    }

    while (entries.size() < MAXENTRIES) {
        entries.push_back({"", 0});
    }
}

bool hScore::loadPlaintext(const std::string& data) {
    std::istringstream stream(data);
    std::string rawName;
    unsigned long value = 0;
    bool parsedAny = false;

    while (stream >> rawName >> value) {
        if (rawName == "---") {
            rawName.clear();
        }

        entries.push_back({normalizeName(rawName), value});
        parsedAny = true;
    }

    return parsedAny;
}

std::string hScore::resolveExistingPath() const {
    if (filename.empty()) {
        return {};
    }

    if (fileExists(filename)) {
        return filename;
    }

    for (const auto& base : buildSearchPaths()) {
        const std::string candidate = joinPath(base, filename);
        if (fileExists(candidate)) {
            return candidate;
        }
    }

    return {};
}

std::string hScore::resolveWritablePath() const {
    if (filename.empty()) {
        return {};
    }

    if (!resolvedFilename.empty()) {
        return resolvedFilename;
    }

    if (!filename.empty() && filename[0] == '/') {
        return filename;
    }

    for (const auto& base : buildSearchPaths()) {
        const std::string candidate = joinPath(base, filename);
        const auto slashPos = candidate.find_last_of('/');
        if (slashPos == std::string::npos) {
            continue;
        }

        // Garante que vai ser gravado na cópia instalada do nível ao invés da pasta atual do executável.
        if (ensureDirectory(candidate.substr(0, slashPos))) {
            return candidate;
        }
    }

    return filename;
}
