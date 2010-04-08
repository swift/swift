/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFT_QtAddContactDialog_H
#define SWIFT_QtAddContactDialog_H

#include "ui_QtAddContactDialog.h"
#include "Swiften/JID/JID.h"

#include <QDialog>

namespace Swift {

class QtAddContactDialog : public QDialog, private Ui::QtAddContactDialog {
	Q_OBJECT

	public:
		QtAddContactDialog(QWidget* parent);
	signals:
		void onAddCommand(const JID& contact, const QString& name);
	public slots:
		void accept();
	private:
		void showError(const QString& error);
};

}

#endif
