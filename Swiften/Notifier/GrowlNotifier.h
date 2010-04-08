/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <Growl/Growl.h>

#include "Swiften/Notifier/Notifier.h"

namespace Swift {
	/**
	 * Preconditions for using growlnotifier:
	 * - Must be part a bundle.
	 * - The Carbon/Cocoa application loop must be running (e.g. through QApplication)
	 *   such that notifications are coming through.
	 *   TODO: Find out what the easiest way is to do this without a QApplication.
	 */
	class GrowlNotifier : public Notifier {
		public:
			GrowlNotifier(const String& name);

			virtual void showMessage(Type type, const String& subject, const String& description, const ByteArray& picture, boost::function<void()> callback);
		
		private:
			String typeToString(Type type);

		private:
			Growl_Delegate delegate_;
	};
}
