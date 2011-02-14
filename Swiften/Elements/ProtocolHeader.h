/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

namespace Swift {
	class ProtocolHeader {
		public:
			ProtocolHeader() : version("1.0") {}

			const std::string& getTo() const { return to; }
			void setTo(const std::string& a)  { 
				to = a;
			}

			const std::string& getFrom() const { return from; }
			void setFrom(const std::string& a)  { 
				from = a;
			}

			const std::string& getVersion() const { return version; }
			void setVersion(const std::string& a)  { 
				version = a;
			}

			const std::string& getID() const { return id; }
			void setID(const std::string& a)  { 
				id = a;
			}
	
		private:
			std::string to;
			std::string from;
			std::string id;
			std::string version;
	};
}
