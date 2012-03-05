/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QValidator>
#include <string>
#include "QtSwiftUtil.h"
#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>
#include <Swift/QtUI/ui_QtJoinMUCWindow.h>

namespace Swift {
	class UIEventStream;
	class NickValidator : public QValidator {
		Q_OBJECT
		public:
			NickValidator(QObject* parent) : QValidator(parent) {
			}

			virtual QValidator::State validate(QString& input, int& /*pos*/) const {
				if (input.isEmpty()) {
					return QValidator::Acceptable;
				}
				JID test("alice", "wonderland.lit", Q2PSTRING(input));

				return test.isValid() ? QValidator::Acceptable : QValidator::Invalid;
			}
	};
	class QtJoinMUCWindow : public QWidget, public JoinMUCWindow {
			Q_OBJECT
		public:
			QtJoinMUCWindow(UIEventStream* uiEventStream);

			virtual void setNick(const std::string& nick);
			virtual void setMUC(const std::string& nick);

			virtual void show();

		private slots:
			void handleJoin();
			void handleSearch();

		private:
			Ui::QtJoinMUCWindow ui;
			std::string lastSetNick;
			UIEventStream* uiEventStream;
	};
}
