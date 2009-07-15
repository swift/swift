#pragma once

#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <list>

namespace Swift {
	class EventOwner;
	class EventLoop {
		public:
			EventLoop();
			virtual ~EventLoop();
			
			void postEvent(boost::function<void ()> event, boost::shared_ptr<EventOwner> owner = boost::shared_ptr<EventOwner>());
			void removeEventsFromOwner(boost::shared_ptr<EventOwner> owner);

		protected:
			struct Event {
				Event(boost::shared_ptr<EventOwner> owner, const boost::function<void()>& callback) :
						owner(owner), callback(callback) {
				}

				bool operator==(const Event& o) const {
					return o.id == id;
				}

				unsigned int id;
				boost::shared_ptr<EventOwner> owner;
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
				HasOwner(boost::shared_ptr<EventOwner> owner) : owner(owner) {}
				bool operator()(const Event& event) { return event.owner == owner; }
				boost::shared_ptr<EventOwner> owner;
			};
			boost::mutex eventsMutex_;
			unsigned int nextEventID_;
			std::list<Event> events_;
	};
}
