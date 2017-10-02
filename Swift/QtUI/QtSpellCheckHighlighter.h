/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QSyntaxHighlighter>

#include <SwifTools/SpellParser.h>

class QString;
class QTextDocument;

namespace Swift {

class SpellChecker;

class QtSpellCheckHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    QtSpellCheckHighlighter(QTextDocument* parent, SpellChecker* spellChecker);
    virtual ~QtSpellCheckHighlighter();

    PositionPairList getMisspelledPositions() const;

protected:
    virtual void highlightBlock(const QString& text);

private:
    SpellChecker* checker_;
    PositionPairList misspelledPositions_;
};


}
