.PHONY: all
all: build.ninja
	ninja

.PHONY: clang-tidy
clang-tidy: compile_commands.json
	ninja -t targets all | grep cxx | grep -v 3rdParty | sed -e's/\.o: cxx/.cpp/' | xargs -n 1 clang-tidy

compile_commands.json: build.ninja
	ninja -t compdb cxx > compile_commands.json

build.ninja:
	./BuildTools/scons2ninja.py check=1 build_examples=0
