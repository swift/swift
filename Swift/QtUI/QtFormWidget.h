/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>

#include <map>
#include <Swiften/Elements/Form.h>

class QListWidget;

namespace Swift {

class QtFormWidget : public QWidget {
	Q_OBJECT
	public:
		QtFormWidget(Form::ref form, QWidget* parent = NULL);
		virtual ~QtFormWidget();
		Form::ref getCompletedForm();
	private:
		QWidget* createWidget(FormField::ref field);
		QListWidget* createList(FormField::ref field);
		std::map<std::string, QWidget*> fields_;
		Form::ref form_;
};

}
