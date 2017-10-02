/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <SwifTools/AutoUpdater/SparkleAutoUpdater.h>


#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

#include <Swiften/Base/Log.h>

#include <SwifTools/AutoUpdater/SparkleAutoUpdaterDelegate.h>
#include <SwifTools/Cocoa/CocoaUtil.h>

namespace Swift {

class SparkleAutoUpdater::Private {
    public:
        SUUpdater* updater;
        boost::intrusive_ptr<SparkleAutoUpdaterDelegate> delegate;
        State currentState = State::NotCheckedForUpdatesYet;
};

SparkleAutoUpdater::SparkleAutoUpdater(const std::string& appcastFeed) : d(new Private()) {
    d->updater = [SUUpdater sharedUpdater];
    [d->updater retain];

    d->delegate = boost::intrusive_ptr<SparkleAutoUpdaterDelegate>([[SparkleAutoUpdaterDelegate alloc] init], false);
    [d->delegate.get() setOnNewUpdateState: [&](AutoUpdater::State updatedState){
        setCurrentState(updatedState);
    }];
    [d->updater setDelegate: d->delegate.get()];

    [d->updater setAutomaticallyChecksForUpdates: true];
    // Automatically check for an update after a day.
    [d->updater setUpdateCheckInterval: 86400];

    auto canDoSilentUpdates = applicationInstallationLocationWritable();
    [d->updater setAutomaticallyDownloadsUpdates: canDoSilentUpdates];

    SWIFT_LOG(debug) << (canDoSilentUpdates ?
        "The current running user has enough permissions to do a silent update." :
        "The current running user has insufficient permissions to do a silent update.") << std::endl;

    setAppcastFeed(appcastFeed);
}

SparkleAutoUpdater::~SparkleAutoUpdater() {
    [d->updater release];
}

void SparkleAutoUpdater::setAppcastFeed(const std::string& appcastFeed) {
    NSURL* nsurl = [NSURL URLWithString: std2NSString(appcastFeed)];
    [d->updater setFeedURL: nsurl];
}

void SparkleAutoUpdater::checkForUpdates() {
    if (!(getCurrentState() == State::CheckingForUpdate ||
          getCurrentState() == State::DownloadingUpdate ||
          getCurrentState() == State::RestartToInstallUpdate)) {
        setCurrentState(State::CheckingForUpdate);
        [d->updater resetUpdateCycle];
        [d->updater checkForUpdatesInBackground];
    }
}

void SparkleAutoUpdater::setCurrentState(AutoUpdater::State updatedState) {
    d->currentState = updatedState;
    onUpdateStateChanged(d->currentState);
}

AutoUpdater::State SparkleAutoUpdater::getCurrentState() {
    return d->currentState;
}

bool Swift::SparkleAutoUpdater::applicationInstallationLocationWritable() {
    auto bundlePath = ns2StdString([[NSBundle mainBundle] bundlePath]);

    auto createTemporaryFile = [](const boost::filesystem::path& parentFolder) {
        boost::optional<boost::filesystem::path> tempFilePath;
        boost::system::error_code error;

        if (!boost::filesystem::is_directory(parentFolder, error) || error) {
            return tempFilePath;
        }
        auto uniquePath = boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%", error);
        if (error) {
            return tempFilePath;
        }
        auto testFilePath = parentFolder / uniquePath;

        boost::filesystem::ofstream testFile(testFilePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        if (testFile) {
            testFile.close();
            tempFilePath = testFilePath;
        }

        return tempFilePath;
    };

    auto isDirectoryWritable = [&](const boost::filesystem::path& path) {
        auto bundleTestFilePath = createTemporaryFile(path);
        if (!bundleTestFilePath) {
            return false;
        }

        boost::system::error_code error;
        if (!boost::filesystem::remove(bundleTestFilePath.get(), error) || error) {
            // File did not exist in the first place or error while removing it.
            return false;
        }

        return true;
    };

    auto applyMatchingPermissions = [](const boost::filesystem::path& permissionsFrom, const boost::filesystem::path& applyTo) {
        auto permissions = boost::filesystem::status(permissionsFrom).permissions();

        boost::system::error_code error;
        boost::filesystem::permissions(applyTo, permissions, error);

        return !error;
    };

    auto canChangePermissionsOnTemporaryFile = [&](const boost::filesystem::path& pathToCreateTemporaryFileUnder, const boost::filesystem::path& pathToTakePermissionsFrom) {
        auto temporaryFilePath = createTemporaryFile(pathToCreateTemporaryFileUnder);
        if (!temporaryFilePath) {
            return false;
        }

        boost::system::error_code error;
        auto fileExists = boost::filesystem::exists(temporaryFilePath.get(), error);
        if (!fileExists || error) {
            return false;
        }

        auto successfullPermissionCopy = applyMatchingPermissions(pathToTakePermissionsFrom, temporaryFilePath.get());

        boost::filesystem::remove(temporaryFilePath.get(), error);

        return successfullPermissionCopy;
    };

    auto bundleBoostPath = boost::filesystem::path(bundlePath);
    if (!isDirectoryWritable(bundleBoostPath.parent_path()) || !isDirectoryWritable(bundleBoostPath)) {
        return false;
    }

    return canChangePermissionsOnTemporaryFile(bundleBoostPath.parent_path(), bundleBoostPath);
}

}
