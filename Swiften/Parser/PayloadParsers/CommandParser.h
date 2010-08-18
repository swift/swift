/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include "Swiften/Elements/Command.h"
#include "Swiften/Parser/GenericPayloadParser.h"

namespace Swift {
	class FormParserFactory;
	class FormParser;

	class CommandParser : public GenericPayloadParser<Command> {
		public:
			CommandParser();
			~CommandParser();

			virtual void handleStartElement(const String& element, const String&, const AttributeMap& attributes);
			virtual void handleEndElement(const String& element, const String&);
			virtual void handleCharacterData(const String& data);

		private:
			static boost::optional<Command::Action> parseAction(const String& action);

		private:
			enum Level { 
				TopLevel = 0, 
				PayloadLevel = 1,
				FormOrNoteOrActionsLevel = 2,
				ActionsActionLevel = 3
			};
			int level_;
			bool inNote_;
			bool inActions_;
			Command::Note::Type noteType_;
			FormParserFactory* formParserFactory_;
			FormParser* formParser_;
			String currentText_;
	};
}
