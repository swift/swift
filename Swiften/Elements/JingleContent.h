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
<<<<<<< HEAD:Swiften/Elements/JingleContent.h
			std::vector<JingleTransport::ref> transports;
=======
			std::vector<boost::shared_ptr<JingleTransportPayload> > transports;
>>>>>>> 7c05f3f... Cleaned up headers.:Swiften/Elements/JingleContentPayload.h
	};
}
