/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include <Swiften/Elements/Element.h>

namespace Swift {
	class StreamFeatures : public Element	{
		public:
			typedef boost::shared_ptr<StreamFeatures> ref;

			StreamFeatures() : hasStartTLS_(false), hasResourceBind_(false), hasSession_(false), hasStreamManagement_(false), hasRosterVersioning_(false) {}

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

			const std::vector<std::string>& getCompressionMethods() const {
				return compressionMethods_;
			}

			void addCompressionMethod(const std::string& mechanism) {
				compressionMethods_.push_back(mechanism);
			}

			bool hasCompressionMethod(const std::string& mechanism) const;

			const std::vector<std::string>& getAuthenticationMechanisms() const {
				return authenticationMechanisms_;
			}

			void addAuthenticationMechanism(const std::string& mechanism) {
				authenticationMechanisms_.push_back(mechanism);
			}

			bool hasAuthenticationMechanism(const std::string& mechanism) const;

			bool hasAuthenticationMechanisms() const {
				return !authenticationMechanisms_.empty();
			}

			bool hasStreamManagement() const {
				return hasStreamManagement_;
			}

			void setHasStreamManagement() {
				hasStreamManagement_ = true;
			}

			bool hasRosterVersioning() const {
				return hasRosterVersioning_;
			}

			void setHasRosterVersioning() {
				hasRosterVersioning_ = true;
			}

		private:
			bool hasStartTLS_;
			std::vector<std::string> compressionMethods_;
			std::vector<std::string> authenticationMechanisms_;
			bool hasResourceBind_;
			bool hasSession_;
			bool hasStreamManagement_;
			bool hasRosterVersioning_;
	};
}
