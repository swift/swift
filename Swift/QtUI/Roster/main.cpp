/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QtGui>
#include "QtTreeWidget.h"
#include "QtTreeWidgetFactory.h"

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
	}
	
	Swift::QtTreeWidgetItem* group = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(tree));
	group->setText("Many People");
	
	for (int i = 0; i < 1500; i++) {
		Swift::QtTreeWidgetItem* item = dynamic_cast<Swift::QtTreeWidgetItem*>(treeWidgetFactory.createTreeWidgetItem(group));
		item->setText(Q2PSTRING(QString("Some person %1").arg(i)));

	}	

	return app.exec();
}
