/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/ChatMessageSummarizer.h>

#include <Swiften/Base/format.h>
#include <Swift/Controllers/Intl.h>
#include <Swiften/Base/foreach.h>

using namespace Swift;
using namespace std;

string ChatMessageSummarizer::getSummary(const string& current, const vector<UnreadPair>& unreads) {
	vector<UnreadPair> others;
	int currentUnread = 0;
	int otherCount = 0;
	foreach (UnreadPair unread, unreads) {
		if (unread.first == current) {
			currentUnread += unread.second;
		} else {
			if (unread.second > 0) {
				otherCount += unread.second;
				others.push_back(unread);
			}
		}
	}
	string myString(current);

	if (currentUnread > 0) {
		string result(QT_TRANSLATE_NOOP("", "%1% (%2%)"));
		myString = str(format(result) % current % currentUnread);
	}

	if (others.size() > 1) {
		string result(QT_TRANSLATE_NOOP("", "%1% and %2% others (%3%)"));
		myString = str(format(result) % myString % others.size() % otherCount);
	} else if (!others.empty()) {
		string result(QT_TRANSLATE_NOOP("", "%1%, %2% (%3%)"));
		myString = str(format(result) % myString % others[0].first % otherCount);
	}
	return myString;
}
