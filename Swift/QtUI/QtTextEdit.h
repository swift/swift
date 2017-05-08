/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QPointer>
#include <QTextEdit>

#include <Swift/Controllers/Settings/SettingsProvider.h>

#include <SwifTools/SpellParser.h>

#include <Swift/QtUI/QtSpellCheckHighlighter.h>

namespace Swift {
    class SpellChecker;
    class QtSpellCheckerWindow;

    class QtTextEdit : public QTextEdit {
        Q_OBJECT
    public:
        QtTextEdit(SettingsProvider* settings, QWidget* parent = nullptr);
        virtual ~QtTextEdit();
        virtual QSize sizeHint() const;

        void setEmphasiseFocus(bool emphasise);
        void setCorrectionHighlight(bool coorectionHighlight);

    signals:
        void wordCorrected(QString& word);
        void returnPressed();
        void unhandledKeyPressEvent(QKeyEvent* event);
        void receivedFocus();
        void lostFocus();
        void itemDropped(QDropEvent* event);

    public slots:
        void handleSettingChanged(const std::string& settings);

    protected:
        virtual void keyPressEvent(QKeyEvent* event);
        virtual void focusInEvent(QFocusEvent* event);
        virtual void focusOutEvent(QFocusEvent* event);
        virtual void contextMenuEvent(QContextMenuEvent* event);
        virtual void dropEvent(QDropEvent* event);

    private slots:
        void handleTextChanged();

    private:
        void addSuggestions(QMenu* menu, QContextMenuEvent* event);
        void replaceMisspelledWord(const QString& word, int cursorPosition);
        void setUpSpellChecker();
        void spellCheckerSettingsWindow();
        PositionPair getWordFromCursor(int cursorPosition);
        void updateStyleSheet();

    private:
        SpellChecker* checker_;
        QtSpellCheckHighlighter* highlighter_;
        std::vector<QAction*> replaceWordActions_;
        SettingsProvider* settings_;
        QPointer<QtSpellCheckerWindow> spellCheckerWindow_;
        bool emphasiseFocus_ = false;
        bool correctionHighlight_ = false;
    };
}
