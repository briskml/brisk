.phone: bootstrap

STDLIB_PATH=$$(ocamlc -where)
BUILD_STORE_PATH=../../_build/install/default/lib

CONFIGURATION="Debug"
SCHEME="BriskCocoa"

bootstrap:
	@echo "> Boostrapping .xcodeproj"
	@rm -rf ./Cocoa.xcodeproj
	@echo "STDLIB_PATH=$(STDLIB_PATH)" > ./env.xcconfig
	@echo "BUILD_STORE_PATH=$(BUILD_STORE_PATH)" >> ./env.xcconfig
	xcodegen --spec project.yml

build:
	@echo "> Building the Cocoa bindings"
	@xcodebuild -scheme $(SCHEME) -configuration $(CONFIGURATION)
