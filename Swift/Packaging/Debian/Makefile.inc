DATE=`date +%Y%m%d`
SIZE=`du -s tmpusr/usr | awk '{print $1}'`
VERSION=0.3.$(DATE)
DEBPATH=Swift/Packaging/Debian
DEBNAME=swift-preview_$(VERSION)-$(ARCH).deb
$(DEBPATH)/$(DEBNAME): all
	rm -rf $(DEBPATH)/tmpusr
	mkdir -p $(DEBPATH)/tmpusr/usr/bin
	mkdir $(DEBPATH)/tmpusr/DEBIAN

	cp Swift/QtUI/swift $(DEBPATH)/tmpusr/usr/bin

	cat $(DEBPATH)/control | sed -e"s/\%ARCH\%/$(ARCH)/" -e"s/\%VERSION\%/$(VERSION)/" -e"s/\%SIZE\%/$(SIZE)/" > $(DEBPATH)/tmpusr/DEBIAN/control

	dpkg -b $(DEBPATH)/tmpusr $(DEBPATH)/$(DEBNAME)

.PHONY: deb
deb: $(DEBPATH)/$(DEBNAME)

