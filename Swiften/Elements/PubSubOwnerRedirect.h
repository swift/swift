/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <string>



namespace Swift {
	class SWIFTEN_API PubSubOwnerRedirect : public Payload {
		public:
			
			PubSubOwnerRedirect();
			
			virtual ~PubSubOwnerRedirect();

			const std::string& getURI() const {
				return uri;
			}

			void setURI(const std::string& value) {
				this->uri = value ;
			}


		private:
			std::string uri;
	};
}
