/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QAbstractTableModel>

#include <Swiften/Elements/Form.h>

namespace Swift {

class QtFormResultItemModel : public QAbstractTableModel {
	Q_OBJECT
	public:
		QtFormResultItemModel(QObject* parent);
		QtFormResultItemModel(QObject* parent, Form::ref formResult);

		void setForm(Form::ref formResult);
		const Form::ref getForm() const;

		virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
		virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	private:
		const std::string getFieldValue(const Form::FormItem& item, const int column) const;

	private:
		Form::ref formResult_;
};

}
