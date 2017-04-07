/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <gtest/gtest.h>

#include <QDateTime>
#include <QLocale>

#include <Swift/QtUI/QtUtilities.h>
#include <Swift/QtUI/QtUtilities.cpp>

TEST(QtUtilitiesTest, testDSTawareness) {
    QLocale::setDefault(QLocale(QLocale::English, QLocale::Germany));

    auto beforeDSTpoint = QDateTime(QDate(2017, 3, 26), QTime(0, 0));

    ASSERT_EQ(23 * 60 * 60, QtUtilities::secondsToNextMidnight(beforeDSTpoint));
}


TEST(QtUtilitiesTest, testNoDSTChange) {
    QLocale::setDefault(QLocale(QLocale::English, QLocale::Germany));

    auto beforeDSTpoint = QDateTime(QDate(2017, 3, 23), QTime(0, 0));

    ASSERT_EQ(24 * 60 * 60, QtUtilities::secondsToNextMidnight(beforeDSTpoint));
}
