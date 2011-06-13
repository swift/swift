/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Elements/Payload.h>

namespace Swift {
	class IBB : public Payload {
		public:
			typedef boost::shared_ptr<IBB> ref;

			enum Action {
				Open,
				Close,
				Data,
			};
			enum StanzaType {
				IQStanza,
				MessageStanza,
			};

			IBB(Action action = Open, const std::string& streamID = "") : action(action), streamID(streamID), stanzaType(IQStanza), blockSize(-1), sequenceNumber(-1) {
			}

			static IBB::ref createIBBOpen(const std::string& streamID, int blockSize) {
				IBB::ref result = boost::make_shared<IBB>(Open, streamID);
				result->setBlockSize(blockSize);
				return result;
			}

			static IBB::ref createIBBData(const std::string& streamID, int sequenceNumber, const std::vector<unsigned char>& data) {
				IBB::ref result = boost::make_shared<IBB>(Data, streamID);
				result->setSequenceNumber(sequenceNumber);
				result->setData(data);
				return result;
			}

			static IBB::ref createIBBClose(const std::string& streamID) {
				return boost::make_shared<IBB>(Close, streamID);
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

			void setStreamID(const std::string& id) {
				streamID = id;
			}

			const std::string& getStreamID() const {
				return streamID;
			}

			const std::vector<unsigned char>& getData() const {
				return data;
			}

			void setData(const std::vector<unsigned char>& data) {
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
			std::string streamID;
			std::vector<unsigned char> data;
			StanzaType stanzaType;
			int blockSize;
			int sequenceNumber;
	};
}
