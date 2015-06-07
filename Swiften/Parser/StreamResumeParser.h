/*
 * Copyright (c) 2011-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericElementParser.h>
#include <Swiften/Elements/StreamResume.h>

namespace Swift {
	class SWIFTEN_API StreamResumeParser : public GenericElementParser<StreamResume> {
		public:
			StreamResumeParser();
			~StreamResumeParser();

			virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
			virtual void handleEndElement(const std::string&, const std::string&);

		private:
			enum Level { 
				TopLevel = 0
			};
			int level;
	};
}
