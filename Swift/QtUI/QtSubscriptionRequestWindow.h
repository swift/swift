/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QDialog>

#include <boost/shared_ptr.hpp>

#include "Swift/Controllers/XMPPEvents/SubscriptionRequestEvent.h"

namespace Swift {
	class QtSubscriptionRequestWindow : public QDialog {
		Q_OBJECT
		public:
			static QtSubscriptionRequestWindow* getWindow(boost::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent = 0);
			~QtSubscriptionRequestWindow();
			boost::shared_ptr<SubscriptionRequestEvent> getEvent();
		private slots:
			void handleYes();
			void handleNo();
			void handleDefer();
		private:
			QtSubscriptionRequestWindow(boost::shared_ptr<SubscriptionRequestEvent> event, QWidget* parent = 0);
			static QList<QtSubscriptionRequestWindow*> windows_;
			boost::shared_ptr<SubscriptionRequestEvent> event_;
			/*QPushButton* yesButton_;
			QPushButton* noButton_;
			QPushButton* deferButton_;*/
	};
}
