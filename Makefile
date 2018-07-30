
NUM_CPU := $(shell python -c 'import multiprocessing; print(multiprocessing.cpu_count())')

.PHONY: all
all: build.ninja
	ninja

.PHONY: clang-tidy
clang-tidy: compile_commands.json
	ninja -t targets all | grep cxx | grep -v 3rdParty | sed -e's/\.o: cxx/.cpp/' | xargs -t -P ${NUM_CPU} -n 1 clang-tidy -warnings-as-errors=*

.PHONY: clang-tidy-fix
clang-tidy-fix: compile_commands.json
	ninja -t targets all | grep cxx | grep -v 3rdParty | sed -e's/\.o: cxx/.cpp/' | xargs -t -P ${NUM_CPU} -n 1 -I{} clang-tidy -p=${PWD} -export-fixes={}.clang-tidy-changes.yaml {}
	clang-apply-replacements -remove-change-desc-files ${PWD}

compile_commands.json: build.ninja
	ninja -t compdb cxx > compile_commands.json

build.ninja:
	./BuildTools/scons2ninja.py check=1 build_examples=0
