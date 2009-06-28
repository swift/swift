TARGETS += qt
CLEAN_TARGETS += clean-qt

ifeq ($(WIN32),1)
QT_MAKE=nmake
else
QT_MAKE=$(MAKE)
endif


.PHONY: qt
qt: Swift/QtUI/Makefile Swiften/Swiften.a $(BUNDLED_LIBS)
	cd Swift/QtUI && $(QT_MAKE)

.PHONY: clean-qt
clean-qt:
	if [ -f "Swift/QtUI/Makefile" ]; then \
		cd Swift/QtUI && $(QT_MAKE) clean; \
	fi

Swift/QtUI/Makefile: Swift/QtUI/DefaultTheme.qrc Swift/QtUI/Swiften.pri
	cd Swift/QtUI && $(QMAKE) Swift.pro

Swift/QtUI/Swiften.pri:
	cd Swift/QtUI && ./qmakeish.py ../../Makefile > Swiften.pri

Swift/QtUI/DefaultTheme.qrc:
	cd Swift/QtUI && ../../tools/ThemeQRC.py ../resources/themes/Default > DefaultTheme.qrc
