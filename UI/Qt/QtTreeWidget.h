#ifndef SWIFT_QtTreeWidget_H
#define SWIFT_QtTreeWidget_H

#include <QTreeWidget>

#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "UI/Qt/QtTreeWidgetItem.h"
#include "UI/Qt/QtTreeWidget.h"

namespace Swift {

class QtTreeWidget : public QTreeWidget, public TreeWidget {
	Q_OBJECT
	public:
		QtTreeWidget(QWidget* parent = 0);
	
	private slots:
		void handleItemActivated(QTreeWidgetItem*, int);

  private:
    void drawBranches(QPainter*, const QRect&, const QModelIndex&) const;
};

}
#endif

