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

#include <Swift/QtUI/QtVCardWidget/QtVCardHomeWork.h>

namespace Swift {

QtVCardHomeWork::QtVCardHomeWork() : tagComboBox(nullptr) {
}

QtVCardHomeWork::~QtVCardHomeWork() {
}

void QtVCardHomeWork::setTagComboBox(QtTagComboBox* tagBox) {
    tagComboBox = tagBox;
    tagComboBox->addTag("home", QObject::tr("Home"));
    tagComboBox->addTag("work", QObject::tr("Work"));
}

void QtVCardHomeWork::setHome(const bool home) {
    tagComboBox->setTag("home", home);
}

bool QtVCardHomeWork::getHome() const {
    return tagComboBox->isTagSet("home");
}

void QtVCardHomeWork::setWork(const bool work) {
    tagComboBox->setTag("work", work);
}

bool QtVCardHomeWork::getWork() const {
    return tagComboBox->isTagSet("work");
}

}

