/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>

#include <Swiften/Base/ByteArray.h>
#include <Swiften/Parser/XMLParserClient.h>

namespace Swift {
	class XMLParserFactory;

	class BOSHBodyExtractor {
			friend class BOSHBodyParserClient;
		public:
			struct BOSHBody {
				AttributeMap attributes;
				std::string content;
			};

			BOSHBodyExtractor(XMLParserFactory* parserFactory, const ByteArray& data);
			
			const boost::optional<BOSHBody>& getBody() const {
				return body;
			}

		private:
			boost::optional<BOSHBody> body;
	};
}
