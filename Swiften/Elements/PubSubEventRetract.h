/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <string>



namespace Swift {
	class SWIFTEN_API PubSubEventRetract : public Payload {
		public:
			
			PubSubEventRetract();
			
			virtual ~PubSubEventRetract();

			const std::string& getID() const {
				return id;
			}

			void setID(const std::string& value) {
				this->id = value ;
			}


		private:
			std::string id;
	};
}
