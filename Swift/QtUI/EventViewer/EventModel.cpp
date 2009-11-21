#include "EventModel.h"

namespace Swift {
EventModel::EventModel() {
	
}

EventModel::~EventModel() {
	
}

QVariant EventModel::data(const QModelIndex& /*index*/, int /*role*/) const {
	return QVariant();
}

int EventModel::rowCount(const QModelIndex& /*parent*/) const {
	return 0;
}

}
