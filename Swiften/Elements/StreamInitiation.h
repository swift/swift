/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <boost/optional.hpp>

#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class StreamInitiation : public Payload, public Shared<StreamInitiation> {
		public:
			struct FileInfo {
				FileInfo(const String& name = "", const String& description = "", int size = -1) : name(name), description(description), size(size) {}

				String name;
				String description;
				int size;
			};

			StreamInitiation() : isFileTransfer(true) {}

			const String& getID() const {
				return id;
			}

			void setID(const String& id) {
				this->id = id;
			}

			const boost::optional<FileInfo>& getFileInfo() const {
				return fileInfo;
			}

			void setFileInfo(const FileInfo& info) {
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
			boost::optional<FileInfo> fileInfo;
			std::vector<String> providedMethods;
			String requestedMethod;
	};
}
