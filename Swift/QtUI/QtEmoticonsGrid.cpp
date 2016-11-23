/*
 * Copyright (c) 2015 Daniel Baczynski
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtEmoticonsGrid.h>

#include <set>

#include <boost/range/adaptor/reversed.hpp>

#include <QPushButton>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtEmoticonsGrid::QtEmoticonsGrid(const std::map<std::string, std::string>& emoticons, QWidget* parent) : QGridLayout(parent) {
    makeUniqueEmoticonsMap(emoticons);

    // Create grid: 3 columns, [uniqueEmoticons_.size()/3] rows
    int row = 0;
    int column = 0;

    for (auto&& emoticon : uniqueEmoticons_) {
        QtEmoticonCell* newCell = new QtEmoticonCell(P2QSTRING(emoticon.first), P2QSTRING(emoticon.second));
        addWidget(newCell, row, column);
        connect(newCell, SIGNAL(emoticonClicked(QString)), this, SLOT(emoticonClickedSlot(QString)));

        column++;
        if (column >= 3) {
            column = 0;
            row++;
        }
    }
}

QtEmoticonsGrid::~QtEmoticonsGrid() {

}

void QtEmoticonsGrid::makeUniqueEmoticonsMap(const std::map<std::string, std::string>& emoticons) {
    std::set<std::string> paths;
    for (auto&& emoticon : boost::adaptors::reverse(emoticons)) {
        if (paths.find(emoticon.second) == paths.end()) {
            uniqueEmoticons_.insert(emoticon);
            paths.insert(emoticon.second);
        }
    }
}

void QtEmoticonsGrid::emoticonClickedSlot(QString emoticonAsText) {
    emit emoticonClicked(emoticonAsText);
}

}
