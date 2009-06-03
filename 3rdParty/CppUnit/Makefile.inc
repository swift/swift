CXXFLAGS += -isystem 3rdParty/CppUnit

CPPUNIT_TARGET = 3rdParty/CppUnit/CppUnit.a

CPPUNIT_SOURCES = \
  3rdParty/CppUnit/src/TextTestRunner.cpp \
  3rdParty/CppUnit/src/TextTestProgressListener.cpp \
  3rdParty/CppUnit/src/TextOutputter.cpp \
  3rdParty/CppUnit/src/XmlOutputter.cpp \
  3rdParty/CppUnit/src/XmlElement.cpp \
  3rdParty/CppUnit/src/XmlDocument.cpp \
  3rdParty/CppUnit/src/StringTools.cpp \
  3rdParty/CppUnit/src/DefaultProtector.cpp \
  3rdParty/CppUnit/src/Protector.cpp \
  3rdParty/CppUnit/src/ProtectorChain.cpp \
  3rdParty/CppUnit/src/SynchronizedObject.cpp \
  3rdParty/CppUnit/src/SourceLine.cpp \
  3rdParty/CppUnit/src/TestRunner.cpp \
  3rdParty/CppUnit/src/TestFactoryRegistry.cpp \
  3rdParty/CppUnit/src/TestSuite.cpp \
  3rdParty/CppUnit/src/TestSuiteBuilderContext.cpp \
  3rdParty/CppUnit/src/TestResult.cpp \
  3rdParty/CppUnit/src/TestResultCollector.cpp \
  3rdParty/CppUnit/src/TestSuccessListener.cpp \
  3rdParty/CppUnit/src/TestComposite.cpp \
  3rdParty/CppUnit/src/TestCase.cpp \
  3rdParty/CppUnit/src/TestFailure.cpp \
  3rdParty/CppUnit/src/TestLeaf.cpp \
  3rdParty/CppUnit/src/TestNamer.cpp \
  3rdParty/CppUnit/src/Asserter.cpp \
  3rdParty/CppUnit/src/TypeInfoHelper.cpp \
  3rdParty/CppUnit/src/Exception.cpp \
  3rdParty/CppUnit/src/Message.cpp \
  3rdParty/CppUnit/src/AdditionalMessage.cpp \
  3rdParty/CppUnit/src/Test.cpp \
  3rdParty/CppUnit/src/TestPath.cpp

CPPUNIT_OBJECTS = \
	$(CPPUNIT_SOURCES:.cpp=.o)

CLEANFILES += \
	$(CPPUNIT_OBJECTS) \
	$(CPPUNIT_TARGET)

$(CPPUNIT_TARGET): $(CPPUNIT_OBJECTS)
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $(CPPUNIT_OBJECTS)
