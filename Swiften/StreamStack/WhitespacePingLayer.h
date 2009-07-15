#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/StreamStack/StreamLayer.h"

namespace Swift {
	class Timer;

	class WhitespacePingLayer : public StreamLayer, boost::noncopyable {
		public:
			WhitespacePingLayer();

			void writeData(const ByteArray& data);
			void handleDataRead(const ByteArray& data);

		private:
			void handleTimerTick();

		private:
			boost::shared_ptr<Timer> timer;
	};
}
