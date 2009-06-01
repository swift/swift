#include "Swiften/SASL/PLAINMessage.h"

namespace Swift {

PLAINMessage::PLAINMessage(const String& authcid, const String& password, const String& authzid) {
	String s = authzid + '\0' + authcid + '\0' + password;
	value_ = ByteArray(s.getUTF8Data(), s.getUTF8Size());
}

}
