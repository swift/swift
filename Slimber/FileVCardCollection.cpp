#include "Slimber/FileVCardCollection.h"

namespace Swift {

FileVCardCollection::FileVCardCollection(boost::filesystem::path dir) : vcardsPath(dir) {
}

boost::shared_ptr<VCard> FileVCardCollection::getOwnVCard() const {
}

void FileVCardCollection::setOwnVCard(boost::shared_ptr<VCard> vcard) {
}

}
