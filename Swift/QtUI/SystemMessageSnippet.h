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
	class SystemMessageSnippet : public ChatSnippet {
		public:
			SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious, QtChatTheme* theme);
			virtual ~SystemMessageSnippet();

			const QString& getContent() const {return content_;}

			/*QString getContinuationElementID() const {
				return "insert";
			};*/

		private:
			QString content_;
	};
}
