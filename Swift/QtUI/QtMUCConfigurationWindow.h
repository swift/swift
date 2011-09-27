/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Elements/Form.h>

class QBoxLayout;

namespace Swift {
	class QtFormWidget;
	class QtMUCConfigurationWindow : public QWidget {
		Q_OBJECT
		public:
			QtMUCConfigurationWindow(Form::ref form);
			virtual ~QtMUCConfigurationWindow();
			boost::signal<void (Form::ref)> onFormComplete;
		private slots:
			void handleCancelClicked();
			void handleOKClicked();
		private:
			QtFormWidget* formWidget_;
			QPushButton* okButton_;
			QPushButton* cancelButton_;
	};
}
