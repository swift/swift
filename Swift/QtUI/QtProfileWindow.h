/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

#include <Swift/Controllers/UIInterfaces/ProfileWindow.h>

class QLabel;
class QLineEdit;
class QHBoxLayout;
class QPushButton;

namespace Swift {
	class QtAvatarWidget;

	class QtProfileWindow : public QWidget, public ProfileWindow {
			Q_OBJECT
		public:
			QtProfileWindow();

			void setVCard(Swift::VCard::ref);
			void setEnabled(bool);
			void setProcessing(bool);
			virtual void setError(const std::string&);
			void show();
			void hide();

			void hideEvent (QHideEvent* event);

		private slots:
			void handleSave();

		private:
			VCard::ref vcard;
	    QtAvatarWidget* avatar;
	    QLabel* nicknameLabel;
	    QLineEdit* nickname;
	    QLabel* throbberLabel;
	    QLabel* errorLabel;
	    QHBoxLayout* horizontalLayout;
	    QPushButton* saveButton;
	};
}
