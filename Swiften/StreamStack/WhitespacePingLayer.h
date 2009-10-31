#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/StreamStack/StreamLayer.h"

namespace Swift {
	class Timer;

	class WhitespacePingLayer : public StreamLayer, boost::noncopyable {
		public:
			WhitespacePingLayer();

			void setActive();
			void setInactive();
			void writeData(const ByteArray& data);
			void handleDataRead(const ByteArray& data);

			bool getIsActive() const {
				return isActive;
			}

		private:
			void handleTimerTick();

		private:
			bool isActive;
			boost::shared_ptr<Timer> timer;
	};
}
