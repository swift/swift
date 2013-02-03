/*
 * Copyright (c) 2010-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Base/Log.h>

#include <cstdio>


namespace Swift {

static Log::Severity logLevel = Log::warning;

Log::Log() {
}

Log::~Log() {
	// Using stdio for thread safety (POSIX file i/o calls are guaranteed to be atomic)
	fprintf(stderr, "%s", stream.str().c_str());
	fflush(stderr);
}

std::ostringstream& Log::getStream(
		Severity /*severity*/, 
		const std::string& severityString, 
		const std::string& file, 
		int line,
		const std::string& function) {
	stream << "[" << severityString << "] " << file << ":" << line << " " << function << ": ";
	return stream;
}

Log::Severity Log::getLogLevel() {
	return logLevel;
}

void Log::setLogLevel(Severity level) {
	logLevel = level;
}

}
