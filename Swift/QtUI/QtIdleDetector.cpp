#include "QtIdleDetector.h"

#include <QCursor>

namespace Swift {

QtIdleDetector::QtIdleDetector() {
	signalled_ = false;
	lastMoveTime_ = QDateTime::currentDateTime();
	lastPosition_ = QCursor::pos();
	connect(&timer_, SIGNAL(timeout()), this, SLOT(handleTimerTick()));
	timer_.start(1000);
}

QtIdleDetector::~QtIdleDetector() {
}

void QtIdleDetector::forceNotIdle() {

}

void QtIdleDetector::handleTimerTick() {
	QPoint position = QCursor::pos();
	QDateTime now = QDateTime::currentDateTime();
	if (position != lastPosition_) {
		if (signalled_) {
			onInputNotIdle();
		}
		lastPosition_ = position;
		lastMoveTime_ = now;
		signalled_ = false;
	} else if (!signalled_ && now > lastMoveTime_.addSecs(600) && position == lastPosition_) {
		signalled_ = true;
		onInputIdle();
	}
}

}
