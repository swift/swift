/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#pragma once

#include <QWebView>
#include <vector>

namespace Swift {
	class QtWebView : public QWebView {
		Q_OBJECT
		public:
			QtWebView(QWidget* parent);
			void keyPressEvent(QKeyEvent* event);
			void dragEnterEvent(QDragEnterEvent *event);
			void contextMenuEvent(QContextMenuEvent* ev);
			void setFontSizeIsMinimal(bool minimum);

		signals:
			void gotFocus();
			void clearRequested();
			void fontGrowRequested();
			void fontShrinkRequested();

		protected:
			void focusInEvent(QFocusEvent* event);

		private:
			std::vector<QWebPage::WebAction> filteredActions;
			bool fontSizeIsMinimal;
	};
}
