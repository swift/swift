/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Swiften.h>
#include "EchoPayload.h"

using namespace Swift;

class EchoPayloadParser : public GenericPayloadParser<EchoPayload> {
	public:
		EchoPayloadParser() : currentDepth(0) {}

		void handleStartElement(
				const String& /* element */, const String& /* ns */, const AttributeMap&) {
			currentDepth++;
		}

		void handleEndElement(const String& /* element */, const String& /* ns */) {
			currentDepth--;
			if (currentDepth == 0) {
				getPayloadInternal()->setMessage(currentText);
			}
		}

		void handleCharacterData(const String& data) {
			currentText += data;
		}

	private:
		int currentDepth;
		String currentText;
};

class EchoPayloadParserFactory : public GenericPayloadParserFactory<EchoPayloadParser> {
	public:
		EchoPayloadParserFactory() :
			GenericPayloadParserFactory<EchoPayloadParser>("echo", "http://swift.im/echo") {}
};
