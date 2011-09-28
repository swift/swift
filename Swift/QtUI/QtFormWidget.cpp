/*
 * Copyright (c) 2010-2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/QtUI/QtFormWidget.h>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QScrollArea>
#include <qdebug.h>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/Base/foreach.h>

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
	foreach (boost::shared_ptr<FormField> field, form->getFields()) {
		QWidget* widget = createWidget(field);
		if (widget) {
			layout->addWidget(new QLabel(field->getLabel().c_str(), this), row, 0);
			layout->addWidget(widget, row++, 1);
		}
	}
	scrollArea->setWidget(scroll);
	scrollArea->setWidgetResizable(true);
}

QtFormWidget::~QtFormWidget() {

}

QListWidget* QtFormWidget::createList(FormField::ref field) {
	QListWidget* listWidget = new QListWidget(this);
	listWidget->setSortingEnabled(false);
	listWidget->setSelectionMode(boost::dynamic_pointer_cast<ListMultiFormField>(field) ? QAbstractItemView::MultiSelection : QAbstractItemView::SingleSelection);
	boost::shared_ptr<ListMultiFormField> listMultiField = boost::dynamic_pointer_cast<ListMultiFormField>(field);
	boost::shared_ptr<ListSingleFormField> listSingleField = boost::dynamic_pointer_cast<ListSingleFormField>(field);
	std::vector<bool> selected;
	foreach (FormField::Option option, field->getOptions()) {
		listWidget->addItem(option.label.c_str());
		if (listSingleField) {
			selected.push_back(option.value == listSingleField->getValue());
		}
		else if (listMultiField) {
			std::string text = option.value;
			selected.push_back(std::find(listMultiField->getValue().begin(), listMultiField->getValue().end(), text) != listMultiField->getValue().end());
		}

	}
	for (int i = 0; i < listWidget->count(); i++) {
		QListWidgetItem* item = listWidget->item(i);
		item->setSelected(selected[i]);
	}
	return listWidget;
}

QWidget* QtFormWidget::createWidget(FormField::ref field) {
	QWidget* widget = NULL;
	boost::shared_ptr<BooleanFormField> booleanField = boost::dynamic_pointer_cast<BooleanFormField>(field);
	if (booleanField) {
		QCheckBox* checkWidget = new QCheckBox(this);
		checkWidget->setCheckState(booleanField->getValue() ? Qt::Checked : Qt::Unchecked);
		widget = checkWidget;
	}
	boost::shared_ptr<FixedFormField> fixedField = boost::dynamic_pointer_cast<FixedFormField>(field);
	if (fixedField) {
		QString value = fixedField->getValue().c_str();
		widget = new QLabel(value, this);
	}
	boost::shared_ptr<ListSingleFormField> listSingleField = boost::dynamic_pointer_cast<ListSingleFormField>(field);
	if (listSingleField) {
		widget = createList(field);
	}
	boost::shared_ptr<TextMultiFormField> textMultiField = boost::dynamic_pointer_cast<TextMultiFormField>(field);
	if (textMultiField) {
		QString value = textMultiField->getValue().c_str();
		widget = new QTextEdit(value, this);
	}
	boost::shared_ptr<TextPrivateFormField> textPrivateField = boost::dynamic_pointer_cast<TextPrivateFormField>(field);
	if (textPrivateField) {
		QString value = textPrivateField->getValue().c_str();
		QLineEdit* lineWidget = new QLineEdit(value, this);
		lineWidget->setEchoMode(QLineEdit::Password);
		widget = lineWidget;
	}
	boost::shared_ptr<TextSingleFormField> textSingleField = boost::dynamic_pointer_cast<TextSingleFormField>(field);
	if (textSingleField) {
		QString value = textSingleField->getValue().c_str();
		widget = new QLineEdit(value, this);
	}
	boost::shared_ptr<JIDSingleFormField> jidSingleField = boost::dynamic_pointer_cast<JIDSingleFormField>(field);
	if (jidSingleField) {
		QString value = jidSingleField->getValue().toString().c_str();
		widget = new QLineEdit(value, this);
	}
	boost::shared_ptr<JIDMultiFormField> jidMultiField = boost::dynamic_pointer_cast<JIDMultiFormField>(field);
	if (jidMultiField) {
		QString text;
		bool prev = false;
		foreach (JID line, jidMultiField->getValue()) {
			if (prev) {
				text += "\n";
			}
			prev = true;
			text += line.toString().c_str();
		}
		widget = new QTextEdit(text, this);
	}
	boost::shared_ptr<ListMultiFormField> listMultiField = boost::dynamic_pointer_cast<ListMultiFormField>(field);
	if (listMultiField) {
		widget = createList(field);
	}
	boost::shared_ptr<HiddenFormField> hiddenField = boost::dynamic_pointer_cast<HiddenFormField>(field);
	if (hiddenField) {
	}
	fields_[field->getName()] = widget;
	return widget;
}

Form::ref QtFormWidget::getCompletedForm() {
	Form::ref result(new Form(Form::SubmitType));
	foreach (boost::shared_ptr<FormField> field, form_->getFields()) {
		boost::shared_ptr<FormField> resultField;
		boost::shared_ptr<BooleanFormField> booleanField = boost::dynamic_pointer_cast<BooleanFormField>(field);
		if (booleanField) {
			resultField = FormField::ref(BooleanFormField::create(qobject_cast<QCheckBox*>(fields_[field->getName()])->checkState() == Qt::Checked));
		}
		boost::shared_ptr<FixedFormField> fixedField = boost::dynamic_pointer_cast<FixedFormField>(field);
		if (fixedField) {
			resultField = FormField::ref(FixedFormField::create(fixedField->getValue()));
		}
		boost::shared_ptr<ListSingleFormField> listSingleField = boost::dynamic_pointer_cast<ListSingleFormField>(field);
		if (listSingleField) {
			QListWidget* listWidget = qobject_cast<QListWidget*>(fields_[field->getName()]);
			if (listWidget->selectedItems().size() > 0) {
				int i = listWidget->row(listWidget->selectedItems()[0]);
				resultField = FormField::ref(ListSingleFormField::create(field->getOptions()[i].value));
			}
			else {
				resultField = FormField::ref(ListSingleFormField::create());
			}
		}
		boost::shared_ptr<TextMultiFormField> textMultiField = boost::dynamic_pointer_cast<TextMultiFormField>(field);
		if (textMultiField) {
			QTextEdit* widget = qobject_cast<QTextEdit*>(fields_[field->getName()]);
			QString string = widget->toPlainText();
			if (string.isEmpty()) {
				resultField = FormField::ref(TextMultiFormField::create());
			}
			else {
				resultField = FormField::ref(TextMultiFormField::create(Q2PSTRING(string)));
			}
		}
		boost::shared_ptr<TextPrivateFormField> textPrivateField = boost::dynamic_pointer_cast<TextPrivateFormField>(field);
		if (textPrivateField) {
			QLineEdit* widget = qobject_cast<QLineEdit*>(fields_[field->getName()]);
			QString string = widget->text();
			if (string.isEmpty()) {
				resultField = FormField::ref(TextPrivateFormField::create());
			}
			else {
				resultField = FormField::ref(TextPrivateFormField::create(Q2PSTRING(string)));
			}
		}
		boost::shared_ptr<TextSingleFormField> textSingleField = boost::dynamic_pointer_cast<TextSingleFormField>(field);
		if (textSingleField) {
			QLineEdit* widget = qobject_cast<QLineEdit*>(fields_[field->getName()]);
			QString string = widget->text();
			if (string.isEmpty()) {
				resultField = FormField::ref(TextSingleFormField::create());
			}
			else {
				resultField = FormField::ref(TextSingleFormField::create(Q2PSTRING(string)));
			}
		}
		boost::shared_ptr<JIDSingleFormField> jidSingleField = boost::dynamic_pointer_cast<JIDSingleFormField>(field);
		if (jidSingleField) {
			QLineEdit* widget = qobject_cast<QLineEdit*>(fields_[field->getName()]);
			QString string = widget->text();
			JID jid(Q2PSTRING(string));
			if (string.isEmpty()) {
				resultField = FormField::ref(JIDSingleFormField::create());
			}
			else {
				resultField = FormField::ref(JIDSingleFormField::create(jid));
			}
		}
		boost::shared_ptr<JIDMultiFormField> jidMultiField = boost::dynamic_pointer_cast<JIDMultiFormField>(field);
		if (jidMultiField) {
			QTextEdit* widget = qobject_cast<QTextEdit*>(fields_[field->getName()]);
			QString string = widget->toPlainText();
			if (string.isEmpty()) {
				resultField = FormField::ref(JIDMultiFormField::create());
			}
			else {
				QStringList lines = string.split("\n");
				std::vector<JID> value;
				foreach (QString line, lines) {
					value.push_back(JID(Q2PSTRING(line)));
				}
				resultField = FormField::ref(JIDMultiFormField::create(value));
			}
		}
		boost::shared_ptr<ListMultiFormField> listMultiField = boost::dynamic_pointer_cast<ListMultiFormField>(field);
		if (listMultiField) {
			QListWidget* listWidget = qobject_cast<QListWidget*>(fields_[field->getName()]);
			std::vector<std::string> values;
			foreach (QListWidgetItem* item, listWidget->selectedItems()) {
				values.push_back(field->getOptions()[listWidget->row(item)].value);
			}
			resultField = FormField::ref(ListMultiFormField::create(values));
		}
		boost::shared_ptr<HiddenFormField> hiddenField = boost::dynamic_pointer_cast<HiddenFormField>(field);
		if (hiddenField) {
			resultField = FormField::ref(HiddenFormField::create(hiddenField->getValue()));
		}
		resultField->setName(field->getName());
		result->addField(resultField);
	}
	return result;
}

}
