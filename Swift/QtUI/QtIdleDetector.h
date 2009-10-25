#pragma once

#include "Swift/Controllers/IdleDetector.h"

#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QDateTime>

namespace Swift {
class QtIdleDetector : public QObject, public IdleDetector {
	Q_OBJECT
	public:
		QtIdleDetector();
		virtual ~QtIdleDetector();
		virtual void forceNotIdle();
	private slots:
		void handleTimerTick();
	private:
		QPoint lastPosition_;
		QDateTime lastMoveTime_;
		QTimer timer_;
		bool signalled_;
};
}
