/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <boost/algorithm/string.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <QStringList>
#include <QColor>

#include <Swift/Controllers/HighlightManager.h>
#include <Swift/QtUI/QtHighlightRulesItemModel.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtHighlightRulesItemModel::QtHighlightRulesItemModel(QObject* parent) : QAbstractItemModel(parent), highlightManager_(NULL)
{
}

void QtHighlightRulesItemModel::setHighlightManager(HighlightManager* highlightManager)
{
	emit layoutAboutToBeChanged();
	highlightManager_ = highlightManager;
	emit layoutChanged();
}

QVariant QtHighlightRulesItemModel::headerData(int section, Qt::Orientation /* orientation */, int role) const
{
	if (role == Qt::DisplayRole) {
		switch (section) {
			case ApplyTo: return QVariant(tr("Apply to"));
			case Sender: return QVariant(tr("Sender"));
			case Keyword: return QVariant(tr("Keyword"));
			case Action: return QVariant(tr("Action"));
			case NickIsKeyword: return QVariant(tr("Nick Is Keyword"));
			case MatchCase: return QVariant(tr("Match Case"));
			case MatchWholeWords: return QVariant(tr("Match Whole Words"));
			case HighlightText: return QVariant(tr("Highlight Text"));
			case TextColor: return QVariant(tr("Text Color"));
			case TextBackground: return QVariant(tr("Text Background"));
			case PlaySound: return QVariant(tr("Play Sounds"));
			case SoundFile: return QVariant(tr("Sound File"));
		}
	}

	return QVariant();
}

int QtHighlightRulesItemModel::columnCount(const QModelIndex& /* parent */) const
{
	return NumberOfColumns;
}

QVariant QtHighlightRulesItemModel::data(const QModelIndex &index, int role) const
{
	if (index.isValid() && highlightManager_ && (role == Qt::DisplayRole || role == Qt::EditRole)) {

		const char* separator = (role == Qt::DisplayRole) ? " ; " : "\n";

		if (boost::numeric_cast<std::vector<std::string>::size_type>(index.row()) < highlightManager_->getRules().size()) {
			const HighlightRule& r = highlightManager_->getRules()[index.row()];
			switch (index.column()) {
				case ApplyTo: {
					int applyTo = 0;
					if (r.getMatchChat() && r.getMatchMUC()) {
						applyTo = 1;
					} else if (r.getMatchChat()) {
						applyTo = 2;
					} else if (r.getMatchMUC()) {
						applyTo = 3;
					}

					if (role == Qt::DisplayRole) {
						return QVariant(getApplyToString(applyTo));
					} else {
						return QVariant(applyTo);
					}
				}
				case Sender: {
					std::string s = boost::join(r.getSenders(), separator);
					return QVariant(P2QSTRING(s));
				}
				case Keyword: {
					std::string s = boost::join(r.getKeywords(), separator);
					QString qs(P2QSTRING(s));
					if (role == Qt::DisplayRole && r.getNickIsKeyword()) {
						qs = tr("<nick>") + (qs.isEmpty() ? "" : separator + qs);
					}
					return QVariant(qs);
				}
				case Action: {
					std::vector<std::string> v;
					const HighlightAction & action = r.getAction();
					if (action.highlightText()) {
						v.push_back(Q2PSTRING(tr("Highlight text")));
					}
					if (action.playSound()) {
						v.push_back(Q2PSTRING(tr("Play sound")));
					}
					std::string s = boost::join(v, separator);
					return QVariant(P2QSTRING(s));
				}
				case NickIsKeyword: {
					return QVariant(r.getNickIsKeyword());
				}
				case MatchCase: {
					return QVariant(r.getMatchCase());
				}
				case MatchWholeWords: {
					return QVariant(r.getMatchWholeWords());
				}
				case HighlightText: {
					return QVariant(r.getAction().highlightText());
				}
				case TextColor: {
					return QVariant(QColor(P2QSTRING(r.getAction().getTextColor())));
				}
				case TextBackground: {
					return QVariant(QColor(P2QSTRING(r.getAction().getTextBackground())));
				}
				case PlaySound: {
					return QVariant(r.getAction().playSound());
				}
				case SoundFile: {
					return QVariant(P2QSTRING(r.getAction().getSoundFile()));
				}
			}
		}
	}
	return QVariant();
}

