/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Crypto/PlatformCryptoProvider.h>

#include <cassert>

#include <Swiften/Base/Platform.h>
#if defined(SWIFTEN_PLATFORM_WIN32)
#include <Swiften/Crypto/WindowsCryptoProvider.h>
#elif defined(HAVE_COMMONCRYPTO_CRYPTO_PROVIDER)
#include <Swiften/Crypto/CommonCryptoCryptoProvider.h>
#elif defined(HAVE_OPENSSL_CRYPTO_PROVIDER)
#include <Swiften/Crypto/OpenSSLCryptoProvider.h>
#endif

using namespace Swift;

CryptoProvider* PlatformCryptoProvider::create() {
#if defined(SWIFTEN_PLATFORM_WIN32)
    return new WindowsCryptoProvider();
#elif defined(HAVE_COMMONCRYPTO_CRYPTO_PROVIDER)
    return new CommonCryptoCryptoProvider();
#elif defined(HAVE_OPENSSL_CRYPTO_PROVIDER)
    return new OpenSSLCryptoProvider();
#endif
    assert(false);
    return nullptr;
}
