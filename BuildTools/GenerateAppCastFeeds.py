#!/usr/bin/env python2

# This script generates three app cast feeds for macOS Sparkle updates from Swift releases in the download folder on the Swift website.

from xml.etree import ElementTree as ET
import argparse
import datetime
import email.utils as eut
import fnmatch
import jinja2
import os.path
import re
import time
import urllib2
import urlparse

class Release:
    def __init__(self, version, absoluteURL, sizeInBytes, date):
        self.version = version
        self.shortVersion = version.split('-', 1)[1]
        self.url = absoluteURL
        self.sizeInBytes = sizeInBytes
        self.date = date
        dateTumple = date.timetuple()
        dateTimestamp = time.mktime(dateTumple)
        self.dateString = eut.formatdate(dateTimestamp)

    def __str__(self):
        return "Release(%s, %s, %s, %s)" % (self.version, self.url, self.sizeInBytes, self.date)

    def __repr__(self):
        return "Release(%s, %s, %s, %s)" % (self.version, self.url, self.sizeInBytes, self.date)

def getReleaseFromAbsoluteFilePath(absolutePath, downloadsFolder, downloadsURL):
    version = os.path.splitext(absolutePath.split('/')[-1])[0]
    sizeInBytes = os.path.getsize(absolutePath)
    date = datetime.datetime.fromtimestamp(os.path.getmtime(absolutePath))
    absoluteURL = urlparse.urljoin(downloadsURL, os.path.relpath(absolutePath, downloadsFolder))
    return Release(version, absoluteURL, sizeInBytes, date)

def getReleaseFromReleaseFolder(releaseFolder, downloadsFolder, downloadsURL, extension):
    release = None
    regex = re.compile(fnmatch.translate(extension))

    files = [f for f in os.listdir(releaseFolder) if os.path.isfile(os.path.join(releaseFolder, f))]
    for file in files:
        fileFullPath = os.path.join(releaseFolder, file)
        if regex.match(fileFullPath):
            release = getReleaseFromAbsoluteFilePath(fileFullPath, downloadsFolder, downloadsURL)
    return release

def getReleaseFilesInDownloadsFolder(downloadsFolder, downloadsURL, extension):
    releasesFolder = os.path.join(downloadsFolder, "releases")
    releases = []

    dirs = [d for d in os.listdir(releasesFolder) if os.path.isdir(os.path.join(releasesFolder, d))]
    for d in dirs:
        release = getReleaseFromReleaseFolder(os.path.join(releasesFolder, d), downloadsFolder, downloadsURL, extension)
        if release:
            releases.append(release)

    return releases

def writeAppcastFile(filename, title, description, regexPattern, appcastURL, releases):
    template = jinja2.Template('''<rss xmlns:sparkle="http://www.andymatuschak.org/xml-namespaces/sparkle" xmlns:dc="http://purl.org/dc/elements/1.1/" version="2.0">
    <channel>
    <title>{{ title }}</title>
    <link>{{ appcast_url }}</link>
    <description>{{ description }}</description>
    <language>en</language>
    {% for item in releases %}<item>
        <title>Swift version {{ item.version }}</title>
            <pubDate>{{ item.dateString }}</pubDate>
            <enclosure url="{{ item.url }}"
                sparkle:version="{{ item.version }}"
                sparkle:shortVersionString="{{ item.shortVersion }}"
                length="{{ item.sizeInBytes }}"
                type="application/octet-stream" />
    </item>
    {% endfor %}</channel>
</rss>''')

    matchingReleases = [i for i in releases if re.match(regexPattern, i.version)]
    matchingReleases = matchingReleases[:2] # only include the first two matches in the appcast

    appcastContent = template.render(title=title, appcast_url=appcastURL, description=description, releases=matchingReleases)

    contentParsesOK = False
    try:
        x = ET.fromstring(appcastContent)
        contentParsesOK = True
    except :
        contentParsesOK = False

    if contentParsesOK:
        with open(filename, 'w') as file:
            file.write(appcastContent)
    else:
        print("Failed to generate valid appcast feed %s." % filename)

parser = argparse.ArgumentParser(description='Generate stable/testing/development appcast feeds for Sparkle updater.')
parser.add_argument('downloadsFolder', type=str, help="e.g. /Users/foo/website/downloads/")
parser.add_argument('downloadsURL', type=str, help="e.g. https://swift.im/downloads/")

args = parser.parse_args()

releases = getReleaseFilesInDownloadsFolder(args.downloadsFolder, args.downloadsURL, "*.dmg")

releases.sort(key=lambda release: release.date, reverse=True)

writeAppcastFile(filename=os.path.join(args.downloadsFolder, "swift-stable-appcast-mac.xml"),
    title="Swift Stable Releases",
    description="",
    regexPattern="^Swift\-\d+(\.\d+)?(\.\d+)?$",
    appcastURL=urlparse.urljoin(args.downloadsURL, "swift-stable-appcast-mac.xml"),
    releases=releases)
writeAppcastFile(filename=os.path.join(args.downloadsFolder, "swift-testing-appcast-mac.xml"),
    title="Swift Testing Releases",
    description="",
    regexPattern="^Swift\-\d+(\.\d+)?(\.\d+)?(beta\d+)?(rc\d+)?$",
    appcastURL=urlparse.urljoin(args.downloadsURL, "swift-testing-appcast-mac.xml"),
    releases=releases)
writeAppcastFile(filename=os.path.join(args.downloadsFolder, "swift-development-appcast-mac.xml"),
    title="Swift Development Releases",
    description="",
    regexPattern="^Swift\-\d+(\.\d+)?(\.\d+)?(alpha)?(beta\d+)?(rc\d+)?(-dev\d+)?$",
    appcastURL=urlparse.urljoin(args.downloadsURL, "swift-development-appcast-mac.xml"),
    releases=releases)
