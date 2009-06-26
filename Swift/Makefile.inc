include Swift/Controllers/Makefile.inc
include Swift/QtUI/Makefile.inc
include Swift/Packaging/Debian/Makefile.inc

INSTALL_TARGETS += install-swift

.PHONY: install-swift
install-swift: qt
	install -d $(DESTDIR)$(bindir)
	install Swift/QtUI/swift $(DESTDIR)$(bindir)
