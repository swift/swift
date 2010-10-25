/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Body.h"
#include "Swiften/Elements/Subject.h"
#include "Swiften/Elements/ErrorPayload.h"
#include "Swiften/Elements/Stanza.h"

namespace Swift {
	class Message : public Stanza {
	  public:
			typedef boost::shared_ptr<Message> ref;

			enum Type { Normal, Chat, Error, Groupchat, Headline };

			Message() : type_(Chat) { }

			String getSubject() const { 
				boost::shared_ptr<Subject> subject(getPayload<Subject>());
				if (subject) {
					return subject->getText();
				}
				return "";
			}

			void setSubject(const String& subject) { 
				updatePayload(boost::shared_ptr<Subject>(new Subject(subject)));
			}

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
				boost::shared_ptr<Swift::ErrorPayload> error(getPayload<Swift::ErrorPayload>());
				return getType() == Message::Error || error;
			}

			Type getType() const { return type_; }
			void setType(Type type) { type_ = type; }

		private:
			Type type_;
	};
}
