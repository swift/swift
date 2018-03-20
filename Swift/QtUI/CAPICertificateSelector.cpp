/*
 * Copyright (c) 2012 Isode Limited, London, England.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <string>

#include <Swift/QtUI/CAPICertificateSelector.h>

#define SECURITY_WIN32
#include <Windows.h>
#include <WinCrypt.h>
#include <cryptuiapi.h>


#include <Swiften/StringCodecs/Hexify.h>
#include <boost/algorithm/string.hpp>
#include <Swift/Controllers/Intl.h>
#include <Swift/QtUI/QtSwiftUtil.h>

#include <Swiften/Base/Log.h>
#include <Swiften/TLS/Schannel/SchannelUtil.h>

namespace Swift {

/////Hmm, maybe we should not exlude the "location" column
#define exclude_columns     CRYPTUI_SELECT_LOCATION_COLUMN | CRYPTUI_SELECT_INTENDEDUSE_COLUMN

#define SHA1_HASH_LENGTH 20

static std::string getCertUri(PCCERT_CONTEXT cert, const char * cert_store_name) {
    DWORD cbHash = SHA1_HASH_LENGTH;
    BYTE aHash[SHA1_HASH_LENGTH];
    std::string result("certstore:");

    result += cert_store_name;
    result += ":sha1:";

    if (CertGetCertificateContextProperty(cert, CERT_HASH_PROP_ID, aHash, &cbHash) == FALSE ) {
        return "";
    }

    ByteArray byteArray = createByteArray((char *)(&aHash[0]), cbHash);
    result += Hexify::hexify(byteArray);

    return result;
}

std::string selectCAPICertificate() {
    const char* certStoreName = "MY";

    DWORD storeFlags = CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG | CERT_SYSTEM_STORE_CURRENT_USER;

    HCERTSTORE hstore = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, 0, 0, storeFlags, certStoreName);
    if (!hstore) {
        return "";
    }

    HWND hwnd = GetForegroundWindow();
    if (!hwnd) {
        hwnd = GetActiveWindow();
    }

    std::string certificateDialogTitle = QT_TRANSLATE_NOOP("", "TLS Client Certificate Selection");
    std::string certificateDialogPrompt = QT_TRANSLATE_NOOP("", "Select a certificate to use for authentication");

    int titleLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, certificateDialogTitle.c_str(), -1, NULL, 0);
    int promptLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, certificateDialogPrompt.c_str(), -1, NULL, 0);

    wchar_t* titleChars = new wchar_t[titleLength];
    wchar_t* promptChars = new wchar_t[promptLength];

    //titleChars[titleLength] = '\0';
    //promptChars[promptLength] = '\0';

    titleLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, certificateDialogTitle.c_str(), -1, titleChars, titleLength);
    promptLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, certificateDialogPrompt.c_str(), -1, promptChars, promptLength);

    if (titleLength == 0 || promptLength == 0) {
        int error = GetLastError();
        switch (error) {
            case ERROR_INSUFFICIENT_BUFFER: SWIFT_LOG(error) << "Insufficient buffer for rendering cert dialog" << std::endl;break;
            case ERROR_INVALID_FLAGS: SWIFT_LOG(error) << "Invalid flags for rendering cert dialog" << std::endl;break;
            case ERROR_INVALID_PARAMETER: SWIFT_LOG(error) << "Invalid parameter for rendering cert dialog" << std::endl;break;
            case ERROR_NO_UNICODE_TRANSLATION: SWIFT_LOG(error) << "Invalid unicode for rendering cert dialog" << std::endl;break;
            default: SWIFT_LOG(error) << "Unexpected multibyte conversion errorcode" << std::endl;

        }
    }

    std::string result;
    /* Call Windows dialog to select a suitable certificate */
    {
        ScopedCertContext cert(CryptUIDlgSelectCertificateFromStore(hstore, hwnd, titleChars, promptChars, exclude_columns, 0, NULL));
        if (cert) {
            result = getCertUri(cert, certStoreName);
        }
    }

    delete[] titleChars;
    delete[] promptChars;

    if (hstore) {
        if (CertCloseStore(hstore, 0) == FALSE) {
            SWIFT_LOG(debug) << "Failed to close the certificate store handle." << std::endl;
        }
    }

    return result;
}

bool isCAPIURI(std::string uri) {
    return (boost::iequals(uri.substr(0, 10), "certstore:"));
}

}
