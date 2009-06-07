TARGETS += qt
CLEAN_TARGETS += clean-qt

ifeq ($(WIN32),1)
QT_MAKE=nmake
else
QT_MAKE=$(MAKE)
endif


.PHONY: qt
qt: UI/Qt/Makefile
	cd UI/Qt && $(QT_MAKE)

.PHONY: clean-qt
clean-qt:
	if [ -f "UI/Qt/Makefile" ]; then \
		cd UI/Qt && $(QT_MAKE) clean; \
	fi

UI/Qt/Makefile: UI/Qt/DefaultTheme.qrc UI/Qt/Swiften.pri
	cd UI/Qt && $(QMAKE) Swift.pro

UI/Qt/Swiften.pri:
	cd UI/Qt && ./qmakeish.py ../../Makefile > Swiften.pri

UI/Qt/DefaultTheme.qrc:
	cd UI/Qt && ../../tools/ThemeQRC.py ../../resources/themes/Default > DefaultTheme.qrc
