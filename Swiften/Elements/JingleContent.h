/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/JingleDescription.h>
#include <Swiften/Elements/JingleTransport.h>
#include <Swiften/Base/foreach.h>

namespace Swift {
	class JingleContent : public Payload {
		public:
			typedef boost::shared_ptr<JingleContent> ref;

			enum Creator {
				InitiatorCreator,
				ResponderCreator,
			};

			/*enum Senders {
				NoSenders,
				InitiatorSender,
				ResponderSender,
				BothSenders,
			};*/

			void setCreator(Creator creator) {
				this->creator = creator;
			}

			void setName(const std::string& name) {
				this->name = name;
			}

			const std::vector<JingleDescription::ref>& getDescriptions() const {
				return descriptions;
			}

			void addDescription(JingleDescription::ref description) {
				descriptions.push_back(description);
			}

			const std::vector<JingleTransport::ref>& getTransports() const {
				return transports;
			}

			void addTransport(JingleTransport::ref transport) {
				transports.push_back(transport);
			}

			template<typename T>
			boost::shared_ptr<T> getDescription() const {
				foreach (JingleDescription::ref i, descriptions) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(i));
					if (result) {
						return result;
					}
				}
				return boost::shared_ptr<T>();
			}

			template<typename T>
			boost::shared_ptr<T> getTransport() const {
				foreach (JingleTransport::ref i, transports) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(i));
					if (result) {
						return result;
					}
				}
				return boost::shared_ptr<T>();
			}

		private:
			Creator creator;
			std::string name;
			//Senders senders;
			std::vector<JingleDescription::ref> descriptions;
			std::vector<JingleTransport::ref> transports;
	};
}
