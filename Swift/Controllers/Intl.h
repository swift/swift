/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swift/Controllers/Translator.h>

#define QT_TRANSLATE_NOOP(context, text) \
	Swift::Translator::getInstance()->translate(text, context)
