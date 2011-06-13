/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class ErrorPayload : public Payload {
		public:
			typedef boost::shared_ptr<ErrorPayload> ref;

			enum Type { Cancel, Continue, Modify, Auth, Wait };

			enum Condition {
				BadRequest,
				Conflict,
				FeatureNotImplemented,
				Forbidden,
				Gone,
				InternalServerError,
				ItemNotFound,
				JIDMalformed,
				NotAcceptable,
				NotAllowed,
				NotAuthorized,
				PaymentRequired,
				RecipientUnavailable,
				Redirect,
				RegistrationRequired,
				RemoteServerNotFound,
				RemoteServerTimeout,
				ResourceConstraint,
				ServiceUnavailable,
				SubscriptionRequired,
				UndefinedCondition,
				UnexpectedRequest
			};

			ErrorPayload(Condition condition = UndefinedCondition, Type type = Cancel, const std::string& text = std::string()) : type_(type), condition_(condition), text_(text) { }

			Type getType() const {
				return type_; 
			}

			void setType(Type type) {
				type_ = type;
			}

			Condition getCondition() const { 
				return condition_; 
			}

			void setCondition(Condition condition) { 
				condition_ = condition;
			}

			void setText(const std::string& text) {
				text_ = text;
			}

			const std::string& getText() const { 
				return text_; 
			}

			void setPayload(boost::shared_ptr<Payload> payload) {
				payload_ = payload;
			}

			boost::shared_ptr<Payload> getPayload() const {
				return payload_;
			}

		private:
			Type type_;
			Condition condition_;
			std::string text_;
			boost::shared_ptr<Payload> payload_;
	};
}
