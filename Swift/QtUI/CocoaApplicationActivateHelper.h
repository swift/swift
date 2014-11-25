/*
 * Copyright (c) 2012 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QObject>

namespace Swift {
	/**
	 * This class is here as a workaround for a bug in Qt.
	 * See Case #501.
	 */
	class CocoaApplicationActivateHelper : public QObject {
		public:
			CocoaApplicationActivateHelper();
			virtual ~CocoaApplicationActivateHelper();

		private:
			bool eventFilter(QObject* o, QEvent* e);

		private:
			struct Private;
			Private* p;
	};
}
