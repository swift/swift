/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swift/Controllers/Translator.h>
#define QT_TRANSLATE_NOOP(context, text) \
    Swift::Translator::getInstance()->translate(text, context)
