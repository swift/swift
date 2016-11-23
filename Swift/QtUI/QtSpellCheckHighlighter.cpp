/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtSpellCheckHighlighter.h>

#include <SwifTools/SpellChecker.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtSpellCheckHighlighter::QtSpellCheckHighlighter(QTextDocument* parent, SpellChecker* spellChecker) : QSyntaxHighlighter(parent), checker_(spellChecker) {

}

QtSpellCheckHighlighter::~QtSpellCheckHighlighter() {

}

void QtSpellCheckHighlighter::highlightBlock(const QString& text) {
    misspelledPositions_.clear();
    std::string fragment = Q2PSTRING(text);
    checker_->checkFragment(fragment, misspelledPositions_);

    QTextCharFormat spellingErrorFormat;
    spellingErrorFormat.setUnderlineColor(QColor(Qt::red));
    spellingErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);

    for (auto&& position : misspelledPositions_) {
        setFormat(boost::get<0>(position), boost::get<1>(position) - boost::get<0>(position), spellingErrorFormat);
    };
}

PositionPairList QtSpellCheckHighlighter::getMisspelledPositions() const {
    return misspelledPositions_;
}

}
