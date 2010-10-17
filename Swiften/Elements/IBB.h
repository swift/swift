/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Base/Shared.h"
#include "Swiften/Base/ByteArray.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class IBB : public Payload, public Shared<IBB> {
		public:
			enum Action {
				Open,
				Close,
				Data,
			};
			enum StanzaType {
				IQStanza,
				MessageStanza,
			};

			IBB(Action action = Open, const String& streamID = "") : action(action), streamID(streamID), stanzaType(IQStanza), blockSize(-1), sequenceNumber(-1) {
			}

			static IBB::ref createIBBOpen(const String& streamID, int blockSize) {
				IBB::ref result(new IBB(Open, streamID));
				result->setBlockSize(blockSize);
				return result;
			}

			static IBB::ref createIBBData(const String& streamID, int sequenceNumber, const ByteArray& data) {
				IBB::ref result(new IBB(Data, streamID));
				result->setSequenceNumber(sequenceNumber);
				result->setData(data);
				return result;
			}

			static IBB::ref createIBBClose(const String& streamID) {
				return IBB::ref(new IBB(Close, streamID));
			}

			void setAction(Action action) {
				this->action = action;
			}

			Action getAction() const {
				return action;
			}

			void setStanzaType(StanzaType stanzaType) {
				this->stanzaType = stanzaType;
			}

			StanzaType getStanzaType() const {
				return stanzaType;
			}

			void setStreamID(const String& id) {
				streamID = id;
			}

			const String& getStreamID() const {
				return streamID;
			}

			const ByteArray& getData() const {
				return data;
			}

			void setData(const ByteArray& data) {
				this->data = data;
			}

			int getBlockSize() const {
				return blockSize;
			}

			void setBlockSize(int blockSize) {
				this->blockSize = blockSize;
			}

			int getSequenceNumber() const {
				return sequenceNumber;
			}

			void setSequenceNumber(int i) {
				sequenceNumber = i;
			}

		private:
			Action action;
			String streamID;
			ByteArray data;
			StanzaType stanzaType;
			int blockSize;
			int sequenceNumber;
	};
}
