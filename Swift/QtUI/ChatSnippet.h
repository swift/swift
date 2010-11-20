/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QString>
#include <QDateTime>
#include "QtChatTheme.h"

namespace Swift {
	class ChatSnippet {
		public:
			ChatSnippet(bool appendToPrevious = false);
			virtual ~ChatSnippet();
			
			virtual const QString& getContent() const = 0;
			virtual QString getContinuationElementID() const { return ""; }

			bool getAppendToPrevious() const {
				return appendToPrevious_;
			}
			
			static QString escape(const QString&);

		protected:
			static QString timeToEscapedString(const QDateTime& time);
		private:
			bool appendToPrevious_;
	};
}

