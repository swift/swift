/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swift/Controllers/Roster/TableRoster.h>

#include <boost/cast.hpp>
#include <cassert>
#include <algorithm>
#include <Swiften/Base/foreach.h>

#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/Timer.h>
#include <Swift/Controllers/Roster/Roster.h>
#include <Swift/Controllers/Roster/GroupRosterItem.h>
#include <Swift/Controllers/Roster/LeastCommonSubsequence.h>

namespace Swift {
	struct SectionNameEquals {
		bool operator()(const TableRoster::Section& s1, const TableRoster::Section& s2) const {
			return s1.name == s2.name;
		}
	};

	template<typename T>
	struct True {
		bool operator()(const T&, const T&) const {
			return true;
		}
	};

	struct ItemEquals {
			bool operator()(const TableRoster::Item& i1, const TableRoster::Item& i2) const {
				return i1.jid == i2.jid;
			}
	};


	struct ItemNeedsUpdate {
			bool operator()(const TableRoster::Item& i1, const TableRoster::Item& i2) const {
				return i1.description != i2.description || i1.name != i2.name;
			}
	};

	struct CreateIndexForSection {
			CreateIndexForSection(size_t section) : section(section) {
			}

			TableRoster::Index operator()(size_t row) const {
				return TableRoster::Index(section, row);
			}

			size_t section;
	};
}

using namespace Swift;

TableRoster::TableRoster(Roster* model, TimerFactory* timerFactory, int updateDelay) : model(model) {
	updateTimer = timerFactory->createTimer(updateDelay);
	model->onChildrenChanged.connect(boost::bind(&TableRoster::scheduleUpdate, this));
	model->onGroupAdded.connect(boost::bind(&TableRoster::scheduleUpdate, this));
	model->onDataChanged.connect(boost::bind(&TableRoster::scheduleUpdate, this));
}

TableRoster::~TableRoster() {
	model->onDataChanged.disconnect(boost::bind(&TableRoster::scheduleUpdate, this));
	model->onGroupAdded.disconnect(boost::bind(&TableRoster::scheduleUpdate, this));
	model->onChildrenChanged.disconnect(boost::bind(&TableRoster::scheduleUpdate, this));
	updateTimer->stop();
}
			
size_t TableRoster::getNumberOfSections() const {
	return sections.size();
}

const std::string& TableRoster::getSectionTitle(size_t section) {
	return sections[section].name;
}

size_t TableRoster::getNumberOfRowsInSection(size_t section) const {
	return sections[section].items.size();
}

TableRoster::Item TableRoster::getItem(const Index& index) const {
	return sections[index.section].items[index.row];
}
			
void TableRoster::handleUpdateTimerTick() {
	updateTimer->stop();
	updatePending = false;

	// Get a model for the new roster
	std::vector<Section> newSections;
	foreach(RosterItem* item, model->getRoot()->getDisplayedChildren()) {
		if (GroupRosterItem* groupItem = boost::polymorphic_downcast<GroupRosterItem*>(item)) {
			Section section(groupItem->getDisplayName());
			foreach(RosterItem* groupChildItem, groupItem->getDisplayedChildren()) {
				if (ContactRosterItem* contact = boost::polymorphic_downcast<ContactRosterItem*>(groupChildItem)) {
					section.items.push_back(Item(contact->getDisplayName(), contact->getStatusText(), contact->getDisplayJID()));
				}
			}
			newSections.push_back(section);
		}
	}

	// Do a diff with the previous roster
	Update update;
	std::vector<size_t> sectionUpdates;
	std::vector<size_t> sectionPostUpdates;
	computeIndexDiff<Section,SectionNameEquals,True<Section> >(sections, newSections, sectionUpdates, sectionPostUpdates, update.deletedSections, update.insertedSections);
	assert(sectionUpdates.size() == sectionPostUpdates.size());
	for (size_t i = 0; i < sectionUpdates.size(); ++i) {
		assert(sectionUpdates[i] < sections.size());
		assert(sectionPostUpdates[i] < newSections.size());
		std::vector<size_t> itemUpdates;
		std::vector<size_t> itemPostUpdates;
		std::vector<size_t> itemRemoves;
		std::vector<size_t> itemInserts;
		computeIndexDiff<Item, ItemEquals, ItemNeedsUpdate >(sections[sectionUpdates[i]].items, newSections[sectionPostUpdates[i]].items, itemUpdates, itemPostUpdates, itemRemoves, itemInserts);
		update.insertedRows.resize(itemInserts.size());
		std::transform(itemInserts.begin(), itemInserts.end(), update.insertedRows.begin(), CreateIndexForSection(sectionPostUpdates[i]));
		update.deletedRows.resize(itemRemoves.size());
		std::transform(itemRemoves.begin(), itemRemoves.end(), update.deletedRows.begin(), CreateIndexForSection(sectionPostUpdates[i]));
		update.updatedRows.resize(itemUpdates.size());
		std::transform(itemUpdates.begin(), itemUpdates.end(), update.updatedRows.begin(), CreateIndexForSection(sectionPostUpdates[i]));
	}
	
	// Switch the old model with the new
	sections.swap(newSections);

	// Emit the update
	onUpdate(update);
}

void TableRoster::scheduleUpdate() {
	if (!updatePending) {
		updatePending = true;
		updateTimer->start();
	}
}
