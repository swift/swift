#ifndef SWIFTEN_STANZAS_STANZA_H
#define SWIFTEN_STANZAS_STANZA_H

#include <vector>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Element.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/JID/JID.h"

namespace Swift {
	class Stanza : public Element {
		public:
			virtual ~Stanza();

			template<typename T> 
			boost::shared_ptr<T> getPayload() const {
				foreach (const boost::shared_ptr<Payload>& i, payloads_) {
					boost::shared_ptr<T> result(boost::dynamic_pointer_cast<T>(i));
					if (result) {
						return result;
					}
				}
				return boost::shared_ptr<T>();
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

			const String& getID() const { return id_; }
			void setID(const String& id) { id_ = id; }
	
		private:
			String id_;
			JID from_;
			JID to_;

			typedef std::vector< boost::shared_ptr<Payload> > Payloads;
			Payloads payloads_;
	};
}

#endif
