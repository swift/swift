include Makefile.config

CXXFLAGS += -I.
ARFLAGS = rcs

.DEFAULT_GOAL = all

################################################################################
# Echoing
################################################################################

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
ifeq ($(C),0)
QUIET_MM   = @echo "    " "MM  " $@;
QUIET_CC   = @echo "    " "CC  " $@;
QUIET_CXX  = @echo "    " "CXX " $@;
QUIET_AR   = @echo "    " "AR  " $@;
QUIET_LINK = @echo "    " "LINK" $@;
else
QUIET_MM   = @echo "    $(shell tput setaf 5)MM$(shell tput sgr0)  " $@;
QUIET_CC   = @echo "    $(shell tput setaf 3)CC$(shell tput sgr0)  " $@;
QUIET_CXX  = @echo "    $(shell tput setaf 2)CXX$(shell tput sgr0) " $@;
QUIET_AR   = @echo "    $(shell tput setaf 1)AR$(shell tput sgr0)  " $@;
QUIET_LINK = @echo "    $(shell tput setaf 6)LINK$(shell tput sgr0)" $@;
endif
endif
endif

################################################################################
# Modules
################################################################################

include 3rdParty/Boost/Makefile.inc
include 3rdParty/CppUnit/Makefile.inc
include 3rdParty/LibIDN/Makefile.inc
include 3rdParty/ZLib/Makefile.inc
include 3rdParty/Expat/Makefile.inc
include 3rdParty/SQLite/Makefile.inc
include Swiften/Makefile.inc
ifeq ($(BUILD_SWIFT),yes)
include Swift/Makefile.inc
endif
ifeq ($(BUILD_LIMBER),yes)
include Limber/Makefile.inc
endif
ifeq ($(BUILD_SLIMBER),yes)
include Slimber/Makefile.inc
endif
include QA/Makefile.inc
################################################################################
# Main targets
################################################################################

.PHONY: all
all: $(TARGETS)

.PHONY: install
install: $(INSTALL_TARGETS)

.PHONY: coverage
coverage:
	tools/coverage/GenerateCoverageResults.sh

.PHONY: clean
clean: clean-deps $(CLEAN_TARGETS)

	-$(RM) $(CLEANFILES) *.gcov
	-find . -name "*.gcda" -exec rm {} \;
	-find . -name "*.gcno" -exec rm {} \;

.PHONY: clean-deps
clean-deps:
		-$(RM) $(DEPS)


################################################################################
# Automatic dependency detection
################################################################################

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (,$(findstring clean-deps, $(MAKECMDGOALS)))
-include $(DEPS)
endif
endif

%.dep: %.cpp
	$(QUIET_MM)$(MM) -MM -MG -MT $(basename $@).o $(CPPFLAGS) $(filter-out -arch armv6 -arch i386 -arch ppc,$(CXXFLAGS)) $< > $@

%.dep: %.c
	$(QUIET_MM)$(MM) -MM -MG -MT $(basename $@).o $(CPPFLAGS) $(filter-out -arch armv6 -arch i386 -arch ppc,$(CFLAGS)) $< > $@

%.dep: %.mm
	$(QUIET_MM)$(CC) -MM -MG -MT $(basename $@).o $(CPPFLAGS) $(filter-out -arch armv6 -arch i386 -arch ppc,$(CXXFLAGS)) $< > $@

%.o: %.c
	$(QUIET_CC)$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

%.o: %.cpp
	$(QUIET_CXX)$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)

%.o: %.mm
	$(QUIET_CC)$(CC) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)
