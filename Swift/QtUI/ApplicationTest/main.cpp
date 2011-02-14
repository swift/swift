/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include "../QtSwiftUtil.h"
#include <string>
#include "SwifTools/Application/Platform/PlatformApplication.h"

using namespace Swift;

class MyWidget : public QWidget {
		Q_OBJECT

	public:
		MyWidget() : application_("MyApplication") {
			QVBoxLayout *layout = new QVBoxLayout(this);
			input_ = new QLineEdit(this);
			layout->addWidget(input_);
			connect(input_, SIGNAL(returnPressed()), SLOT(handleReturnPressed()));
		}

	private slots:
		void handleReturnPressed() {
			application_.getApplicationMessageDisplay()->setMessage(Q2PSTRING(input_->text()));
		}
	
	private:
		PlatformApplication application_;
		QLineEdit* input_;
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	MyWidget widget;
	widget.show();
	return app.exec();
}

#include "main.moc"
