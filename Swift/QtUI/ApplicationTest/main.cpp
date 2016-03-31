/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <string>

#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <SwifTools/Application/Platform/PlatformApplication.h>

#include <Swift/QtUI/QtSwiftUtil.h>

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
