/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/WinUIHelpers.h>

#include <windows.h>
#include <Wincrypt.h>
#include <cryptuiapi.h>
#pragma comment(lib, "cryptui.lib")

#include <memory>

namespace Swift {

void WinUIHelpers::displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain) {
    if (chain.empty()) {
        return;
    }

    // create certificate store to store the certificate chain in
    HCERTSTORE chainStore = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, NULL, CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG, NULL);
    if (!chainStore) {
        return;
    }

    ByteArray certAsDER = chain[0]->toDER();
    std::shared_ptr<const CERT_CONTEXT> certificate_chain;
    {
        PCCERT_CONTEXT certChain;
        BOOL ok = CertAddCertificateContextToStore(chainStore, CertCreateCertificateContext(X509_ASN_ENCODING, vecptr(certAsDER), certAsDER.size()), CERT_STORE_ADD_ALWAYS, &certChain);
        // maybe free the cert contex we created
        if (!ok || !certChain) {
            return;
        }
        certificate_chain.reset(certChain, CertFreeCertificateContext);
    }

    for (size_t i = 1; i < chain.size(); ++i) {
        ByteArray certAsDER = chain[i]->toDER();
        CertAddCertificateContextToStore(chainStore, CertCreateCertificateContext(X509_ASN_ENCODING, vecptr(certAsDER), certAsDER.size()), CERT_STORE_ADD_ALWAYS, NULL);
    }

    CRYPTUI_VIEWCERTIFICATE_STRUCT viewDialogProperties = { 0 };
    viewDialogProperties.dwSize = sizeof(viewDialogProperties);
    viewDialogProperties.hwndParent = (HWND) parent->winId();
    viewDialogProperties.dwFlags = CRYPTUI_DISABLE_EDITPROPERTIES | CRYPTUI_DISABLE_ADDTOSTORE | CRYPTUI_ENABLE_REVOCATION_CHECKING;
    viewDialogProperties.pCertContext = certificate_chain.get();
    viewDialogProperties.cStores = 1;
    viewDialogProperties.rghStores = &chainStore;
    BOOL properties_changed;

    // blocking call that shows modal certificate dialog
    BOOL rv = ::CryptUIDlgViewCertificate(&viewDialogProperties, &properties_changed);
}

}
