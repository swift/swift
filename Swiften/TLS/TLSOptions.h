/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {

    struct TLSOptions {
        TLSOptions() : schannelTLS1_0Workaround(false) {

        }

        /**
         * A bug in the Windows SChannel TLS stack, combined with
         * overly-restrictive server stacks means it's sometimes necessary to
         * not use TLS>1.0. This option has no effect unless compiled on
         * Windows against SChannel (OpenSSL users are unaffected).
         */
        bool schannelTLS1_0Workaround;

    };
}
