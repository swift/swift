#include "Slimber/FileVCardCollection.h"

#include <boost/filesystem/fstream.hpp>

#include "Swiften/Elements/VCard.h"
#include "Swiften/Serializer/PayloadSerializers/VCardSerializer.h"

namespace Swift {

FileVCardCollection::FileVCardCollection(boost::filesystem::path dir) : vcardsPath(dir) {
	vcard = boost::shared_ptr<VCard>(new VCard());
}

boost::shared_ptr<VCard> FileVCardCollection::getOwnVCard() const {
	return vcard;
}

void FileVCardCollection::setOwnVCard(boost::shared_ptr<VCard> v) {
	boost::filesystem::ofstream file(vcardsPath / std::string("vcard.xml"));
	file << VCardSerializer().serializePayload(v);
	file.close();
}

}
