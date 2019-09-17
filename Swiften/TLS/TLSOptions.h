/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

namespace Swift {
    class TLSContext;

    class TLSOptions {
      public:
        TLSOptions() : schannelTLS1_0Workaround(false) {

        }

        /**
         * A bug in the Windows SChannel TLS stack, combined with
         * overly-restrictive server stacks means it's sometimes necessary to
         * not use TLS>1.0. This option has no effect unless compiled on
         * Windows against SChannel (OpenSSL users are unaffected).
         */
        bool schannelTLS1_0Workaround;

        /**
         * OpenSSL configuration flags
         */
        boost::optional<bool> workaroundMicrosoftSessID;
        boost::optional<bool> workaroundNetscapeChallenge;
        boost::optional<bool> workaroundNetscapeReuseCipherChange;
        boost::optional<bool> workaroundSSLRef2ReuseCertType;
        boost::optional<bool> workaroundMicrosoftBigSSLv3Buffer;
        boost::optional<bool> workaroundSSLeay080ClientDH;
        boost::optional<bool> workaroundTLSD5;
        boost::optional<bool> workaroundTLSBlockPadding;
        boost::optional<bool> workaroundDontInsertEmptyFragments;
        boost::optional<bool> workaroundAll;
        boost::optional<bool> suppressSSLv2;
        boost::optional<bool> suppressSSLv3;
        boost::optional<bool> suppressTLSv1;
        boost::optional<bool> disableTLSRollBackBug;
        boost::optional<bool> singleDHUse;

        /**
         * Other OpenSSL configuration items
         */
        boost::optional<std::string> cipherSuites;
        boost::optional<std::string> context;
        boost::optional<int> sessionCacheTimeout;
        boost::optional<int> verifyDepth;

        enum class VerifyMode {
            None,
            Required,
            Optional
        };
        boost::optional<VerifyMode> verifyMode;

        /**
         * Callback for certificate verification
         */

        boost::optional<std::function<int(const TLSContext *)>> verifyCertificateCallback;
    };
}
