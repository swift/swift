#include "SystemMessageSnippet.h"

#include <QDateTime>

namespace Swift {

SystemMessageSnippet::SystemMessageSnippet(const QString& message, const QDateTime& time, bool appendToPrevious) : ChatSnippet(appendToPrevious) {
	content_ = loadTemplate(":/themes/Default/Status.html");

	content_.replace("%message%", escape(message));
	content_.replace("%shortTime%", escape(time.toString("h:mm")));
	content_.replace("%time%", escape(time.toString("h:mm")));
}

}
