#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include "Slimber/VCardCollection.h"

namespace Swift {
	class FileVCardCollection : public VCardCollection {
		public:
			FileVCardCollection(boost::filesystem::path dir);

			boost::shared_ptr<VCard> getOwnVCard() const;
			void setOwnVCard(boost::shared_ptr<VCard> vcard);

		private:
			boost::filesystem::path vcardsPath;
	};
}
