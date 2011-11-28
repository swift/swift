/*
 * Copyright (c) 2011 Vlad Voicu
 * Licensed under the Simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include "ui_QtSpellCheckerWindow.h"

#include <QDialog>

namespace Swift {
	class SettingsProvider;
	class QtSpellCheckerWindow : public QDialog, protected Ui::QtSpellCheckerWindow {
		Q_OBJECT
		public:
			QtSpellCheckerWindow(SettingsProvider* settings, QWidget* parent = NULL);
		public slots:
			void handleChecker(bool state);
			void handleCancel();
			void handlePathButton();
			void handlePersonalPathButton();
			void handleApply();

		private:
			void setEnabled(bool state);
			void setFromSettings();
			void showFiles(const QStringList& files);
			SettingsProvider* settings_;
			Ui::QtSpellCheckerWindow ui_;
	};
}