bool QtHighlightRulesItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && highlightManager_ && role == Qt::EditRole) {
		if (boost::numeric_cast<std::vector<std::string>::size_type>(index.row()) < highlightManager_->getRules().size()) {
			HighlightRule r = highlightManager_->getRule(index.row());
			std::vector<int> changedColumns;
			switch (index.column()) {
				case ApplyTo: {
					bool ok = false;
					int applyTo = value.toInt(&ok);
					if (!ok) {
						return false;
					}
					r.setMatchChat(applyTo == ApplyToAll || applyTo == ApplyToChat);
					r.setMatchMUC(applyTo == ApplyToAll || applyTo == ApplyToMUC);
					break;
				}
				case Sender:
				case Keyword: {
					std::string s = Q2PSTRING(value.toString());
					std::vector<std::string> v;
					boost::split(v, s, boost::is_any_of("\n"));
					v.erase(std::remove_if(v.begin(), v.end(), boost::lambda::_1 == ""), v.end());
					if (index.column() == Sender) {
						r.setSenders(v);
					} else {
						r.setKeywords(v);
					}
					break;
				}
				case NickIsKeyword: {
					r.setNickIsKeyword(value.toBool());
					changedColumns.push_back(Keyword);	// "<nick>"
					break;
				}
				case MatchCase: {
					r.setMatchCase(value.toBool());
					break;
				}
				case MatchWholeWords: {
					r.setMatchWholeWords(value.toBool());
					break;
				}
				case HighlightText: {
					r.getAction().setHighlightText(value.toBool());
					changedColumns.push_back(Action);
					break;
				}
				case TextColor: {
					QColor c = value.value<QColor>();
					r.getAction().setTextColor(c.isValid() ? Q2PSTRING(c.name()) : "");
					break;
				}
				case TextBackground: {
					QColor c = value.value<QColor>();
					r.getAction().setTextBackground(c.isValid() ? Q2PSTRING(c.name()) : "");
					break;
				}
				case PlaySound: {
					r.getAction().setPlaySound(value.toBool());
					changedColumns.push_back(Action);
					break;
				}
				case SoundFile: {
					r.getAction().setSoundFile(Q2PSTRING(value.toString()));
					break;
				}
			}

			highlightManager_->setRule(index.row(), r);
			emit dataChanged(index, index);
			foreach (int column, changedColumns) {
				QModelIndex i = createIndex(index.row(), column, static_cast<void*>(0));
				emit dataChanged(i, i);
			}
		}
	}

	return false;
}

QModelIndex QtHighlightRulesItemModel::parent(const QModelIndex& /* child */) const
{
	return QModelIndex();
}

int QtHighlightRulesItemModel::rowCount(const QModelIndex& /* parent */) const
{
	return highlightManager_ ? highlightManager_->getRules().size() : 0;
}

QModelIndex QtHighlightRulesItemModel::index(int row, int column, const QModelIndex& /* parent */) const
{
	return createIndex(row, column, static_cast<void*>(0));
}

bool QtHighlightRulesItemModel::insertRows(int row, int count, const QModelIndex& /* parent */)
{
	if (highlightManager_) {
		beginInsertRows(QModelIndex(), row, row + count);
		while (count--) {
			highlightManager_->insertRule(row, HighlightRule());
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool QtHighlightRulesItemModel::removeRows(int row, int count, const QModelIndex& /* parent */)
{
	if (highlightManager_) {
		beginRemoveRows(QModelIndex(), row, row + count);
		while (count--) {
			highlightManager_->removeRule(row);
		}
		endRemoveRows();
		return true;
	}
	return false;
}

bool QtHighlightRulesItemModel::swapRows(int row1, int row2)
{
	if (highlightManager_) {
		assert(row1 >= 0 && row2 >= 0 && boost::numeric_cast<std::vector<std::string>::size_type>(row1) < highlightManager_->getRules().size() && boost::numeric_cast<std::vector<std::string>::size_type>(row2) < highlightManager_->getRules().size());
		HighlightRule r = highlightManager_->getRule(row1);
		highlightManager_->setRule(row1, highlightManager_->getRule(row2));
		highlightManager_->setRule(row2, r);
		emit dataChanged(index(row1, 0, QModelIndex()), index(row1, 0, QModelIndex()));
		emit dataChanged(index(row2, 0, QModelIndex()), index(row2, 0, QModelIndex()));
		return true;
	}
	return false;
}

QString QtHighlightRulesItemModel::getApplyToString(int applyTo)
{
	switch (applyTo) {
		case ApplyToNone: return tr("None");
		case ApplyToAll: return tr("Chat or MUC");
		case ApplyToChat: return tr("Chat");
		case ApplyToMUC: return tr("MUC");
		default: return "";
	}
}

}
