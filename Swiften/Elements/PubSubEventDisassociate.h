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
	class SWIFTEN_API PubSubEventDisassociate : public Payload {
		public:
			
			PubSubEventDisassociate();
			
			virtual ~PubSubEventDisassociate();

			const std::string& getNode() const {
				return node;
			}

			void setNode(const std::string& value) {
				this->node = value ;
			}


		private:
			std::string node;
	};
}
