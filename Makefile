.PHONY: all
all: build

.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make format          - Format all .cpp files with clang-format"
	@echo "  make lint            - Run all linting (C++ and Makefile)"
	@echo "  make lint/cpp        - Lint all .cpp files with cpplint"
	@echo "  make lint/makefile   - Lint Makefile with checkmake"
	@echo "  make conan-install   - Install dependencies with Conan"
	@echo "  make cmake-configure - Configure CMake with Conan toolchain"
	@echo "  make build           - Build all targets"
	@echo "  make build/hw0       - Build Homework_0 targets"
	@echo "  make build/hw1       - Build Homework_1 targets"
	@echo "  make test            - Run all tests with CTest"
	@echo "  make clean           - Remove build artifacts"

.PHONY: format
format:
	@echo "Formatting all .cpp files..."
	@find . -name "*.cpp" -type f -exec clang-format -i {} \;
	@echo "✓ Formatting complete"

.PHONY: lint
lint: lint/cpp lint/makefile
	@echo "✓ All linting complete"

.PHONY: lint/cpp
lint/cpp:
	@echo "Linting all .cpp files..."
	@find . -name "*.cpp" -type f -exec cpplint {} \;
	@echo "✓ C++ linting complete"

.PHONY: lint/makefile
lint/makefile:
	@echo "Linting Makefile..."
	@checkmake Makefile
	@echo "✓ Makefile linting complete"

.PHONY: conan-install
conan-install:
	@echo "Installing dependencies with Conan..."
	@mkdir -p build
	@cd build && conan install .. --output-folder=. --build=missing
	@echo "✓ Conan dependencies installed"

.PHONY: cmake-configure
cmake-configure: conan-install
	@echo "Configuring CMake..."
	@cd build && cmake .. --preset conan-release
	@echo "✓ CMake configured"

.PHONY: cmake-default
cmake-default: conan-install
	@echo "Configuring CMake with default settings..."
	@cmake --preset default
	@echo "✓ CMake configured with default settings"
	@cd build && make

.PHONY: build
build: cmake-configure
	@echo "Building all targets..."
	@cmake --build build/build/Release
	@echo "✓ Build complete"

.PHONY: build/hw0
build/hw0: cmake-configure
	@echo "Building Homework_0..."
	@cmake --build build/build/Release --target Homework_0
	@echo "✓ Homework_0 build complete"

.PHONY: build/hw1
build/hw1: cmake-configure
	@echo "Building Homework_1..."
	@cmake --build build/build/Release --target Homework_1
	@echo "✓ Homework_1 build complete"

.PHONY: test
test: build
	@echo "Running tests..."
	@ctest --test-dir build/build/Release --verbose
	@echo "✓ Tests complete"

.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	@find build -mindepth 1 ! -name ".gitkeep" -delete
	@echo "✓ Build directory cleaned"

.PHONY: list-presets
list-presets:
	@echo "Available CMake presets:"
	@cmake --list-presets
