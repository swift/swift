/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QLabel>

namespace Swift {
	class QtClickableLabel : public QLabel {
			Q_OBJECT
		public:
			QtClickableLabel(QWidget* parent);

			void mousePressEvent(QMouseEvent* event);

		signals:
			void clicked();
	};
}
