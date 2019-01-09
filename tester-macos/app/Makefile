.phone: bootstrap

CONFIGURATION="Debug"
SCHEME="BriskMac"

run: build exec

bootstrap:
	$(eval STDLIB_PATH := $(shell ocamlc -where))
	$(eval BUILD_INSTALL_PATH := $(abspath ../../_esy/default/install))

	@echo "> Boostrapping .xcodeproj"
	@rm -rf ./Brisk.xcodeproj
	@echo "STDLIB_PATH=$(STDLIB_PATH)" > ./Resources/env.xcconfig
	@echo "LIBFFI_FLAGS=$(LIBFFI_LIBS)" >> ./Resources/env.xcconfig
	@echo "BUILD_INSTALL_PATH=$(BUILD_INSTALL_PATH)" >> ./Resources/env.xcconfig
	xcodegen --spec project.yml

build:
	$(eval BUILD_INSTALL_PATH := $(abspath ../../_esy/default/install))
	$(eval STORE_INSTALL_PATH := $(shell esy -p @brisk/tester-macos echo '#{self.install}'))
	@rm -rf $(BUILD_INSTALL_PATH)
	@ln -sF $(STORE_INSTALL_PATH) $(BUILD_INSTALL_PATH)
	@echo "> Building the project"
	@xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION) build

test:
	xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION) clean build test | xcpretty && exit ${PIPESTATUS[0]}

exec:
	$(eval BUILT_PRODUCTS_DIR := $(shell xcodebuild -showBuildSettings -scheme $(SCHEME) -configuration $(CONFIGURATION) | grep ' BUILT_PRODUCTS_DIR' | sed -e 's/.*= *//'))
	$(eval FULL_PRODUCT_NAME := $(shell xcodebuild -showBuildSettings -scheme $(SCHEME) -configuration $(CONFIGURATION) | grep ' FULL_PRODUCT_NAME' | sed -e 's/.*= *//'))
	$(eval PRODUCT_NAME := $(basename $(FULL_PRODUCT_NAME)))

	@eval DYLD_FRAMEWORK_PATH=$(BUILT_PRODUCTS_DIR) $(BUILT_PRODUCTS_DIR)/$(FULL_PRODUCT_NAME)/Contents/MacOS/$(PRODUCT_NAME)
