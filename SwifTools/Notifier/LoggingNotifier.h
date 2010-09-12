/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "SwifTools/Notifier/Notifier.h"
#include "Swiften/Base/ByteArray.h"

namespace Swift {
	class LoggingNotifier : public Notifier {
		public:
			virtual void showMessage(Type type, const String& subject, const String& description, const ByteArray& picture, boost::function<void()> callback) {
				notifications.push_back(Notification(type, subject, description, picture, callback));
			}

			struct Notification {
					Notification(Type type, const String& subject, const String& description, const ByteArray& picture, boost::function<void()> callback) : type(type), subject(subject), description(description), picture(picture), callback(callback) {}
					Type type;
					String subject;
					String description;
					ByteArray picture;
					boost::function<void()> callback;
			};

			std::vector<Notification> notifications;
	};
}
