/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtFormWidget.h>

#include <memory>

#include <boost/algorithm/string/join.hpp>

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QScrollArea>
#include <QTextEdit>

#include <qdebug.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtFormWidget::QtFormWidget(Form::ref form, QWidget* parent) : QWidget(parent), form_(form) {
    QGridLayout* thisLayout = new QGridLayout(this);
    int row = 0;
    if (!form->getTitle().empty()) {
        QLabel* instructions = new QLabel(("<b>" + form->getTitle() + "</b>").c_str(), this);
        thisLayout->addWidget(instructions, row++, 0, 1, 2);
    }
    if (!form->getInstructions().empty()) {
        QLabel* instructions = new QLabel(form->getInstructions().c_str(), this);
        thisLayout->addWidget(instructions, row++, 0, 1, 2);
    }
    QScrollArea* scrollArea = new QScrollArea(this);
    thisLayout->addWidget(scrollArea);
    QWidget* scroll = new QWidget(this);
    QGridLayout* layout = new QGridLayout(scroll);
    const std::vector<Form::FormItem> items = form->getItems();
    if (items.empty()) { /* single item forms */
        for (auto&& field : form->getFields()) {
            QWidget* widget = createWidget(field, field->getType(), 0);
            if (widget) {
                layout->addWidget(new QLabel(field->getLabel().c_str(), this), row, 0);
                layout->addWidget(widget, row++, 1);
            }
        }
    } else { /* multi-item forms */
        const Form::FormItem& headers = form->getFields();
        for (size_t i = 0; i < items.size(); ++i) {
            const Form::FormItem& item = items[i];
            assert(item.size() == headers.size());
            for (size_t j = 0; j < item.size(); ++j) {
                QWidget* widget = createWidget(item[j], headers[j]->getType(), i);
                if (widget) {
                    layout->addWidget(new QLabel(item[j]->getLabel().c_str(), this), row, 0);
                    layout->addWidget(widget, row++, 1);
                }
            }
        }
    }
    scrollArea->setWidget(scroll);
    scrollArea->setWidgetResizable(true);
    setEditable(form->getType() != Form::CancelType && form->getType() != Form::ResultType);
}

QtFormWidget::~QtFormWidget() {

}

QListWidget* QtFormWidget::createList(FormField::ref field) {
    QListWidget* listWidget = new QListWidget(this);
    listWidget->setSortingEnabled(false);
    listWidget->setSelectionMode(field->getType() == FormField::ListMultiType ? QAbstractItemView::MultiSelection : QAbstractItemView::SingleSelection);
    std::vector<bool> selected;
    /* if this is an editable form, use the 'options' list, otherwise use the 'values' list */
    if (form_->getType() != Form::FormType) {
        for (const auto& value : field->getValues()) {
            listWidget->addItem(P2QSTRING(value));
            selected.push_back(false);
        }
    } else {
        for (auto&& option : field->getOptions()) {
            listWidget->addItem(option.label.c_str());
            if (field->getType() == FormField::ListSingleType) {
                selected.push_back(!field->getValues().empty() && option.value == field->getValues()[0]);
            }
            else if (field->getType() == FormField::ListMultiType) {
                std::string text = option.value;
                selected.push_back(std::find(field->getValues().begin(), field->getValues().end(), text) != field->getValues().end());
            }
        }
    }
    for (int i = 0; i < listWidget->count(); i++) {
        QListWidgetItem* item = listWidget->item(i);
        item->setSelected(selected[i]);
    }
    return listWidget;
}

