#ifndef SWIFT_QtJoinMUCDialog_H
#define SWIFT_QtJoinMUCDialog_H

#include "ui_QtJoinMUCDialog.h"
#include "Swiften/JID/JID.h"

#include <QDialog>

namespace Swift {

class QtJoinMUCDialog : public QDialog, private Ui::QtJoinMUCDialog {
	Q_OBJECT

	public:
		QtJoinMUCDialog(const QString& muc, const QString& nick, QWidget* parent);
	signals:
		void onJoinCommand(const JID& muc, const QString& nick);
	public slots:
		void accept();
	private:
		void showError(const QString& error);
};

}

#endif
