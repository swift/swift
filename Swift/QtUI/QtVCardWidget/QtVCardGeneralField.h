/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include <Swift/QtUI/QtVCardWidget/QtCloseButton.h>
#include <Swift/QtUI/QtVCardWidget/QtTagComboBox.h>

namespace Swift {

class QtElidingLabel;

/*
 *	covers features like:
 *		- preffered (star ceckbox)
 *		- combo check box
 *		- label
 *		- remove button
 */
class QtVCardGeneralField : public QWidget {
		Q_OBJECT
		Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)
		Q_PROPERTY(bool empty READ isEmpty)

	public:
		explicit QtVCardGeneralField(QWidget* parent = 0, QGridLayout* layout = 0, bool editable = false, int row = 0, QString label = QString(),
										bool preferrable = true, bool taggable = true);
		virtual ~QtVCardGeneralField();

		void initialize();

		virtual bool isEditable() const;
		virtual void setEditable(bool);

		virtual bool isEmpty() const = 0;

		void setStarVisible(const bool isVisible);
		bool getStarVisible() const;

		void setPreferred(const bool preferred);
		bool getPreferred() const;

	protected:
		virtual void setupContentWidgets() = 0;
		virtual void customCleanup();

		QtTagComboBox* getTagComboBox() const;
		QGridLayout* getGridLayout() const;

	signals:
		void editableChanged(bool);
		void deleteField(QtVCardGeneralField*);
		
	public slots:
		void handleCloseButtonClicked();
		void handlePreferredStarStateChanged(int statte);

	protected:
		QList<QWidget*> childWidgets;

	private:
		void updatePreferredStarVisibility();

	private:
		bool editable;
		bool preferrable;
		bool starVisible;
		bool taggable;
		QGridLayout* layout;
		int row;
		QCheckBox* preferredCheckBox;
		QLabel* label;
		QString labelText;
		QtTagComboBox* tagComboBox;
		QtElidingLabel* tagLabel;
		QtCloseButton* closeButton;
};

}
