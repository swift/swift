/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QObject>

#include "QtTagComboBox.h"

namespace Swift {

class QtVCardHomeWork {
	public:
		QtVCardHomeWork();
		virtual ~QtVCardHomeWork();

		void setTagComboBox(QtTagComboBox* tagBox);

		void setHome(const bool home);
		bool getHome() const;
		void setWork(const bool work);
		bool getWork() const;

	private:
		QtTagComboBox* tagComboBox;
};

}
