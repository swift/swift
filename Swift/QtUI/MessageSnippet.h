#pragma once

#include <QString>

#include "ChatSnippet.h"

class QDateTime;

namespace Swift {
	class MessageSnippet : public ChatSnippet {
		public:
			MessageSnippet(const QString& message, const QString& sender, const QDateTime& time, const QString& iconURI, bool isIncoming, bool appendToPrevious);

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
