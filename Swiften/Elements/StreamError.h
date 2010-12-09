/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Element.h>
#include <Swiften/Base/String.h>

namespace Swift {
	class StreamError : public Element {
		public:
			typedef boost::shared_ptr<StreamError> ref;

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
				UnsupportedVersion,
			};

			StreamError(Type type = UndefinedCondition, const String& text = String()) : type_(type), text_(text) { }

			Type getType() const {
				return type_; 
			}

			void setType(Type type) {
				type_ = type;
			}

			void setText(const String& text) {
				text_ = text;
			}

			const String& getText() const { 
				return text_; 
			}

		private:
			Type type_;
			String text_;
	};
}
