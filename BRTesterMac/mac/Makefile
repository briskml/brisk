.phone: bootstrap

STDLIB_PATH=$$(ocamlc -where)
BUILD_STORE_PATH=../../_build/install/default/lib

CONFIGURATION="Debug"
SCHEME="BriskMac"

BUILT_PRODUCTS_DIR := $(shell xcodebuild -showBuildSettings -scheme $(SCHEME) -configuration $(CONFIGURATION) | grep ' BUILT_PRODUCTS_DIR' | sed -e 's/.*= *//')
FULL_PRODUCT_NAME := $(shell xcodebuild -showBuildSettings -scheme $(SCHEME) -configuration $(CONFIGURATION) | grep ' FULL_PRODUCT_NAME' | sed -e 's/.*= *//')
PRODUCT_NAME := $(basename $(FULL_PRODUCT_NAME))

run: build exec

bootstrap:
	@echo "> Boostrapping .xcodeproj"
	@rm -rf ./Brisk.xcodeproj
	@echo "STDLIB_PATH=$(STDLIB_PATH)" > ./Resources/env.xcconfig
	@echo "BUILD_STORE_PATH=$(BUILD_STORE_PATH)" >> ./Resources/env.xcconfig
	xcodegen --spec project.yml

build:
	@echo "> Building the project"
	@xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION)

test:
	xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION) clean build test | xcpretty && exit ${PIPESTATUS[0]}

exec:
	@eval DYLD_FRAMEWORK_PATH=$(BUILT_PRODUCTS_DIR) $(BUILT_PRODUCTS_DIR)/$(FULL_PRODUCT_NAME)/Contents/MacOS/$(PRODUCT_NAME)
