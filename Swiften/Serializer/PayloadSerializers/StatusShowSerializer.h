#ifndef SWIFTEN_StatusShowSerializer_H
#define SWIFTEN_StatusShowSerializer_H

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/StatusShow.h"

namespace Swift {
	class StatusShowSerializer : public GenericPayloadSerializer<StatusShow> {
		public:
			StatusShowSerializer() : GenericPayloadSerializer<StatusShow>() {}

			virtual String serializePayload(boost::shared_ptr<StatusShow> statusShow)  const {
				if (statusShow->getType () == StatusShow::Online || statusShow->getType() == StatusShow::None) {
					return "";
				}
				else {
					String result("<show>");
					switch (statusShow->getType()) {
						case StatusShow::Away: result += "away"; break;
						case StatusShow::XA: result += "xa"; break;
						case StatusShow::FFC: result += "chat"; break;
						case StatusShow::DND: result += "dnd"; break;
						case StatusShow::Online: assert(false); break;
						case StatusShow::None: assert(false); break;
					}
					result += "</show>";
					return result;
				}
			}
	};
}

#endif
