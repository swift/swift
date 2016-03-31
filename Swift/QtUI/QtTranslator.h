/*
 * Copyright (c) 2011 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QCoreApplication>

#include <Swift/Controllers/Translator.h>

class QtTranslator : public Swift::Translator {
    public:
        QtTranslator() {
        }

        virtual std::string translate(const std::string& text, const std::string& context) {
#if QT_VERSION >= 0x050000
            return std::string(QCoreApplication::translate(context.c_str(), text.c_str(), 0).toUtf8());
#else
            return std::string(QCoreApplication::translate(context.c_str(), text.c_str(), 0, QCoreApplication::UnicodeUTF8).toUtf8());
#endif
        }
};
