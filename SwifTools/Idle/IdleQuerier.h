#pragma once

namespace Swift {
	class IdleQuerier {
		public:
			virtual ~IdleQuerier();

			virtual int getIdleTimeSeconds() = 0;
	};
}
