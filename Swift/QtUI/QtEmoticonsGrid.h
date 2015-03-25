/*
 * Copyright (c) 2015 Daniel Baczynski
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <map>
#include <string>

#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QString>

#include <Swift/QtUI/QtEmoticonCell.h>

class QWidget;

namespace Swift {

	class QtEmoticonsGrid : public QGridLayout {
		Q_OBJECT
		public:
			explicit QtEmoticonsGrid(const std::map<std::string, std::string>& emoticons, QWidget* parent = 0);
			virtual ~QtEmoticonsGrid();

		signals:
			void emoticonClicked(QString emoticonAsText);

		public slots:
			void emoticonClickedSlot(QString emoticonAsText);

		private:
			void makeUniqueEmoticonsMap(const std::map<std::string, std::string>& emoticons);

			std::map<std::string, std::string> uniqueEmoticons_;
	};
}
