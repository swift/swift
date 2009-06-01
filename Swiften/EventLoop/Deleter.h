#ifndef SWIFTEN_Deleter_H
#define SWIFTEN_Deleter_H

#include <cassert>

namespace Swift {
	template<typename T>
	class Deleter {
		public:
			Deleter(T* object) : object_(object) {
			}

			void operator()() {
				assert(object_);
				delete object_;
				object_ = 0;
			}

		private:
			T* object_;
	};
}
#endif
