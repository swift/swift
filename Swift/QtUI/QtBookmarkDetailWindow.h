/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <QDialog>

#include <Swiften/MUC/MUCBookmark.h>

#include <Swift/QtUI/ui_QtBookmarkDetailWindow.h>

namespace Swift {
    class QtBookmarkDetailWindow : public QDialog, protected Ui::QtBookmarkDetailWindow {
        Q_OBJECT
        public:
            QtBookmarkDetailWindow(QWidget* parent = nullptr);
            virtual bool commit() = 0;
            boost::optional<MUCBookmark> createBookmarkFromForm();

        protected:
            void createFormFromBookmark(const MUCBookmark& bookmark);

        public slots:
            void accept();
    };
}

