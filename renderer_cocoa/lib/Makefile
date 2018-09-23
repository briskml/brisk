.phone: bootstrap

CONFIGURATION="Debug"
SCHEME="BriskCocoa"

bootstrap:
	$(eval STDLIB_PATH := $(shell ocamlc -where))
	$(eval BUILD_STORE_PATH := $(abspath ../../_build/default))

	@echo "> Boostrapping .xcodeproj"
	@rm -rf ./Cocoa.xcodeproj
	@echo "STDLIB_PATH=$(STDLIB_PATH)" > ./env.xcconfig
	@echo "BUILD_STORE_PATH=$(BUILD_STORE_PATH)" >> ./env.xcconfig
	xcodegen --spec project.yml

build:
	@echo "> Building the Cocoa bindings"
	@xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION) build
