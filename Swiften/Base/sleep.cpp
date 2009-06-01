#include "Swiften/Base/sleep.h"

#include <boost/thread.hpp>

namespace Swift {

void sleep(unsigned int msecs) {
  boost::xtime xt;
  boost::xtime_get(&xt, boost::TIME_UTC);
  xt.nsec += msecs*1000000;
  boost::thread::sleep(xt);
}

}
