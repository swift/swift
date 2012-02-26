/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QComboBox>
#include <QMenu>
#include <QStandardItem>
#include <QStandardItemModel>

namespace Swift {

class QtTagComboBox : public QComboBox {
	Q_OBJECT
	Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

	public:
		explicit QtTagComboBox(QWidget* parent = 0);
		~QtTagComboBox();

		bool isEditable() const;
		void setEditable(const bool);

		void addTag(const QString& id, const QString& label);
		void setTag(const QString& id, bool value);
		bool isTagSet(const QString& id) const;

		virtual void showPopup();
		virtual void hidePopup();

		virtual bool event(QEvent* event);

	private:
		void updateDisplayItem();

	private:
		bool editable;
		QStandardItemModel* displayModel;
		QStandardItem* displayItem;
		QMenu* editMenu;
};

}
