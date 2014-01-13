/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
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
	HighlightRulesListPtr rules = manager_->getRules();
	for (size_t i = 0; i < rules->getSize(); ++i) {
		const HighlightRule& rule = rules->getRule(i);
		if (rule.isMatch(body, sender, nick_, messageType_)) {
			return rule.getAction();
		}
	}

	return HighlightAction();
}

void Highlighter::handleHighlightAction(const HighlightAction& action)
{
	manager_->onHighlight(action);
}

}
