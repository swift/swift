/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QAbstractItemModel>

namespace Swift {

	class HighlightManager;

	class QtHighlightRulesItemModel : public QAbstractItemModel {
		Q_OBJECT

		public:
			QtHighlightRulesItemModel(QObject* parent = NULL);

			void setHighlightManager(HighlightManager* highlightManager);

			QVariant headerData(int section, Qt::Orientation orientation, int role) const;
			int columnCount(const QModelIndex& parent) const;
			QVariant data(const QModelIndex& index, int role) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
			QModelIndex parent(const QModelIndex& child) const;
			int rowCount(const QModelIndex& parent) const;
			QModelIndex index(int row, int column, const QModelIndex& parent) const;
			bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
			bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
			bool swapRows(int row1, int row2);

			static QString getApplyToString(int);

			enum Columns {
				ApplyTo = 0,
				Sender,
				Keyword,
				Action,
				NickIsKeyword,
				MatchCase,
				MatchWholeWords,
				HighlightText,
				TextColor,
				TextBackground,
				PlaySound,
				SoundFile,
				NumberOfColumns // end of list marker
			};

			enum ApplyToValues {
				ApplyToNone = 0,
				ApplyToAll,
				ApplyToChat,
				ApplyToMUC,
				ApplyToEOL	// end of list marker
			};

		private:
			HighlightManager* highlightManager_;
	};

}
