/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QWidget>
#include <Swiften/Elements/VCard.h>
#include <boost/smart_ptr/make_shared.hpp>

#include "QtVCardFieldInfo.h"
#include "QtVCardGeneralField.h"
#include "QtVCardPhotoAndNameFields.h"

namespace Ui {
	class QtVCardWidget;
}

namespace Swift {

	class QtVCardWidget : public QWidget {
		Q_OBJECT
		Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

		public :
			explicit QtVCardWidget(QWidget* parent = 0);
			~QtVCardWidget();

			bool isEditable() const;
			void setEditable(bool);

			void setVCard(VCard::ref vcard);
			VCard::ref getVCard();

		signals:
			void editableChanged(bool editable);

		private slots:
			void addField();
			void removeField(QtVCardGeneralField* field);

		private:
			void addFieldType(QMenu*, boost::shared_ptr<QtVCardFieldInfo>);
			int fieldTypeInstances(boost::shared_ptr<QtVCardFieldInfo>);
			void clearFields();
			void clearEmptyFields();
			void appendField(QtVCardGeneralField* field);

		private:
			VCard::ref vcard;
			Ui::QtVCardWidget* ui;
			bool editable;
			QMenu* menu;
			std::list<QtVCardGeneralField*> fields;
			std::map<QAction*, boost::shared_ptr<QtVCardFieldInfo> > actionFieldInfo;
	};

}
