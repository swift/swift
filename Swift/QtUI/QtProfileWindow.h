/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/JID/JID.h>

#include <Swift/Controllers/UIInterfaces/ProfileWindow.h>

#include <QWidget>

namespace Ui {
	class QtProfileWindow;
}

namespace Swift {

class QtProfileWindow : public QWidget, public ProfileWindow {
	Q_OBJECT

	public:
		QtProfileWindow();
		virtual ~QtProfileWindow();

		virtual void setJID(const JID& jid);
		virtual void setVCard(VCard::ref vcard);

		virtual void setEnabled(bool b);
		virtual void setProcessing(bool processing);
		virtual void setError(const std::string& error);
		virtual void setEditable(bool b);

		virtual void show();
		virtual void hide();

	private:
		void updateTitle();
		void updateWindowSize();
		virtual void closeEvent(QCloseEvent* event);

	private slots:
		void handleSave();

	private:
		Ui::QtProfileWindow* ui;
		JID jid;
};

}
