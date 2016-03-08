/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/Highlighter.h>

#include <Swiften/Base/foreach.h>

#include <Swift/Controllers/HighlightManager.h>

namespace Swift {

Highlighter::Highlighter(HighlightManager* manager)
	: manager_(manager)
{
	setMode(ChatMode);
}

void Highlighter::setMode(Mode mode)
{
	mode_ = mode;
	messageType_ = mode_ == ChatMode ? HighlightRule::ChatMessage : HighlightRule::MUCMessage;
}

HighlightAction Highlighter::findFirstFullMessageMatchAction(const std::string& body, const std::string& sender) const
{
	HighlightAction match;
	HighlightRulesListPtr rules = manager_->getRules();
	for (size_t i = 0; i < rules->getSize(); ++i) {
		const HighlightRule& rule = rules->getRule(i);
		if (rule.isMatch(body, sender, nick_, messageType_) && rule.getAction().highlightWholeMessage()) {
			match = rule.getAction();
			break;
		}
	}

	return match;
}

void Highlighter::handleHighlightAction(const HighlightAction& action)
{
	manager_->onHighlight(action);
}

}
