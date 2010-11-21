/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <QString>
#include <QDateTime>
#include "QtChatTheme.h"

namespace Swift {
	class ChatSnippet {
		public:
			ChatSnippet(bool appendToPrevious);
			virtual ~ChatSnippet();
			
			virtual const QString& getContent() const = 0;
			virtual QString getContinuationElementID() const { return ""; }

			boost::shared_ptr<ChatSnippet> getContinuationFallbackSnippet() {return continuationFallback_;}

			bool getAppendToPrevious() const {
				return appendToPrevious_;
			}
			
			static QString escape(const QString&);

		protected:
			void setContinuationFallbackSnippet(boost::shared_ptr<ChatSnippet> continuationFallback) {
				continuationFallback_ = continuationFallback;
			}
			static QString timeToEscapedString(const QDateTime& time);
		private:
			bool appendToPrevious_;
			boost::shared_ptr<ChatSnippet> continuationFallback_;
	};
}

