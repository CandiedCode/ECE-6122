.PHONY: all
all: build

.PHONY: lint/makefile
lint/makefile: ## Lint Makefile using checkmake
	@echo "Linting Makefile..."
	@checkmake Makefile
	@echo "✓ Makefile linting complete"

.PHONY: lint/format
lint/format: ## Check code formatting using clang-format
	@echo "Checking code formatting..."
	@find . -path ./build -prune -o -path ./SFML -prune -o \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print | xargs clang-format -i --Werror
	@echo "✓ Code formatting check complete"

.PHONY: lint/cpplint
lint/cpplint: ## Lint C++ code using cpplint
	@echo "Linting C++ code..."
	@find . -path ./build -prune -o -path ./SFML -prune -o \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print | xargs cpplint
	@echo "✓ C++ linting complete"

.PHONY: lint/tidy
lint/tidy: ## Run clang-tidy static analysis
	@echo "Running clang-tidy..."
	@find . -path ./build -prune -o \
		-path ./node_modules -prune -o \
		-path ./SFML -prune -o \
		-path ./Homework_0 -prune -o \
		-path ./Homework_1 -prune -o \
		\( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print | xargs clang-tidy -p build
	@echo "✓ clang-tidy analysis complete"

.PHONY: lint/tidy-fix
lint/tidy-fix: ## Run clang-tidy static analysis and apply fixes
	@echo "Running clang-tidy..."
	@find . -path ./build -prune -o \
		-path ./node_modules -prune -o \
		-path ./SFML -prune -o \
		-path ./Homework_0 -prune -o \
		-path ./Homework_1 -prune -o \
		\( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print | xargs clang-tidy -p build --fix --fix-errors
	@echo "✓ clang-tidy analysis complete"

.PHONY: lint/cppcheck
lint/cppcheck: ## Run Cppcheck security analysis with MISRA rules
	@echo "Running Cppcheck with MISRA security checks..."
	@cppcheck --enable=all --suppress=missingIncludeSystem \
		--suppress=checkersReport \
		--suppress=normalCheckLevelMaxBranches \
		--error-exitcode=1 \
		--std=c++17 \
		-i build \
		-i node_modules \
		-i SFML \
		.
	@echo "✓ Cppcheck analysis complete"

.PHONY: lint/markdown
lint/markdown: ## Check markdown files using markdownlint v0.38.0
	@echo "Linting markdown files..."
	@find . -name "*.md" ! -path "./build/*" ! -path "./node_modules/*" -print | xargs markdownlint
	@echo "✓ Markdown linting complete"

.PHONY: lint/markdown-fix
lint/markdown-fix: ## Fix markdown files using markdownlint v0.38.0
	@echo "Fixing markdown files..."
	@find . -name "*.md" ! -path "./build/*" ! -path "./node_modules/*" -print | xargs markdownlint --fix
	@echo "✓ Markdown files fixed"

.PHONY: cmake
cmake: BUILD_TYPE ?= Debug
cmake: ## Generate CMake build files using the default preset
	@echo "Generating CMake build files..."
	cd build && cmake .. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/libomp

.PHONY: cmake/benchmark
cmake/benchmark: ## Generate CMake build files for Benchmark configuration
	cmake --build build --config Benchmark --verbose

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

.PHONY: help
help: ## Shows all targets and help from the Makefile (this message).
	@grep --no-filename -E '^([a-zA-Z0-9_.%/-]+:.*?)##' $(MAKEFILE_LIST) | sort | \
		awk 'BEGIN {FS = ":.*?(## ?)"}; { \
			if (length($$1) > 0) { \
				printf "  \033[36m%-30s\033[0m %s\n", $$1, $$2; \
			} else { \
				printf "%s\n", $$2; \
			} \
		}'

.PHONY: pre-commit/gc
pre-commit/gc: ## Clean unused cached repos.
	@pre-commit gc

.PHONY: pre-commit/remove-cache
pre-commit/remove-cache: ## Remove all of pre-commit's cache
	rm -rf ~/.cache/pre-commit/

.PHONY: pre-commit/run
pre-commit/run: ## Manually run pre-commit
	@pre-commit run --verbose

.PHONY: pre-commit/install
pre-commit/install: ## Install pre-commit
	@pre-commit install

.PHONY: pre-commit/autoupdate
pre-commit/autoupdate: ## Update pre-commit hook versions
	@pre-commit autoupdate

.PHONY: pre-commit/uninstall
pre-commit/uninstall: ## Uninstall pre-commit
	@pre-commit uninstall

.PHONY: semantic-release/install
semantic-release/install: ## Install semantic-release dependencies (nvm use + npm install)
	@echo "Setting up Node.js environment for semantic-release..."
	@command -v nvm >/dev/null 2>&1 && . ~/.nvm/nvm.sh && nvm use || echo "⚠ nvm not found, skipping nvm use"
	@echo "Installing dependencies..."
	@npm install
	@echo "✓ Setup complete"

.PHONY: semantic-release/dry-run
semantic-release/dry-run: ## Perform a dry-run of semantic-release (no changes made)
	@echo "Running semantic-release dry-run..."
	@npx semantic-release --dry-run --no-ci --branches main
	@echo "✓ Dry-run complete"

.PHONY: semantic-release/publish
semantic-release/publish: ## Publish a release using semantic-release
	@echo "Publishing release..."
	@npx semantic-release --branches main
	@echo "✓ Release published"
