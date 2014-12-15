/*
 * Copyright (c) 2010 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QString>

#include "ChatSnippet.h"

class QDateTime;

namespace Swift {
	class SystemMessageSnippet : public ChatSnippet {
		public:
			SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme, Direction direction);
			virtual ~SystemMessageSnippet();

			const QString& getContent() const {return content_;}

			/*QString getContinuationElementID() const {
				return "insert";
			};*/

		private:
			QString content_;
	};
}
