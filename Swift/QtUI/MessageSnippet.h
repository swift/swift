/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QString>

#include "ChatSnippet.h"

class QDateTime;

namespace Swift {
	class MessageSnippet : public ChatSnippet {
		public:
			MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious, QtChatTheme* theme, const QString& id);
			virtual ~MessageSnippet();
			const QString& getContent() const {
				return content_;
			}

			QString getContinuationElementID() const {
				return "insert";
			};

		private:
			QString content_;
	};
}
