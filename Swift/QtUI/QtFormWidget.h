/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>

#include <QWidget>

#include <Swiften/Elements/Form.h>

class QListWidget;

namespace Swift {

class QtFormWidget : public QWidget {
    Q_OBJECT
    public:
        QtFormWidget(Form::ref form, QWidget* parent = nullptr);
        virtual ~QtFormWidget();
        Form::ref getCompletedForm();
        void setEditable(bool editable);
    private:
        QWidget* createWidget(FormField::ref field, const FormField::Type type, const size_t index);
        QListWidget* createList(FormField::ref field);
        template<class T> void setEnabled(QWidget* rawWidget, bool editable);
        template<class T> void setEditable(QWidget* rawWidget, bool editable);
        std::map<std::string, QWidget*> fields_;
        Form::ref form_;
};

}
