/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include "QtFormResultItemModel.h"

#include <boost/algorithm/string/join.hpp>

#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

QtFormResultItemModel::QtFormResultItemModel(QObject *parent) : QAbstractTableModel(parent) {

}

QtFormResultItemModel::QtFormResultItemModel(QObject* parent, Form::ref formResult) : QAbstractTableModel(parent), formResult_(formResult) {

}

void QtFormResultItemModel::setForm(Form::ref formResult) {
	emit layoutAboutToBeChanged();
	formResult_ = formResult;
	emit layoutChanged();
}

const Form::ref QtFormResultItemModel::getForm() const {
	return formResult_;
}

QVariant QtFormResultItemModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const {
	if (!formResult_) return QVariant();
	if (role != Qt::DisplayRole) return QVariant();
	if (static_cast<size_t>(section) >= formResult_->getReportedFields().size()) return QVariant();
	return QVariant(QString::fromStdString(formResult_->getReportedFields().at(section)->getLabel()));
}

int QtFormResultItemModel::rowCount(const QModelIndex &/*parent*/) const {
	if (!formResult_) return 0;
	return formResult_->getItems().size();
}

int QtFormResultItemModel::columnCount(const QModelIndex &/*parent*/) const {
	if (!formResult_) return 0;
	return formResult_->getReportedFields().size();
}

QVariant QtFormResultItemModel::data(const QModelIndex &index, int role) const {
	if (!formResult_) return QVariant();
	if (role != Qt::DisplayRole || !index.isValid()) {
		return QVariant();
	}

	if (static_cast<size_t>(index.row()) >= formResult_->getItems().size()) return QVariant();
	if (static_cast<size_t>(index.column()) >= formResult_->getReportedFields().size()) return QVariant();

	Form::FormItem item = formResult_->getItems().at(index.row());

	return QVariant(P2QSTRING(getFieldValue(item, index.column())));
}

const std::string QtFormResultItemModel::getFieldValue(const Form::FormItem& item, const int column) const {
	// determine field name
	std::string name = formResult_->getReportedFields().at(column)->getName();

	foreach(FormField::ref field, item) {
		if (field->getName() == name) {
			std::string delimiter = "";
			if (boost::dynamic_pointer_cast<TextMultiFormField>(field)) {
				delimiter = "\n";
			} else if (boost::dynamic_pointer_cast<JIDMultiFormField>(field)) {
				delimiter = ", ";
			} else if (boost::dynamic_pointer_cast<ListMultiFormField>(field)) {
				delimiter = ", ";
			}

			return boost::algorithm::join(field->getRawValues(), delimiter);
		}
	}

	return "";
}

}
