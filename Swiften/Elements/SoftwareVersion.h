#ifndef SWIFTEN_SoftwareVersion_H
#define SWIFTEN_SoftwareVersion_H

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class SoftwareVersion : public Payload {
		public:
			SoftwareVersion(
					const String& name = "", 
					const String& version = "",
					const String& os = "") : 
						name_(name), version_(version), os_(os) {}
			
			const String& getName() const {
				return name_;
			}

			void setName(const String& name) {
				name_ = name;
			}

			const String& getVersion() const {
				return version_;
			}

			void setVersion(const String& version) {
				version_ = version;
			}

			const String& getOS() const {
				return os_;
			}

			void setOS(const String& os) {
				os_ = os;
			}

		private:
			String name_;
			String version_;
			String os_;
	};
}

#endif
