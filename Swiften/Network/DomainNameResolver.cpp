#include "Swiften/Network/DomainNameResolver.h"

#include <idna.h>

namespace Swift {

DomainNameResolver::~DomainNameResolver() {
}

String DomainNameResolver::getNormalized(const String& domain) {
	char* output;
	if (idna_to_ascii_8z(domain.getUTF8Data(), &output, 0) == IDNA_SUCCESS) {
		String result(output);
		free(output);
		return result;
	}
	else {
		return domain;
	}
}

}
