/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>

namespace Swift {
	class JingleFileTransferDescription : public JingleDescription {
		public:
			typedef boost::shared_ptr<JingleFileTransferDescription> ref;

			void setOffer(const StreamInitiationFileInfo& offer) {
				this->offer = offer;
			}

			const boost::optional<StreamInitiationFileInfo>& getOffer() const {
				return offer;
			}

		private:
			boost::optional<StreamInitiationFileInfo> offer;
	};
}
