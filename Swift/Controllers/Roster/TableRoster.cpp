/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Roster/TableRoster.h>

#include <boost/cast.hpp>

#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>

using namespace Swift;

TableRoster::TableRoster(Roster* model) : model(model) {
}
			
size_t TableRoster::getNumberOfSections() const {
	return model ? model->getRoot()->getDisplayedChildren().size() : 0;
}

size_t TableRoster::getNumberOfRowsInSection(size_t section) const {
	return boost::polymorphic_downcast<Swift::GroupRosterItem*>(model->getRoot()->getDisplayedChildren()[section])->getDisplayedChildren().size();
}

const std::string& TableRoster::getSectionTitle(size_t section) {
	return model->getRoot()->getDisplayedChildren()[section]->getDisplayName();
}

TableRoster::Item TableRoster::getItem(const Index& index) const {
	Item item;
	item.name = boost::polymorphic_downcast<Swift::GroupRosterItem*>(model->getRoot()->getDisplayedChildren().at(index.section))->getDisplayedChildren().at(index.row)->getDisplayName();
	item.description = "My Status";
	return item;
}
