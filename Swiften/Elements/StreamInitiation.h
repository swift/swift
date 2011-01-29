/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"
#include <Swiften/Elements/StreamInitiationFileInfo.h>

namespace Swift {
	class StreamInitiation : public Payload {
		public:
			typedef boost::shared_ptr<StreamInitiation> ref;

			StreamInitiation() : isFileTransfer(true) {}

			const String& getID() const {
				return id;
			}

			void setID(const String& id) {
				this->id = id;
			}

			const boost::optional<StreamInitiationFileInfo>& getFileInfo() const {
				return fileInfo;
			}

			void setFileInfo(const StreamInitiationFileInfo& info) {
				fileInfo = info;
			}

			const std::vector<String>& getProvidedMethods() const {
				return providedMethods;
			}

			void addProvidedMethod(const String& method) {
				providedMethods.push_back(method);
			}

			void setRequestedMethod(const String& method) {
				requestedMethod = method;
			}

			const String& getRequestedMethod() const {
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
			String id;
			boost::optional<StreamInitiationFileInfo> fileInfo;
			std::vector<String> providedMethods;
			String requestedMethod;
	};
}
