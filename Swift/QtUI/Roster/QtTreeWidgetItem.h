#ifndef SWIFT_QtTreeWidgetItem_H
#define SWIFT_QtTreeWidgetItem_H

#include <QColor>

#include "Swiften/Base/String.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"


#include "Swift/QtUI/QtSwiftUtil.h"


namespace Swift {
	enum RosterRoles {
		StatusTextRole = Qt::UserRole,
		AvatarRole = Qt::UserRole + 1
	};
	
class QtTreeWidget;
class QtTreeWidgetItem : public QObject, public TreeWidgetItem {
	Q_OBJECT
	public:
			~QtTreeWidgetItem();
			void addChild(QtTreeWidgetItem* child);
			QtTreeWidgetItem* getParentItem();
			int rowCount();
			int rowOf(QtTreeWidgetItem* item);
			int row();
			QtTreeWidgetItem* getItem(int row);
			QVariant data(int role);
			QtTreeWidgetItem(QtTreeWidgetItem* parentItem);
			void setText(const String& text);
			void setStatusText(const String& text);
			void setTextColor(unsigned long color);
			void setBackgroundColor(unsigned long color);
			void setExpanded(bool b);
			void hide();
			void show();
			bool isShown();
			bool isContact();

			QWidget* getCollapsedRosterWidget();
			QWidget* getExpandedRosterWidget();
		
		signals:
			void changed();
		private slots:
			void handleChanged();
		private:
			QList<QtTreeWidgetItem*> children_;
			QList<QtTreeWidgetItem*> shownChildren_;
			QtTreeWidgetItem* parent_;
			QString displayName_;
			QString statusText_;
			QColor textColor_;
			QColor backgroundColor_;
			bool shown_;
};

}
#endif

