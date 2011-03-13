/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/optional/optional_fwd.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <Swiften/Elements/Element.h>
#include <Swiften/JID/JID.h>

namespace Swift {
	class Payload;

	class Stanza : public Element {
		public:
			typedef boost::shared_ptr<Stanza> ref;

			Stanza();
			virtual ~Stanza();

			template<typename T> 
			boost::shared_ptr<T> getPayload() const {
				for (size_t i = 0; i < payloads_.size(); ++i) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(payloads_[i]));
					if (result) {
						return result;
					}
				}
				return boost::shared_ptr<T>();
			}

			template<typename T> 
			std::vector< boost::shared_ptr<T> > getPayloads() const {
				std::vector< boost::shared_ptr<T> > results;
				for (size_t i = 0; i < payloads_.size(); ++i) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(payloads_[i]));
					if (result) {
						results.push_back(result);
					}
				}
				return results;
			}


			const std::vector< boost::shared_ptr<Payload> >& getPayloads() const {
				return payloads_;
			}

			void addPayload(boost::shared_ptr<Payload> payload) {
				payloads_.push_back(payload);
			}

			void updatePayload(boost::shared_ptr<Payload> payload);

			boost::shared_ptr<Payload> getPayloadOfSameType(boost::shared_ptr<Payload>) const;

			const JID& getFrom() const { return from_; }
			void setFrom(const JID& from) { from_ = from; }

			const JID& getTo() const { return to_; }
			void setTo(const JID& to) { to_ = to; }

			const std::string& getID() const { return id_; }
			void setID(const std::string& id) { id_ = id; }

			boost::optional<boost::posix_time::ptime> getTimestamp() const;

			// Falls back to any timestamp if no specific timestamp for the given JID is found.
			boost::optional<boost::posix_time::ptime> getTimestampFrom(const JID& jid) const;
	
		private:
			std::string id_;
			JID from_;
			JID to_;
			std::vector< boost::shared_ptr<Payload> > payloads_;
	};
}
