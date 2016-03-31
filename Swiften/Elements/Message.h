/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Body.h>
#include <Swiften/Elements/ErrorPayload.h>
#include <Swiften/Elements/Replace.h>
#include <Swiften/Elements/Stanza.h>
#include <Swiften/Elements/Subject.h>

namespace Swift {
    class SWIFTEN_API Message : public Stanza {
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

            // Explicitly convert to bool. In C++11, it would be cleaner to
            // compare to nullptr.
            bool hasSubject() {
                return static_cast<bool>(getPayload<Subject>());
            }

            boost::optional<std::string> getBody() const {
                boost::shared_ptr<Body> body(getPayload<Body>());
                boost::optional<std::string> bodyData;
                if (body) {
                    bodyData = body->getText();
                }
                return bodyData;
            }

            void setBody(const std::string& body) {
                setBody(boost::optional<std::string>(body));
            }

            void setBody(const boost::optional<std::string>& body) {
                if (body) {
                    updatePayload(boost::make_shared<Body>(body.get()));
                }
                else {
                    removePayloadOfSameType(boost::make_shared<Body>());
                }
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
