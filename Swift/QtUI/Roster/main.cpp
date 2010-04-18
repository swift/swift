/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QtGui>
#include "QtTreeWidget.h"
#include "QtTreeWidgetFactory.h"

#include "Swiften/Elements/StatusShow.h"

int main(int argc, char *argv[])
{
		QApplication app(argc, argv);

		//Swift::RosterModel model;

		//QTreeView view;
		//view.setModel(&model);
		//view.setWindowTitle("A roster");
		//view.show();

	Swift::QtTreeWidgetFactory treeWidgetFactory;
	Swift::QtTreeWidget* tree = dynamic_cast<Swift::QtTreeWidget*>(treeWidgetFactory.createTreeWidget());
	tree->show();
	QList<Swift::QtTreeWidgetItem*> item3s;
	for (int i = 0; i < 500; i++) {
		Swift::QtTreeWidgetItem* group = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(tree));
		group->setText("People");
		group->setBackgroundColor(0xBBBBBB);
		Swift::QtTreeWidgetItem* item1 = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(group));
		Swift::QtTreeWidgetItem* item2 = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(group));
		Swift::QtTreeWidgetItem* item3 = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(group));
		Swift::QtTreeWidgetItem* item4 = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(group));
		item1->setText("Remko");
		item2->setText("Kevin");
		item3->setText("Cath");
		item4->setText("KimTypo");
		item4->setText("Kim");
		item3s.push_back(item3);
	}
	
	Swift::QtTreeWidgetItem* group = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(tree));
	group->setText("Many People");
	
	Swift::QtTreeWidgetItem* person350;
	Swift::QtTreeWidgetItem* person1200;

	for (int i = 0; i < 1500; i++) {
		Swift::QtTreeWidgetItem* item = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(group));
		item->setText(Q2PSTRING(QString("Some person %1").arg(i)));
		item->setStatusShow(Swift::StatusShow::Away);
		if (i == 350) person350 = item;
		if (i == 1200) person1200 = item;
	}	

	for (int i = 0; i < item3s.size(); i++) {
		item3s[i]->setStatusShow(Swift::StatusShow::XA);
	}

	person350->setStatusShow(Swift::StatusShow::DND);
	person1200->setStatusShow(Swift::StatusShow::Online);

	return app.exec();
}
