/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QObject>

#include <Swift/QtUI/QtVCardWidget/QtTagComboBox.h>

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
