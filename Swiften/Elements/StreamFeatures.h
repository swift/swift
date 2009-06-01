#ifndef SWIFTEN_StreamFeatures_H
#define SWIFTEN_StreamFeatures_H

#include <vector>
#include <algorithm>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Element.h"

namespace Swift {
	class StreamFeatures : public Element
	{
		public:
			StreamFeatures() : hasStartTLS_(false), hasResourceBind_(false), hasSession_(false) {}

			void setHasStartTLS() {
				hasStartTLS_ = true;
			}

			bool hasStartTLS() const {
				return hasStartTLS_;
			}

			void setHasSession() {
				hasSession_ = true;
			}

			bool hasSession() const {
				return hasSession_;
			}

			void setHasResourceBind() {
				hasResourceBind_ = true;
			}

			bool hasResourceBind() const {
				return hasResourceBind_;
			}

			const std::vector<String>& getCompressionMethods() const {
				return compressionMethods_;
			}

			void addCompressionMethod(const String& mechanism) {
				compressionMethods_.push_back(mechanism);
			}

			bool hasCompressionMethod(const String& mechanism) const {
				return std::find(compressionMethods_.begin(), compressionMethods_.end(), mechanism) != compressionMethods_.end();
			}

			const std::vector<String>& getAuthenticationMechanisms() const {
				return authenticationMechanisms_;
			}

			void addAuthenticationMechanism(const String& mechanism) {
				authenticationMechanisms_.push_back(mechanism);
			}

			bool hasAuthenticationMechanism(const String& mechanism) const {
				return std::find(authenticationMechanisms_.begin(), authenticationMechanisms_.end(), mechanism) != authenticationMechanisms_.end();
			}

			bool hasAuthenticationMechanisms() const {
				return !authenticationMechanisms_.empty();
			}

		private:
			bool hasStartTLS_;
			std::vector<String> compressionMethods_;
			std::vector<String> authenticationMechanisms_;
			bool hasResourceBind_;
			bool hasSession_;
	};
}

#endif
