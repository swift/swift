ifdef USE_VALGRIND
# Not enabled: --show-reachable=yes
TEST_RUNNER=valgrind --suppressions=Swiften/QA/valgrind.supp -q --leak-check=full --track-origins=yes
endif

include Swiften/QA/UnitTest/Makefile.inc
include Swiften/QA/NetworkTest/Makefile.inc
include Swiften/QA/ClientTest/Makefile.inc

.PHONY: test
test: $(TEST_TARGETS)
