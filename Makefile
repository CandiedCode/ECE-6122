.PHONY: all
all: build

.PHONY: lint/makefile
lint/makefile: ## Lint Makefile using checkmake
	@echo "Linting Makefile..."
	@checkmake Makefile
	@echo "✓ Makefile linting complete"

.PHONY: cmake
cmake: BUILD_TYPE ?= Debug
cmake: ## Generate CMake build files using the default preset
	@echo "Generating CMake build files..."
	cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

.PHONY: cmake/debug
cmake/debug: ## Generate CMake build files for Debug configuration
	@$(MAKE) -B cmake BUILD_TYPE=Debug

.PHONY: cmake/release
cmake/release: ## Generate CMake build files for Release configuration
	@$(MAKE) -B cmake BUILD_TYPE=Release

.PHONY: build
build: cmake 
build: ## Build the project using CMake
	cd build && cmake --build . -j 8

.PHONY: build/debug
build/debug: cmake/debug 
build/debug: ## Build the project in Debug configuration
	cd build && cmake --build . -j 8

.PHONY: build/release
build/release: cmake/release
build/release: ## Build the project in Release configuration
	cd build && cmake --build . -j 8

.PHONY: clean
clean: ## Clean the build directory
	@echo "Cleaning build directory..."
	@find build -mindepth 1 ! -name ".gitkeep" -delete
	@echo "✓ Build directory cleaned"

.PHONY: list-presets
list-presets: ## List available CMake presets
	@echo "Available CMake presets:"
	@cmake --list-presets
