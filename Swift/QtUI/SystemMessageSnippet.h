#pragma once

#include <QString>

#include "ChatSnippet.h"

class QDateTime;

namespace Swift {
	class SystemMessageSnippet : public ChatSnippet {
		public:
			SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious);

			const QString& getContent() const {
				return content_;
			}

			/*QString getContinuationElementID() const {
				return "insert";
			};*/

		private:
			QString content_;
	};
}
