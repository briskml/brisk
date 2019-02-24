.PHONY: build run

CONFIGURATION := "Debug"
SCHEME := "HackerNews"

build:
	$(eval STDLIB_PATH := $(shell ocamlc -where))
	$(eval BUILD_INSTALL_PATH := $(abspath ../_esy/default/install))

	@echo "STDLIB_PATH=$(STDLIB_PATH)" > ./Resources/env.xcconfig
	@echo "BUILD_INSTALL_PATH=$(BUILD_INSTALL_PATH)" >> ./Resources/env.xcconfig

	@xcodegen generate --use-cache --spec project.yml
	@xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION) build | xcpretty && exit ${PIPESTATUS[0]}

run: build
	$(eval BUILT_PRODUCTS_DIR := $(shell xcodebuild -showBuildSettings -scheme $(SCHEME) -configuration $(CONFIGURATION) | grep ' BUILT_PRODUCTS_DIR' | sed -e 's/.*= *//'))
	$(eval FULL_PRODUCT_NAME := $(shell xcodebuild -showBuildSettings -scheme $(SCHEME) -configuration $(CONFIGURATION) | grep ' FULL_PRODUCT_NAME' | sed -e 's/.*= *//'))
	$(eval PRODUCT_NAME := $(basename $(FULL_PRODUCT_NAME)))

	@eval DYLD_FRAMEWORK_PATH=$(BUILT_PRODUCTS_DIR) $(BUILT_PRODUCTS_DIR)/$(FULL_PRODUCT_NAME)/Contents/MacOS/$(PRODUCT_NAME)
