.phone: bootstrap

OCAMLLIB_PATH=$$(ocamlfind query findlib)/../ocaml
BUILD_STORE_PATH=../../_build/install/default/lib

bootstrap:
	@rm -rf ./Brisk.xcodeproj
	@echo "OCAMLLIB_PATH=$(OCAMLLIB_PATH)" > ./Resources/env.xcconfig
	@echo "BUILD_STORE_PATH=$(BUILD_STORE_PATH)" >> ./Resources/env.xcconfig
	xcodegen --spec project.yml
