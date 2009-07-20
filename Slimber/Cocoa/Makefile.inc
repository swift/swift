SLIMBER_COCOA_TARGET = Slimber/Cocoa/Slimber.app
SLIMBER_COCOA_BINARY = \
	Slimber/Cocoa/Slimber
SLIMBER_COCOA_SOURCES = \
	Slimber/Server.cpp \
	Slimber/Cocoa/main.m \
	Slimber/Cocoa/Menulet.m
SLIMBER_COCOA_XIBS = \
	Slimber/Cocoa/MainMenu.xib
SLIMBER_COCOA_RESOURCES = \
	Slimber/Resources/Online.png \
	Slimber/Resources/Offline.png

SLIMBER_COCOA_NIBS = \
	$(SLIMBER_COCOA_XIBS:.xib=.nib)
SLIMBER_COCOA_OBJECTS = \
	$(patsubst %.m,%.o,$(patsubst %.cpp,%.o, $(SLIMBER_COCOA_SOURCES)))
CLEANFILES += \
	Slimber/Cocoa/PkgInfo \
	$(SLIMBER_COCOA_OBJECTS) \
	$(SLIMBER_COCOA_NIBS) \
	$(SLIMBER_COCOA_TARGET) \
	$(SLIMBER_COCOA_BINARY)
//DEPS += \
//	$(SLIMBER_COCOA_SOURCES:.m=.dep) \

.PHONY: slimber-cocoa
slimber-cocoa: $(SLIMBER_COCOA_TARGET)

$(SLIMBER_COCOA_TARGET): $(SLIMBER_COCOA_BINARY) $(SLIMBER_COCOA_NIBS) Slimber/Cocoa/Info.plist Slimber/Cocoa/PkgInfo
	-rm -rf $(SLIMBER_COCOA_TARGET)
	mkdir -p $(SLIMBER_COCOA_TARGET)/Contents/Resources
	mkdir -p $(SLIMBER_COCOA_TARGET)/Contents/MacOS
	cp -r Slimber/Cocoa/Info.plist $(SLIMBER_COCOA_TARGET)/Contents
	cp -r Slimber/Cocoa/PkgInfo $(SLIMBER_COCOA_TARGET)/Contents
	cp $(SLIMBER_COCOA_BINARY) $(SLIMBER_COCOA_TARGET)/Contents/MacOS
	cp $(SLIMBER_COCOA_NIBS) $(SLIMBER_COCOA_TARGET)/Contents/Resources
	cp $(SLIMBER_COCOA_RESOURCES) $(SLIMBER_COCOA_TARGET)/Contents/Resources

$(SLIMBER_COCOA_BINARY): $(SLIMBER_COCOA_OBJECTS) $(SWIFTEN_TARGET)
	$(QUIET_LINK)$(CXX) -o $@ $(SLIMBER_COCOA_OBJECTS) $(LDFLAGS) $(SWIFTEN_TARGET) $(LIBS) -framework Cocoa
