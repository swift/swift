/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtTextEdit.h>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>

#include <QApplication>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>
#include <QTextDocument>
#include <QMimeData>

#include <Swiften/Base/Log.h>

#include <SwifTools/SpellChecker.h>
#include <SwifTools/SpellCheckerFactory.h>

#include <Swift/QtUI/QtSpellCheckerWindow.h>
#include <Swift/QtUI/QtSwiftUtil.h>
#include <Swift/QtUI/QtUISettingConstants.h>
#include <Swift/QtUI/QtUtilities.h>

namespace Swift {

QtTextEdit::QtTextEdit(SettingsProvider* settings, QWidget* parent) : QTextEdit(parent), checker_(nullptr), highlighter_(nullptr) {
    connect(this, SIGNAL(textChanged()), this, SLOT(handleTextChanged()));
    settings_ = settings;
#ifdef HAVE_SPELLCHECKER
    setUpSpellChecker();
#endif
    handleTextChanged();
    QTextOption textOption = document()->defaultTextOption();
    textOption.setWrapMode(QTextOption::WordWrap);
    document()->setDefaultTextOption(textOption);
}

QtTextEdit::~QtTextEdit() {
    delete checker_;
}

void QtTextEdit::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    Qt::KeyboardModifiers modifiers = event->modifiers();
    if ((key == Qt::Key_Enter || key == Qt::Key_Return)
        && (modifiers == Qt::NoModifier || modifiers == Qt::KeypadModifier)) {
        emit returnPressed();
    }
    else if (((key == Qt::Key_PageUp || key == Qt::Key_PageDown) && modifiers == Qt::ShiftModifier)
               || (key == Qt::Key_C && modifiers == Qt::ControlModifier && textCursor().selectedText().isEmpty())
               || (key == Qt::Key_W && modifiers == Qt::ControlModifier)
               || (key == Qt::Key_PageUp && modifiers == Qt::ControlModifier)
               || (key == Qt::Key_PageDown && modifiers == Qt::ControlModifier)
               || (key == Qt::Key_Tab && modifiers == Qt::ControlModifier)
               || (key == Qt::Key_A && modifiers == Qt::AltModifier)
               || (key == Qt::Key_Tab)
#ifdef SWIFTEN_PLATFORM_MACOSX
               || (key == Qt::Key_Minus && (modifiers & Qt::ControlModifier))
               || (key == Qt::Key_Equal && (modifiers & Qt::ControlModifier))
#endif
               || (event->matches(QKeySequence::ZoomIn))
               || (event->matches(QKeySequence::ZoomOut))
    ) {
        emit unhandledKeyPressEvent(event);
    }
    else if ((key == Qt::Key_Up)
               || (key == Qt::Key_Down)) {
        emit unhandledKeyPressEvent(event);
        QTextEdit::keyPressEvent(event);
    }
    else if ((key == Qt::Key_K && modifiers == QtUtilities::ctrlHardwareKeyModifier)) {
        QTextCursor cursor = textCursor();
        cursor.setPosition(toPlainText().size(), QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    else {
        QTextEdit::keyPressEvent(event);
    }
}

void QtTextEdit::setEmphasiseFocus(bool emphasise) {
    emphasiseFocus_ = emphasise;
    updateStyleSheet();
}

void QtTextEdit::setCorrectionHighlight(bool correctionHighlight) {
    correctionHighlight_ = correctionHighlight;
    updateStyleSheet();
}

void QtTextEdit::updateStyleSheet() {
    QString newStyleSheet;

    if (correctionHighlight_) {
        newStyleSheet += "background: rgb(255, 255, 153); color: black;";
    }

    if (emphasiseFocus_) {
        if (hasFocus()) {
            newStyleSheet += "border: 2px solid palette(highlight);";
        }
    }

    setStyleSheet(newStyleSheet);
    handleTextChanged();
}

void QtTextEdit::focusInEvent(QFocusEvent* event) {
    receivedFocus();
    QTextEdit::focusInEvent(event);
    updateStyleSheet();
}

void QtTextEdit::focusOutEvent(QFocusEvent* event) {
    lostFocus();
    QTextEdit::focusOutEvent(event);
    updateStyleSheet();
}

void QtTextEdit::handleTextChanged() {
    QSize previous(maximumSize());
    setMaximumSize(QSize(maximumWidth(), sizeHint().height()));
    if (previous != maximumSize()) {
        updateGeometry();
    }
}

void QtTextEdit::replaceMisspelledWord(const QString& word, size_t cursorPosition) {
    QTextCursor cursor = textCursor();
    auto wordPosition = getWordFromCursor(cursorPosition);
    if (wordPosition) {
        cursor.setPosition(boost::get<0>(*wordPosition), QTextCursor::MoveAnchor);
        cursor.setPosition(boost::get<1>(*wordPosition), QTextCursor::KeepAnchor);
        QTextCharFormat normalFormat;
        cursor.insertText(word, normalFormat);
    }
}

boost::optional<PositionPair> QtTextEdit::getWordFromCursor(size_t cursorPosition) {
    PositionPairList misspelledPositions = highlighter_->getMisspelledPositions();
    for (auto& misspelledPosition : misspelledPositions) {
        if (cursorPosition >= boost::get<0>(misspelledPosition) && cursorPosition <= boost::get<1>(misspelledPosition)) {
            return misspelledPosition;
        }
    }
    return boost::optional<PositionPair>(boost::make_tuple(-1,-1));
}

QSize QtTextEdit::sizeHint() const {
    QSize hint = document()->size().toSize();
    QMargins margins = contentsMargins();
    return hint + QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
}

void QtTextEdit::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = createStandardContextMenu();
    QTextCursor cursor = cursorForPosition(event->pos());
#ifdef HAVE_SPELLCHECKER
    QAction* insertPoint = menu->actions().first();
    QAction* settingsAction = new QAction(tr("Spell Checker Options"), menu);
    menu->insertAction(insertPoint, settingsAction);
    menu->insertAction(insertPoint, menu->addSeparator());
    addSuggestions(menu, event);
    QAction* result = menu->exec(event->globalPos());
    if (result == settingsAction) {
        spellCheckerSettingsWindow();
    }
    for (auto& replaceWordAction : replaceWordActions_) {
        if (replaceWordAction == result) {
            replaceMisspelledWord(replaceWordAction->text(), cursor.position());
        }
    }
#else
    menu->exec(event->globalPos());
#endif
    delete menu;
}

void QtTextEdit::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasUrls()) {
        itemDropped(event);
    }
    else {
        QTextEdit::dropEvent(event);
    }
}

