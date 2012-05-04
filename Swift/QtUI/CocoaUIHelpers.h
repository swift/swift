/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/TLS/Certificate.h>
#include <QWidget>

namespace Swift {

class CocoaUIHelpers {
public:
	static void displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain);
};

}