QWidget* QtFormWidget::createWidget(FormField::ref field, const FormField::Type type, const size_t index) {
    QWidget* widget = nullptr;
    if (type == FormField::BooleanType) {
        QCheckBox* checkWidget = new QCheckBox(this);
        checkWidget->setCheckState(field->getBoolValue() ? Qt::Checked : Qt::Unchecked);
        widget = checkWidget;
    }
    if (type == FormField::FixedType) {
        QString value = field->getFixedValue().c_str();
        widget = new QLabel(value, this);
    }
    if (type == FormField::ListSingleType) {
        widget = createList(field);
    }
    if (type == FormField::TextMultiType) {
        QString value = field->getTextMultiValue().c_str();
        QTextEdit* textWidget = new QTextEdit(this);
        textWidget->setPlainText(value);
        widget = textWidget;
    }
    if (type == FormField::TextPrivateType) {
        QString value = field->getTextPrivateValue().c_str();
        QLineEdit* lineWidget = new QLineEdit(value, this);
        lineWidget->setEchoMode(QLineEdit::Password);
        widget = lineWidget;
    }
    if (type == FormField::TextSingleType) {
        QString value = field->getTextSingleValue().c_str();
        widget = new QLineEdit(value, this);
    }
    if (type == FormField::JIDSingleType) {
        QString value = field->getJIDSingleValue().toString().c_str();
        widget = new QLineEdit(value, this);
    }
    if (type == FormField::JIDMultiType) {
        QString text = boost::join(field->getValues(), "\n").c_str();
        QTextEdit* textWidget = new QTextEdit(this);
        textWidget->setPlainText(text);
        widget = textWidget;
    }
    if (type == FormField::ListMultiType) {
        widget = createList(field);
    }
    std::string indexString;
    if (index) {
        /* for multi-item forms we need to distinguish between the different rows */
        indexString = boost::lexical_cast<std::string>(index);
    }
    fields_[field->getName() + indexString] = widget;
    return widget;
}

Form::ref QtFormWidget::getCompletedForm() {
    Form::ref result(new Form(Form::SubmitType));
    for (auto&& field : form_->getFields()) {
        std::shared_ptr<FormField> resultField = std::make_shared<FormField>(field->getType());
        if (field->getType() == FormField::BooleanType) {
            resultField->setBoolValue(qobject_cast<QCheckBox*>(fields_[field->getName()])->checkState() == Qt::Checked);
        }
        if (field->getType() == FormField::FixedType || field->getType() == FormField::HiddenType) {
            resultField->addValue(field->getValues().empty() ? "" : field->getValues()[0]);
        }
        if (field->getType() == FormField::ListSingleType) {
            QListWidget* listWidget = qobject_cast<QListWidget*>(fields_[field->getName()]);
            if (listWidget->selectedItems().size() > 0) {
                int i = listWidget->row(listWidget->selectedItems()[0]);
                resultField->addValue(field->getOptions()[i].value);
            }
        }
        if (field->getType() == FormField::TextMultiType) {
            QTextEdit* widget = qobject_cast<QTextEdit*>(fields_[field->getName()]);
            QString string = widget->toPlainText();
            if (!string.isEmpty()) {
                resultField->setTextMultiValue(Q2PSTRING(string));
            }
        }
        if (field->getType() == FormField::TextPrivateType || field->getType() == FormField::TextSingleType || field->getType() == FormField::JIDSingleType) {
            QLineEdit* widget = qobject_cast<QLineEdit*>(fields_[field->getName()]);
            QString string = widget->text();
            if (!string.isEmpty()) {
                resultField->addValue(Q2PSTRING(string));
            }
        }
        if (field->getType() == FormField::JIDMultiType) {
            QTextEdit* widget = qobject_cast<QTextEdit*>(fields_[field->getName()]);
            QString string = widget->toPlainText();
            if (!string.isEmpty()) {
                QStringList lines = string.split("\n");
                for (auto&& line : lines) {
                    resultField->addValue(Q2PSTRING(line));
                }
            }
        }
        if (field->getType() == FormField::ListMultiType) {
            QListWidget* listWidget = qobject_cast<QListWidget*>(fields_[field->getName()]);
            for (auto item : listWidget->selectedItems()) {
                resultField->addValue(field->getOptions()[listWidget->row(item)].value);
            }
        }
        resultField->setName(field->getName());
        result->addField(resultField);
    }
    return result;
}

template<class T> void QtFormWidget::setEnabled(QWidget* rawWidget, bool editable) {
    T* widget = qobject_cast<T*>(rawWidget);
    if (widget) {
        widget->setEnabled(editable);
    }
}

template<class T> void QtFormWidget::setEditable(QWidget* rawWidget, bool editable) {
    T* widget = qobject_cast<T*>(rawWidget);
    if (widget) {
        widget->setReadOnly(!editable);
    }
}

void QtFormWidget::setEditable(bool editable) {
    if (!form_) {
        return;
    }
    for (auto&& field : form_->getFields()) {
        QWidget* widget = nullptr;
        if (field) {
            widget = fields_[field->getName()];
        }
        setEnabled<QCheckBox>(widget, editable);
        setEnabled<QListWidget>(widget, editable);
        setEditable<QTextEdit>(widget, editable);
        setEditable<QLineEdit>(widget, editable);
    }
}

}
