ifdef USE_VALGRIND
# Not enabled: --show-reachable=yes
TEST_RUNNER=valgrind --suppressions=QA/valgrind.supp -q --leak-check=full --track-origins=yes
endif

include QA/UnitTest/Makefile.inc

.PHONY: test
test: $(TEST_TARGETS)
