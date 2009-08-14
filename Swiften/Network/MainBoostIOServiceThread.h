#pragma once

namespace Swift {
	class BoostIOServiceThread;

	class MainBoostIOServiceThread {
		public:
			static BoostIOServiceThread& getInstance();
	};
}
