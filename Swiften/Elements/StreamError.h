/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ToplevelElement.h>

namespace Swift {
    class SWIFTEN_API StreamError : public ToplevelElement {
        public:
            typedef std::shared_ptr<StreamError> ref;

            enum Type {
                BadFormat,
                BadNamespacePrefix,
                Conflict,
                ConnectionTimeout,
                HostGone,
                HostUnknown,
                ImproperAddressing,
                InternalServerError,
                InvalidFrom,
                InvalidID,
                InvalidNamespace,
                InvalidXML,
                NotAuthorized,
                NotWellFormed,
                PolicyViolation,
                RemoteConnectionFailed,
                Reset,
                ResourceConstraint,
                RestrictedXML,
                SeeOtherHost,
                SystemShutdown,
                UndefinedCondition,
                UnsupportedEncoding,
                UnsupportedStanzaType,
                UnsupportedVersion
            };

            StreamError(Type type = UndefinedCondition, const std::string& text = std::string()) : type_(type), text_(text) { }

            Type getType() const {
                return type_;
            }

            void setType(Type type) {
                type_ = type;
            }

            void setText(const std::string& text) {
                text_ = text;
            }

            const std::string& getText() const {
                return text_;
            }

        private:
            Type type_;
            std::string text_;
    };
}
