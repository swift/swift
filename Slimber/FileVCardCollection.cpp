#include "Slimber/FileVCardCollection.h"
#include "Swiften/Elements/VCard.h"

namespace Swift {

FileVCardCollection::FileVCardCollection(boost::filesystem::path dir) : vcardsPath(dir) {
	vcard = boost::shared_ptr<VCard>(new VCard());
}

boost::shared_ptr<VCard> FileVCardCollection::getOwnVCard() const {
	return vcard;
}

void FileVCardCollection::setOwnVCard(boost::shared_ptr<VCard> v) {
	vcard = v;
}

}
