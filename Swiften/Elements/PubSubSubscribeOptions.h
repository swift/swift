/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>




namespace Swift {
	class SWIFTEN_API PubSubSubscribeOptions : public Payload {
		public:
			
			PubSubSubscribeOptions();
			
			virtual ~PubSubSubscribeOptions();

			bool isRequired() const {
				return required;
			}

			void setRequired(bool value) {
				this->required = value ;
			}


		private:
			bool required;
	};
}