void QtTextEdit::addSuggestions(QMenu* menu, QContextMenuEvent* event)
{
    replaceWordActions_.clear();
    if (checker_ && highlighter_) {
        QAction* insertPoint = menu->actions().first();
        QTextCursor cursor = cursorForPosition(event->pos());
        auto wordPosition = getWordFromCursor(cursor.position());
        if (!wordPosition) {
            // The click was executed outside a spellable word so no
            // suggestions are necessary
            return;
        }
        cursor.setPosition(boost::get<0>(*wordPosition), QTextCursor::MoveAnchor);
        cursor.setPosition(boost::get<1>(*wordPosition), QTextCursor::KeepAnchor);
        std::vector<std::string> wordList;
        checker_->getSuggestions(Q2PSTRING(cursor.selectedText()), wordList);
        if (wordList.size() == 0) {
            QAction* noSuggestions = new QAction(tr("No Suggestions"), menu);
            noSuggestions->setDisabled(true);
            menu->insertAction(insertPoint, noSuggestions);
        }
        else {
            for (auto& word : wordList) {
                QAction* wordAction = new QAction(word.c_str(), menu);
                menu->insertAction(insertPoint, wordAction);
                replaceWordActions_.push_back(wordAction);
            }
        }
        menu->insertAction(insertPoint, menu->addSeparator());
    }
}


#ifdef HAVE_SPELLCHECKER
void QtTextEdit::setUpSpellChecker() {
    delete highlighter_;
    highlighter_ = nullptr;
    delete checker_;
    checker_ = nullptr;
    if (settings_->getSetting(QtUISettingConstants::SPELL_CHECKER)) {
        checker_ = SpellCheckerFactory().createSpellChecker();
        if (checker_) {
            if (!checker_->isAutomaticallyDetectingLanguage()) {
                checker_->setActiveLanguage(settings_->getSetting(QtUISettingConstants::SPELL_CHECKER_LANGUAGE));
            }
            highlighter_ = new QtSpellCheckHighlighter(document(), checker_);
        }
        else {
            // Spellchecking is not working, as we did not get a valid checker from the factory. Disable spellchecking.
            SWIFT_LOG(warning) << "Spellchecking is currently misconfigured in Swift (e.g. missing dictionary or broken dictionary file). Disable spellchecking." << std::endl;
            settings_->storeSetting(QtUISettingConstants::SPELL_CHECKER, false);
        }

    }
}
#endif

void QtTextEdit::spellCheckerSettingsWindow() {
    if (!spellCheckerWindow_) {
        spellCheckerWindow_ = new QtSpellCheckerWindow(settings_);
        settings_->onSettingChanged.connect(boost::bind(&QtTextEdit::handleSettingChanged, this, _1));
        spellCheckerWindow_->show();
    }
    else {
        spellCheckerWindow_->show();
        spellCheckerWindow_->raise();
        spellCheckerWindow_->activateWindow();
    }
    if (checker_) {
        spellCheckerWindow_->setAutomaticallyIdentifiesLanguage(checker_->isAutomaticallyDetectingLanguage());
        if (!checker_->isAutomaticallyDetectingLanguage()) {
            spellCheckerWindow_->setSupportedLanguages(checker_->supportedLanguages());
            spellCheckerWindow_->setActiveLanguage(checker_->activeLanguage());
        }
    }
}

void QtTextEdit::handleSettingChanged(const std::string& settings) {
    if (settings == QtUISettingConstants::SPELL_CHECKER.getKey() ||
        settings == QtUISettingConstants::SPELL_CHECKER_LANGUAGE.getKey()) {
#ifdef HAVE_SPELLCHECKER
        setUpSpellChecker();
        if (highlighter_) {
            highlighter_->rehighlight();
        }
#endif
    }
}

}
