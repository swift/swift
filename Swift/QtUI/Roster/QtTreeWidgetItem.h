#ifndef SWIFT_QtTreeWidgetItem_H
#define SWIFT_QtTreeWidgetItem_H

#include <QColor>
#include <QVariant>

#include "Swiften/Base/String.h"
#include "Swiften/Roster/TreeWidgetFactory.h"
#include "Swiften/Roster/TreeWidget.h"
#include "Swiften/Roster/TreeWidgetItem.h"
#include "Swift/QtUI/Roster/QtTreeWidgetItem.h"


#include "Swift/QtUI/QtSwiftUtil.h"


namespace Swift {
	enum RosterRoles {
		StatusTextRole = Qt::UserRole,
		AvatarRole = Qt::UserRole + 1,
		PresenceIconRole = Qt::UserRole + 2,
		StatusShowTypeRole = Qt::UserRole + 3
	};
	
class QtTreeWidget;
class QtTreeWidgetItem : public QObject, public TreeWidgetItem {
	Q_OBJECT
	public:
			~QtTreeWidgetItem();
			void addChild(QtTreeWidgetItem* child);
			void removeChild(QtTreeWidgetItem* child);
			QtTreeWidgetItem* getParentItem();
			int rowCount();
			int rowOf(QtTreeWidgetItem* item);
			int row();
			QtTreeWidgetItem* getItem(int row);
			QVariant data(int role);
			QIcon getPresenceIcon(); 
			QtTreeWidgetItem(QtTreeWidgetItem* parentItem);
			void setText(const String& text);
			void setAvatarPath(const String& path);
			void setStatusText(const String& text);
			void setStatusShow(StatusShow::Type show);
			void setTextColor(unsigned long color);
			void setBackgroundColor(unsigned long color);
			void setExpanded(bool b);
			void parentItemHasBeenDeleted();
			void hide();
			void show();
			bool isShown();
			bool isContact() const;
			bool isExpanded();
			const QString& getName() const {return displayName_;};
			const QString& getLowerName() const {return displayNameLower_;};
			StatusShow::Type getStatusShow() const {return statusShowType_;};
			StatusShow::Type getStatusShowMerged() const {return mergedShowType_;};

			QWidget* getCollapsedRosterWidget();
			QWidget* getExpandedRosterWidget();
			bool operator<(const QtTreeWidgetItem& item) const;
			
		signals:
			void changed(QtTreeWidgetItem*);
		private slots:
			void handleChanged(QtTreeWidgetItem* item);
		private:
			QString toolTipString();
			QList<QtTreeWidgetItem*> children_;
			QList<QtTreeWidgetItem*> shownChildren_;
			QtTreeWidgetItem* parent_;
			QString displayName_;
			QString displayNameLower_;
			QString statusText_;
			QColor textColor_;
			QColor backgroundColor_;
			QVariant avatar_;
			bool shown_;
			bool expanded_;
			StatusShow::Type statusShowType_;
			StatusShow::Type mergedShowType_;
};

bool itemLessThan(QtTreeWidgetItem* left, QtTreeWidgetItem* right);

}
#endif

