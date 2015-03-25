/*
 * Copyright (c) 2015 Daniel Baczynski
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <map>
#include <string>

#include <QLabel>
#include <QPixmap>
#include <QString>

class QWidget;
class QMouseEvent;

namespace Swift {

	class QtEmoticonCell : public QLabel {
		Q_OBJECT
		public:
			QtEmoticonCell(const QString emoticonAsText, QString filePath, QWidget* parent = 0);
			~QtEmoticonCell();
			virtual void mousePressEvent(QMouseEvent* event);

		signals:
			void emoticonClicked(QString emoticonAsText);

		private:
			QString emoticonAsText_;
	};
}
