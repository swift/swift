/*
 * Copyright (c) 2010-2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/IDGenerator.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>

namespace Swift {

IDGenerator::IDGenerator() {
}

std::string IDGenerator::generateID() {
	static boost::uuids::random_generator generator;
	return boost::lexical_cast<std::string>(generator());
}

}
