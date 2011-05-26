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
#include <Swiften/Elements/JingleTransportPayload.h>

namespace Swift {
	class JingleContentPayload : public Payload {
		public:
			typedef boost::shared_ptr<JingleContentPayload> ref;

			enum Creator {
				UnknownCreator,
				InitiatorCreator,
				ResponderCreator,
			};

			/*enum Senders {
				NoSenders,
				InitiatorSender,
				ResponderSender,
				BothSenders,
			};*/

			Creator getCreator() const {
				return creator;
			}

			void setCreator(Creator creator) {
				this->creator = creator;
			}

			const std::string& getName() const {
				return name;
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

			const std::vector<boost::shared_ptr<JingleTransportPayload> >& getTransports() const {
				return transports;
			}

			void addTransport(boost::shared_ptr<JingleTransportPayload>  transport) {
				transports.push_back(transport);
			}

			template<typename T>
			boost::shared_ptr<T> getDescription() const {
				for (size_t i = 0; i < descriptions.size(); ++i) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(descriptions[i]));
					if (result) {
						return result;
					}
				}
				return boost::shared_ptr<T>();
			}

			template<typename T>
			boost::shared_ptr<T> getTransport() const {
				for (size_t i = 0; i < transports.size(); ++i) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(transports[i]));
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
			std::vector<boost::shared_ptr<JingleTransportPayload> > transports;
	};
}
