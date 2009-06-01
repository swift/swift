include Makefile.config

CXXFLAGS += -I.
ARFLAGS = rcs

.DEFAULT_GOAL = all

################################################################################
# Echoing
################################################################################

ifeq ($(shell echo -e),-e)
ECHO_E=
else
ECHO_E=-e
endif
ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
ifeq ($(C),0)
QUIET_MM   = @echo "    " "MM  " $@;
QUIET_CC   = @echo "    " "CC  " $@;
QUIET_CXX  = @echo "    " "CXX " $@;
QUIET_AR   = @echo "    " "AR  " $@;
QUIET_LINK = @echo "    " "LINK" $@;
else
QUIET_MM   = @echo $(ECHO_E) "    \033[0;35;140m" "MM  " "\033[0m" $@;
QUIET_CC   = @echo $(ECHO_E) "    \033[0;33;140m" "CC  " "\033[0m" $@;
QUIET_CXX  = @echo $(ECHO_E) "    \033[0;32;140m" "CXX " "\033[0m" $@;
QUIET_AR   = @echo $(ECHO_E) "    \033[0;31;140m" "AR  " "\033[0m" $@;
QUIET_LINK = @echo $(ECHO_E) "    \033[0;36;140m" "LINK" "\033[0m" $@;
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
include Swiften/Makefile.inc
include UI/Qt/Makefile.inc

################################################################################
# Main targets
################################################################################

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean: clean-deps $(CLEANTARGETS)
	-$(RM) $(CLEANFILES) \
		$(SWIFTEN_SOURCES:.cpp=.gcda) \
		$(SWIFTEN_SOURCES:.cpp=.gcno) \
		$(UNITTEST_SOURCES:.cpp=.gcda) \
		$(UNITTEST_SOURCES:.cpp=.gcno) \
		*.gcov

.PHONY: clean-deps
clean-deps:
		-$(RM) $(SWIFTEN_SOURCES:.cpp=.dep) $(SWIFTEN_OBJECTIVE_SOURCES:.mm=.dep) $(UNITTEST_SOURCES:.cpp=.dep)


################################################################################
# Automatic dependency detection
################################################################################

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
ifeq (,$(findstring clean-deps, $(MAKECMDGOALS)))
-include $(SWIFTEN_SOURCES:.cpp=.dep)
-include $(SWIFTEN_OBJECTIVE_SOURCES:.mm=.dep)
-include $(UNITTEST_SOURCES:.cpp=.dep)
endif
endif

%.dep: %.cpp
	$(QUIET_MM)$(CXX) -MM -MG -MT $(basename $@).o $(filter-out -arch armv6 -arch i386 -arch ppc,$(CXXFLAGS)) $< > $@

%.dep: %.c
	$(QUIET_MM)$(CC) -MM -MG -MT $(basename $@).o $(filter-out -arch armv6 -arch i386 -arch ppc,$(CFLAGS)) $< > $@

%.dep: %.mm
	$(QUIET_MM)$(CC) -MM -MG -MT $(basename $@).o $(filter-out -arch armv6 -arch i386 -arch ppc,$(CXXFLAGS)) $< > $@

%.o: %.c
	$(QUIET_CC)$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

%.o: %.cpp
	$(QUIET_CXX)$(CXX) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)

%.o: %.mm
	$(QUIET_CC)$(CC) -c $< -o $@ $(CPPFLAGS) $(CXXFLAGS)
