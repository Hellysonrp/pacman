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
#include "PathUtils.h"
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <sstream>
#include <vector>

namespace {
std::vector<std::filesystem::path> buildSearchPaths() {
    // FIXME: Procura arquivos nas mesmas pastas utilizadas para mapas, priorizando a cópia na pasta do usuário.
    std::vector<std::filesystem::path> paths;
    paths.emplace_back(PathUtils::getHomeConfigPath());
    paths.emplace_back(".");
    paths.emplace_back(PathUtils::getAppPath());
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
    const std::filesystem::path target = resolveWritablePath();
    if (!target.empty()) {
        remove(target.string().c_str());
    }
}

int hScore::load() {
    if (filename.empty()) {
        entries.assign(MAXENTRIES, {"", 0});
        return 1;
    }

    const std::filesystem::path writableCandidate = resolveWritablePath();
    const bool hasWritableFile = !writableCandidate.empty() && std::filesystem::exists(writableCandidate) && std::filesystem::is_regular_file(writableCandidate);

    std::ifstream file;
    if (hasWritableFile) {
        file.open(writableCandidate, std::ios::binary);
        if (file) {
            resolvedFilename = writableCandidate;
        }
    }

    if (!file) {
        resolvedFilename = resolveExistingPath();

        if (!resolvedFilename.empty()) {
            file.open(resolvedFilename, std::ios::binary);
        }

        if (!file && std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename)) {
            file.clear();
            file.open(filename, std::ios::binary);
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

    std::filesystem::path target = resolvedFilename.empty() ? resolveWritablePath() : resolvedFilename;
    if (target.empty()) {
        target = filename;
    }

    std::ofstream file(target, std::ios::binary | std::ios::trunc);
    if (!file && target != filename) {
        file.clear();
        file.open(filename, std::ios::binary | std::ios::trunc);
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

void hScore::setfilename(std::filesystem::path fn) {
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

std::filesystem::path hScore::resolveExistingPath() const {
    if (filename.empty()) {
        return {};
    }

    if (std::filesystem::exists(filename) && std::filesystem::is_regular_file(filename)) {
        return filename;
    }

    for (const auto& base : buildSearchPaths()) {
        const std::filesystem::path candidate = base / filename;
        if (std::filesystem::exists(candidate) && std::filesystem::is_regular_file(candidate)) {
            return candidate;
        }
    }

    return {};
}

std::filesystem::path hScore::resolveWritablePath() const {
    if (filename.empty()) {
        return {};
    }

    if (!resolvedFilename.empty()) {
        return resolvedFilename;
    }

    if (!filename.empty() && filename.is_absolute()) {
        return filename;
    }

    for (const auto& base : buildSearchPaths()) {
        const std::filesystem::path candidate = base / filename;
        const std::filesystem::path parentDir = candidate.parent_path();
        
        if (parentDir.empty()) {
            continue;
        }

        // Garante que vai ser gravado na cópia instalada do nível ao invés da pasta atual do executável.
        if (std::filesystem::create_directories(parentDir)) {
            return candidate;
        }
    }

    return filename;
}
