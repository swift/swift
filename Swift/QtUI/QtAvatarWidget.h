/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QImage>
#include <QWidget>

#include <Swiften/Base/ByteArray.h>

class QLabel;

namespace Swift {
    class QtAvatarWidget : public QWidget {
            Q_OBJECT
            Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
        public:
            QtAvatarWidget(QWidget* parent);

            void setAvatar(const ByteArray& data, const std::string& type);

            const ByteArray& getAvatarData() const {
                return data;
            }

            const std::string& getAvatarType() const {
                return type;
            }

            void setEditable(bool b) {
                editable = b;
            }

            bool isEditable() const {
                return editable;
            }

            void mousePressEvent(QMouseEvent* event);

        private:
            bool editable;
            ByteArray data;
            std::string type;
            QLabel* label;
    };
}
