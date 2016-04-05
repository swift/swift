/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <boost/cstdint.hpp>
#include <boost/optional.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/FileTransfer/FileTransferError.h>

namespace Swift {
    /**
     * The FileTransfer class provides a general interface for file-transfer
     * implmenetations. Currently, only Jingle File Transfer based on XEP-0234 is
     * implementated in the \ref OutgoingJingleFileTransfer and
     * \ref IncomingJingleFileTransfer classes.
     */
    class SWIFTEN_API FileTransfer {
        public:
            struct State {
                enum Type {
                    Initial,
                    WaitingForStart,
                    Negotiating,
                    WaitingForAccept,
                    Transferring,
                    Canceled,
                    Failed,
                    Finished
                };

                State(Type type, const std::string& message = "") : type(type), message(message) {}

                Type type;
                std::string message;
            };
            typedef std::shared_ptr<FileTransfer> ref;

        public:
            FileTransfer();
            virtual ~FileTransfer();

            virtual void cancel() = 0;

            const std::string& getFileName() const {
                return filename_;
            }

            boost::uintmax_t getFileSizeInBytes() const {
                return fileSizeInBytes_;
            }

            const State& getState() const {
                return state_;
            }

            const std::string& getDescription() const {
                return description_;
            }

        public:
            boost::signals2::signal<void (size_t /* proccessedBytes */)> onProcessedBytes;
            boost::signals2::signal<void (const State&)> onStateChanged;
            boost::signals2::signal<void (boost::optional<FileTransferError>)> onFinished;

        protected:
            void setState(const State& state);
            void setFileInfo(const std::string& name, boost::uintmax_t size, const std::string& description);

        private:
            boost::uintmax_t fileSizeInBytes_;
            std::string filename_;
            std::string description_;
            State state_;
    };
}
