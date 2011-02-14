/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "SwifTools/Notifier/Notifier.h"

namespace Swift {
	class TogglableNotifier : public Notifier {
		public:
			TogglableNotifier(Notifier* notifier) : notifier(notifier), persistentEnabled(true), temporarilyDisabled(false) {
			}

			/**
			 * Set a long-term (usually user-set) enabled.
			 * This may be temporarily overriden by the application, e.g. if the 
			 * user is marked DND.
			 */
			void setPersistentEnabled(bool b) {
				persistentEnabled = b;
			}

			/**
			 * Set a temporary override to stop notifications without changing the 
			 * long-term state. e.g. if the user goes DND, but the persistent 
			 * enabled shouldn't be lost when they become available again.
			 */
			void setTemporarilyDisabled(bool b) {
				temporarilyDisabled = b;
			}

			/**
			 * Get the result of applying the temporary override to the persistent 
			 * enabledness.
			 */
			bool getCurrentlyEnabled() const {
				return persistentEnabled && !temporarilyDisabled;
			}
 
			virtual void showMessage(Type type, const std::string& subject, const std::string& description, const boost::filesystem::path& picture, boost::function<void()> callback) {
				if (getCurrentlyEnabled()) {
					notifier->showMessage(type, subject, description, picture, callback);
				}
			}

		private:
			Notifier* notifier;
			bool persistentEnabled;
			bool temporarilyDisabled;
	};
}
