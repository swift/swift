/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace Swift {
	class JingleSessionManager;
	class IQRouter;
	class FileTransferTransporterFactory;
	class OutgoingFileTransfer;
	class JID;
	class IDGenerator;
	class ReadBytestream;
	class StreamInitiationFileInfo;
	class CryptoProvider;
	class FileTransferOptions;

	class OutgoingFileTransferManager {
		public:
			OutgoingFileTransferManager(
					JingleSessionManager* jingleSessionManager, 
					IQRouter* router, 
					FileTransferTransporterFactory* transporterFactory,
					CryptoProvider* crypto);
			~OutgoingFileTransferManager();
			
			boost::shared_ptr<OutgoingFileTransfer> createOutgoingFileTransfer(
					const JID& from, 
					const JID& to, 
					boost::shared_ptr<ReadBytestream>, 
					const StreamInitiationFileInfo&,
					const FileTransferOptions&);

		private:
			JingleSessionManager* jingleSessionManager;
			IQRouter* iqRouter;
			FileTransferTransporterFactory* transporterFactory;
			IDGenerator* idGenerator;
			CryptoProvider* crypto;
	};
}
