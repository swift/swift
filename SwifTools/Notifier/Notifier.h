/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/function.hpp>
#include <boost/filesystem/path.hpp>

#include "Swiften/Base/String.h"

namespace Swift {
	class Notifier {
		public:
			Notifier();
			virtual ~Notifier();

			enum Type { ContactAvailable, ContactUnavailable, ContactStatusChange, IncomingMessage, SystemMessage };

			/**
			 * Picture is a PNG image.
			 */
			virtual void showMessage(
				Type type,
				const String& subject, 
				const String& description, 
				const boost::filesystem::path& picture,
				boost::function<void()> callback);

			void setEnabled(bool b) {
				enabled = b;
			}

			bool getEnabled() const {return enabled;}

		private:
			virtual void doShowMessage(
				Type type,
				const String& subject, 
				const String& description, 
				const boost::filesystem::path& picture,
				boost::function<void()> callback) = 0;

		protected:
			String typeToString(Type type);
			static std::vector<Type> getAllTypes();
			static std::vector<Type> getDefaultTypes();

			static const int DEFAULT_STATUS_NOTIFICATION_TIMEOUT_SECONDS;
			static const int DEFAULT_MESSAGE_NOTIFICATION_TIMEOUT_SECONDS;

		private:
			bool enabled;
	};
}
