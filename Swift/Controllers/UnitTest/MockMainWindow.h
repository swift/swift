#pragma once

#include "Swift/Controllers/MainWindow.h"
#include "Swiften/Roster/TreeWidget.h"

namespace Swift {
	class MockMainWindow : public MainWindow {
		public:
			MockMainWindow(TreeWidget* treeWidget) {treeWidget_ = treeWidget;};
			virtual ~MockMainWindow() {};
			virtual TreeWidget* getTreeWidget() {return treeWidget_;};
			virtual void setMyName(const String& /*name*/) {};;
			virtual void setMyAvatarPath(const String& /*path*/) {};
			virtual void setMyStatusText(const String& /*status*/) {};
			virtual void setMyStatusType(StatusShow::Type /*type*/) {};
			
		private:
			TreeWidget* treeWidget_;
	};
}
