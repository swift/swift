/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QToolButton>

namespace Swift {

	class QtColorToolButton : public QToolButton {
		Q_OBJECT
		Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
		public:
			explicit QtColorToolButton(QWidget* parent = NULL);
			void setColor(const QColor& color);
			const QColor& getColor() const { return color_; }

		signals:
			void colorChanged(const QColor&);

		private slots:
			void onClicked();

		private:
			void setColorIcon(const QColor& color);
			QColor color_;
	};

}
