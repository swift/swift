/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>
#include <QImage>
#include <Swiften/Base/ByteArray.h>

class QLabel;

namespace Swift {
	class QtAvatarWidget : public QWidget {
			Q_OBJECT
		public:
			QtAvatarWidget(QWidget* parent);

			void setAvatar(const ByteArray& data, const String& type);

			const ByteArray& getAvatarData() const {
				return data;
			}

			const String& getAvatarType() const {
				return type;
			}

			void mousePressEvent(QMouseEvent* event);

		private:
			ByteArray data;
			String type;
			QLabel* label;
	};
}
