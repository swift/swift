/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Serializer/GenericPayloadSerializer.h"
#include "Swiften/Elements/Command.h"

namespace Swift {
	class PayloadSerializerCollection;

	class CommandSerializer : public GenericPayloadSerializer<Command> {
		public:
			CommandSerializer();

			virtual String serializePayload(boost::shared_ptr<Command>)  const;

		private:
			String actionToString(Command::Action action) const;
	};
}
