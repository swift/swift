/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/QtUI/ui_QtJoinMUCWindow.h>

#include <string>

#include <QValidator>

#include <Swift/Controllers/UIInterfaces/JoinMUCWindow.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
	class UIEventStream;
	class NickValidator : public QValidator {
		Q_OBJECT
		public:
			NickValidator(QObject* parent) : QValidator(parent) {
			}

			virtual QValidator::State validate(QString& input, int& /*pos*/) const {
				if (input.isEmpty()) {
					return QValidator::Intermediate;
				}
				JID test("alice", "wonderland.lit", Q2PSTRING(input));

				return test.isValid() ? QValidator::Acceptable : QValidator::Invalid;
			}
	};

	class RoomJIDValidator : public QValidator {
		Q_OBJECT
		public:
			RoomJIDValidator(QObject* parent) : QValidator(parent) {
			}

			virtual QValidator::State validate(QString& input, int& /*pos*/) const {
				if (input.isEmpty()) {
					return QValidator::Intermediate;
				}
				JID roomJID(Q2PSTRING(input));

				if (roomJID.getNode().empty() || roomJID.getDomain().empty()) {
					return QValidator::Intermediate;
				}

				return (roomJID.getResource().empty() && !roomJID.getNode().empty() && !roomJID.getDomain().empty() &&  roomJID.isValid()) ? QValidator::Acceptable : QValidator::Invalid;
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
