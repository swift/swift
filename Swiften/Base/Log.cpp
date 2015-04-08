/*
 * Copyright (c) 2010-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/Log.h>

#include <cstdio>

#if defined(SWIFT_ANDROID_LOGGING) && defined(__ANDROID__)
#include <android/log.h>
#endif


namespace Swift {

static Log::Severity logLevel = Log::warning;

Log::Log() {
}

Log::~Log() {
#if defined(SWIFT_ANDROID_LOGGING) && defined(__ANDROID__)
	__android_log_print(ANDROID_LOG_VERBOSE, "Swift", stream.str().c_str(), 1);
#else
	// Using stdio for thread safety (POSIX file i/o calls are guaranteed to be atomic)
	fprintf(stderr, "%s", stream.str().c_str());
	fflush(stderr);
#endif
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
