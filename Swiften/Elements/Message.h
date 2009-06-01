#ifndef SWIFTEN_STANZAS_MESSAGE_H
#define SWIFTEN_STANZAS_MESSAGE_H

#include <boost/optional.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Body.h"
#include "Swiften/Elements/Error.h"
#include "Swiften/Elements/Stanza.h"

namespace Swift
{
	class Message : public Stanza
	{
	  public:
			enum Type { Normal, Chat, Error, Groupchat, Headline };

			Message() : type_(Chat) { }

			String getBody() const { 
				boost::shared_ptr<Body> body(getPayload<Body>());
				if (body) {
					return body->getText();
				}
				return "";
			}

			void setBody(const String& body) { 
				updatePayload(boost::shared_ptr<Body>(new Body(body)));
			}

			bool isError() {
				boost::shared_ptr<Swift::Error> error(getPayload<Swift::Error>());
				return getType() == Message::Error || error.get() != NULL;
			}

			Type getType() const { return type_; }
			void setType(Type type) { type_ = type; }

		private:
			String body_;
			Type type_;
	};
}

#endif
