#ifndef SWIFTEN_WhitespacePingLayer_H
#define SWIFTEN_WhitespacePingLayer_H

#include <boost/noncopyable.hpp>

#include "Swiften/StreamStack/StreamLayer.h"

namespace Swift {
	class Timer;

	class WhitespacePingLayer : public StreamLayer, boost::noncopyable {
		public:
			WhitespacePingLayer();
			~WhitespacePingLayer();

			void writeData(const ByteArray& data);
			void handleDataRead(const ByteArray& data);

		private:
			void handleTimerTick();

		private:
			Timer* timer_;
	};
}

#endif
