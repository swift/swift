/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/StreamInitiationFileInfo.h>

namespace Swift {
	class StreamInitiation : public Payload {
		public:
			typedef boost::shared_ptr<StreamInitiation> ref;

			StreamInitiation() : isFileTransfer(true) {}

			const std::string& getID() const {
				return id;
			}

			void setID(const std::string& id) {
				this->id = id;
			}

			const boost::optional<StreamInitiationFileInfo>& getFileInfo() const {
				return fileInfo;
			}

			void setFileInfo(const StreamInitiationFileInfo& info) {
				fileInfo = info;
			}

			const std::vector<std::string>& getProvidedMethods() const {
				return providedMethods;
			}

			void addProvidedMethod(const std::string& method) {
				providedMethods.push_back(method);
			}

			void setRequestedMethod(const std::string& method) {
				requestedMethod = method;
			}

			const std::string& getRequestedMethod() const {
				return requestedMethod;
			}

			bool getIsFileTransfer() const {
				return isFileTransfer;
			}

			void setIsFileTransfer(bool b) {
				isFileTransfer = b;
			}

		private:
			bool isFileTransfer;
			std::string id;
			boost::optional<StreamInitiationFileInfo> fileInfo;
			std::vector<std::string> providedMethods;
			std::string requestedMethod;
	};
}
