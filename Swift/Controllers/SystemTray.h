#pragma once

namespace Swift {
	class SystemTray {
		public:
			virtual ~SystemTray(){};
			virtual void setUnreadMessages(bool some) = 0;
	};
}
