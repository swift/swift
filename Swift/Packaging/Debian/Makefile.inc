DATE=`date +%Y%m%d`
SIZE=`du -s tmpusr/usr | awk '{print $1}'`
VERSION=0.3.$DATE
ARCH=amd64
DEBPATH=Swift/Packaging/Debian
DEBNAME=swift-preview_$(VERSION)-$(ARCH).deb
$(DEBPATH)/$(DEBNAME): all
	echo "Removing old package files"
	rm -rf $(DEBPATH)/tmpusr
	mkdir -p $(DEBPATH)/tmpusr/usr/bin
	mkdir $(DEBPATH)/tmpusr/DEBIAN

	echo "Building fake filesystem"
	cp Swift/QtUI/swift $(DEBPATH)/tmpusr/usr/bin

	echo "Populating debian control scripts"
	cat $(DEBPATH)/control | sed -e"s/\%ARCH\%/$ARCH/" -e"s/\%VERSION\%/$VERSION/" -e"s/\%SIZE\%/$SIZE/" > $(DEBPATH)/tmpusr/DEBIAN/control

	echo "Building package"
	dpkg -b $(DEBPATH)tmpusr $(DEBPATH)/$(DEBNAME)

deb: $(DEBPATH)/$(DEBNAME)

.PHONY: deb
