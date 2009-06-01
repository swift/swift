#ifndef SWIFTEN_EventLoop_H
#define SWIFTEN_EventLoop_H

#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

namespace Swift {
	class EventLoop {
		public:
			EventLoop();
			virtual ~EventLoop();
			
			void postEvent(boost::function<void ()> event, void* owner);
			void removeEventsFromOwner(void* owner);

		protected:
			struct Event {
				Event(void* owner, const boost::function<void()>& callback) :
						owner(owner), callback(callback) {
				}

				bool operator==(const Event& o) const {
					return o.id == id;
				}

				unsigned int id;
				void* owner;
				boost::function<void()> callback;
			};

			/**
			 * Reimplement this to call handleEvent(event) from the thread in which
			 * the event loop is residing.
			 */
			virtual void post(const Event& event) = 0;
		
			void handleEvent(const Event& event);

		private:
			struct HasOwner {
				HasOwner(void* owner) : owner(owner) {}
				bool operator()(const Event& event) { return event.owner == owner; }
				void* owner;
			};
			boost::mutex eventsMutex_;
			unsigned int nextEventID_;
			std::list<Event> events_;
	};
}

#endif
