/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class SWIFTEN_API PubSubEventItem : public Payload {
		public:
			
			PubSubEventItem();
			
			virtual ~PubSubEventItem();

			const boost::optional< std::string >& getNode() const {
				return node;
			}

			void setNode(const boost::optional< std::string >& value) {
				this->node = value ;
			}

			const boost::optional< std::string >& getPublisher() const {
				return publisher;
			}

			void setPublisher(const boost::optional< std::string >& value) {
				this->publisher = value ;
			}

			const std::vector< boost::shared_ptr<Payload> >& getData() const {
				return data;
			}

			void setData(const std::vector< boost::shared_ptr<Payload> >& value) {
				this->data = value ;
			}

			void addData(boost::shared_ptr<Payload> value) {
				this->data.push_back(value);
			}

			const boost::optional< std::string >& getID() const {
				return id;
			}

			void setID(const boost::optional< std::string >& value) {
				this->id = value ;
			}


		private:
			boost::optional< std::string > node;
			boost::optional< std::string > publisher;
			std::vector< boost::shared_ptr<Payload> > data;
			boost::optional< std::string > id;
	};
}
