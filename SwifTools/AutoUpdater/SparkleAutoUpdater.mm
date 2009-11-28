#include "SwifTools/AutoUpdater/SparkleAutoUpdater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

namespace Swift {

class SparkleAutoUpdater::Private {
	public:
		SUUpdater* updater;
};

SparkleAutoUpdater::SparkleAutoUpdater(const String& url) {
	d = new Private;

	d->updater = [SUUpdater sharedUpdater];
	[d->updater retain];
	[d->updater setAutomaticallyChecksForUpdates: true];

	NSURL* nsurl = [NSURL URLWithString:
			[NSString stringWithUTF8String: url.getUTF8Data()]];
	[d->updater setFeedURL: nsurl];
}

SparkleAutoUpdater::~SparkleAutoUpdater() {
	[d->updater release];
	delete d;
}

void SparkleAutoUpdater::checkForUpdates() {
	[d->updater checkForUpdatesInBackground];
}

}
