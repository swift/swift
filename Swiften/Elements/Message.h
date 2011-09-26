/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <string>
#include <Swiften/Elements/Body.h>
#include <Swiften/Elements/Subject.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/Replace.h>

namespace Swift {
	class Message : public Stanza {
	  public:
			typedef boost::shared_ptr<Message> ref;

			enum Type { Normal, Chat, Error, Groupchat, Headline };

			Message() : type_(Chat) { }

			std::string getSubject() const { 
				boost::shared_ptr<Subject> subject(getPayload<Subject>());
				if (subject) {
					return subject->getText();
				}
				return "";
			}

			void setSubject(const std::string& subject) { 
				updatePayload(boost::make_shared<Subject>(subject));
			}

			bool hasSubject() {
				return getPayload<Subject>();
			}

			std::string getBody() const { 
				boost::shared_ptr<Body> body(getPayload<Body>());
				if (body) {
					return body->getText();
				}
				return "";
			}

			void setBody(const std::string& body) { 
				updatePayload(boost::make_shared<Body>(body));
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
