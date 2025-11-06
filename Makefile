all: generate build

generate:
ifeq ($(OS),Windows_NT)
	@if not exist build mkdir build
else
	@mkdir -p build
endif
	cd build && cmake ..

build:
	cmake --build build

clean:
ifeq ($(OS),Windows_NT)
	@if exist build rmdir /s /q build
else
	@rm -rf build
endif

.PHONY: build generate clean
