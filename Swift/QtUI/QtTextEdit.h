/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QPointer>
#include <QTextEdit>

#include <Swift/Controllers/SettingConstants.h>
#include <Swift/Controllers/Settings/SettingsProvider.h>

#include <SwifTools/SpellParser.h>

#include <Swift/QtUI/QtSpellCheckHighlighter.h>

namespace Swift {
	class SpellChecker;
	class QtSpellCheckerWindow;

	class QtTextEdit : public QTextEdit {
		Q_OBJECT
	public:
		QtTextEdit(SettingsProvider* settings, QWidget* parent = 0);
		virtual ~QtTextEdit();
		virtual QSize sizeHint() const;

	signals:
		void wordCorrected(QString& word);
		void returnPressed();
		void unhandledKeyPressEvent(QKeyEvent* event);
		void receivedFocus();
		void lostFocus();

	public slots:
		void handleSettingChanged(const std::string& settings);

	protected:
		virtual void keyPressEvent(QKeyEvent* event);
		virtual void focusInEvent(QFocusEvent* event);
		virtual void focusOutEvent(QFocusEvent* event);
		virtual void contextMenuEvent(QContextMenuEvent* event);

	private slots:
		void handleTextChanged();

	private:
		void addSuggestions(QMenu* menu, QContextMenuEvent* event);
		void replaceMisspelledWord(const QString& word, int cursorPosition);
		void setUpSpellChecker();
		void spellCheckerSettingsWindow();
		PositionPair getWordFromCursor(int cursorPosition);

	private:
		SpellChecker* checker_;
		QtSpellCheckHighlighter* highlighter_;
		std::vector<QAction*> replaceWordActions_;
		SettingsProvider* settings_;
		QPointer<QtSpellCheckerWindow> spellCheckerWindow_;
	};
}
