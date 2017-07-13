/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>

namespace Swift {
    class SWIFTEN_API FileTransferOptions {
        public:
            FileTransferOptions() : allowInBand_(true), allowAssisted_(true), allowProxied_(true), allowDirect_(true) {
            }
            SWIFTEN_DEFAULT_COPY_CONSTRUCTOR(FileTransferOptions)
            ~FileTransferOptions();

            FileTransferOptions& withInBandAllowed(bool b) {
                allowInBand_ = b;
                return *this;
            }

            bool isInBandAllowed() const {
                return allowInBand_;
            }

            FileTransferOptions& withAssistedAllowed(bool b) {
                allowAssisted_ = b;
                return *this;
            }

            bool isAssistedAllowed() const {
                return allowAssisted_;
            }

            FileTransferOptions& withProxiedAllowed(bool b) {
                allowProxied_ = b;
                return *this;
            }

            bool isProxiedAllowed() const {
                return allowProxied_;
            }

            FileTransferOptions& withDirectAllowed(bool b) {
                allowDirect_ = b;
                return *this;
            }

            bool isDirectAllowed() const {
                return allowDirect_;
            }



            SWIFTEN_DEFAULT_COPY_ASSIGMNENT_OPERATOR(FileTransferOptions)

        private:
            bool allowInBand_;
            bool allowAssisted_;
            bool allowProxied_;
            bool allowDirect_;
    };
}
