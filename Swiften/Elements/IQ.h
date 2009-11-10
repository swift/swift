#pragma once

#include "Swiften/Elements/Stanza.h"
#include "Swiften/Elements/ErrorPayload.h"

namespace Swift 
{
	class IQ : public Stanza
	{
		public: 
			enum Type { Get, Set, Result, Error };

			IQ(Type type = Get) : type_(type) { }

			Type getType() const { return type_; }
			void setType(Type type) { type_ = type; }

			static boost::shared_ptr<IQ> createRequest(
					Type type,
					const JID& to,
					const String& id,
					boost::shared_ptr<Payload> payload);
			static boost::shared_ptr<IQ> createResult(
					const JID& to, 
					const String& id, 
					boost::shared_ptr<Payload> payload = boost::shared_ptr<Payload>());
			static boost::shared_ptr<IQ> createError(
					const JID& to,
					const String& id,
					ErrorPayload::Condition condition,
					ErrorPayload::Type type);

		private:
			Type type_;
	};
}
