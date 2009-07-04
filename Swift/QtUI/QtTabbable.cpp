#include "QtTabbable.h"

#include <QApplication>

namespace Swift {

bool QtTabbable::isWidgetSelected() {
	/*isActiveWindow() shouldn't be necessary, but I don't trust it as far as I can throw it*/
	return isActiveWindow() && isAncestorOf(QApplication::focusWidget());
}

}
