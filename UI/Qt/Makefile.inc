TARGETS += qt
CLEAN_TARGETS += clean-qt

.PHONY: qt
qt: UI/Qt/Makefile
	$(MAKE) -C UI/Qt

.PHONY: clean-qt
	if [ -f "UI/Qt/Makefile" ]; then \
		$(MAKE) -C UI/Qt clean; \
	fi

UI/Qt/Makefile: UI/Qt/Swift.pro
	cd UI/Qt && $(QMAKE) Swift.pro

UI/Qt/Swift.pro: UI/Qt/DefaultTheme.qrc

UI/Qt/DefaultTheme.qrc:
	cd UI/Qt && ../../tools/ThemeQRC.py ../../resources/themes/Default > DefaultTheme.qrc
