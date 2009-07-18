#pragma once

#include "Swiften/Base/String.h"

namespace Swift {
	class ProtocolHeader {
		public:
			ProtocolHeader() : version("1.0") {}

			const String& getTo() const { return to; }
			void setTo(const String& a)  { 
				to = a;
			}

			const String& getFrom() const { return from; }
			void setFrom(const String& a)  { 
				from = a;
			}

			const String& getVersion() const { return version; }
			void setVersion(const String& a)  { 
				version = a;
			}

			const String& getID() const { return id; }
			void setID(const String& a)  { 
				id = a;
			}
	
		private:
			String to;
			String from;
			String id;
			String version;
	};
}
