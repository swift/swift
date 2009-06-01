#ifndef SWIFTEN_MainEventLoop_H
#define SWIFTEN_MainEventLoop_H

#include <boost/function.hpp>

#include "Swiften/EventLoop/Deleter.h"
#include "Swiften/EventLoop/EventLoop.h"

namespace Swift {
	class EventLoop;

	class MainEventLoop {
			friend class EventLoop;

		public:
			/**
			 * Post an event from the given owner to the event loop.
			 * If the owner is destroyed, all events should be removed from the
			 * loop using removeEventsFromOwner().
			 */
			static void postEvent(boost::function<void ()> event, void* owner = 0);

			static void removeEventsFromOwner(void* owner);

			template<typename T>
			static void deleteLater(T* t) {
				getInstance()->postEvent(Deleter<T>(t), 0);
			}

		private:
			static void setInstance(EventLoop*);
			static void resetInstance();
			static EventLoop* getInstance();
		
		private:
			static EventLoop* instance_;
	};
}

#endif
