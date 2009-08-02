SLIMBER_COCOA_TARGET = Slimber/Cocoa/Slimber.app
SLIMBER_COCOA_BINARY = \
	Slimber/Cocoa/Slimber
SLIMBER_COCOA_SOURCES = \
	Slimber/Cocoa/main.mm \
	Slimber/Cocoa/CocoaController.mm \
	Slimber/Cocoa/CocoaMenulet.mm \
	Slimber/Cocoa/CocoaAction.mm
SLIMBER_COCOA_XIBS = \
	Slimber/Cocoa/MainMenu.xib
SLIMBER_COCOA_RESOURCES = \
	Slimber/Resources/Slimber.icns \
	Slimber/Resources/Credits.html \
	Slimber/Resources/Online.png \
	Slimber/Resources/Offline.png \
	Slimber/Resources/UsersOnline.png \
	Slimber/Resources/UsersOffline.png

TARGETS += $(SLIMBER_COCOA_TARGET)
SLIMBER_TARGETS += $(SLIMBER_COCOA_TARGET)
SLIMBER_COCOA_NIBS = \
	$(SLIMBER_COCOA_XIBS:.xib=.nib)
SLIMBER_COCOA_OBJECTS = \
	$(patsubst %.m,%.o,$(patsubst %.mm,%.o,$(patsubst %.cpp,%.o, $(SLIMBER_COCOA_SOURCES))))
CLEANFILES += \
	Slimber/Cocoa/PkgInfo \
	$(SLIMBER_COCOA_NIBS) \
	$(SLIMBER_COCOA_TARGET) \
	$(SLIMBER_COCOA_BINARY)
DEPS += \
	$(patsubst %.m, %.dep, $(patsubst %.mm, %.dep, $(SLIMBER_COCOA_SOURCES)))

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

$(SLIMBER_COCOA_BINARY): $(SLIMBER_COCOA_OBJECTS) $(SWIFTEN_TARGET) $(SLIMBER_TARGET)
	$(QUIET_LINK)$(CXX) -o $@ $(SLIMBER_COCOA_OBJECTS) $(LDFLAGS) $(SLIMBER_TARGET) $(SWIFTEN_TARGET) $(LIBS) -framework Cocoa
