/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/Base/foreach.h>
#include <Swift/Controllers/Highlighter.h>
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

HighlightAction Highlighter::findAction(const std::string& body, const std::string& sender) const
{
	foreach (const HighlightRule & r, manager_->getRules()) {
		if (r.isMatch(body, sender, nick_, messageType_)) {
			return r.getAction();
		}
	}

	return HighlightAction();
}

void Highlighter::handleHighlightAction(const HighlightAction& action)
{
	manager_->onHighlight(action);
}

}
