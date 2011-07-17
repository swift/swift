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
				return i1.status != i2.status || i1.description != i2.description || i1.name != i2.name || i1.avatarPath.empty() != i2.avatarPath.empty();
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

TableRoster::TableRoster(Roster* model, TimerFactory* timerFactory, int updateDelay) : model(model), updatePending(false) {
	updateTimer = timerFactory->createTimer(updateDelay);
	updateTimer->onTick.connect(boost::bind(&TableRoster::handleUpdateTimerTick, this));
	if (model) {
		model->onChildrenChanged.connect(boost::bind(&TableRoster::scheduleUpdate, this));
		model->onGroupAdded.connect(boost::bind(&TableRoster::scheduleUpdate, this));
		model->onDataChanged.connect(boost::bind(&TableRoster::scheduleUpdate, this));
	}
}

TableRoster::~TableRoster() {
	updateTimer->stop();
	updateTimer->onTick.disconnect(boost::bind(&TableRoster::handleUpdateTimerTick, this));
	if (model) {
		model->onDataChanged.disconnect(boost::bind(&TableRoster::scheduleUpdate, this));
		model->onGroupAdded.disconnect(boost::bind(&TableRoster::scheduleUpdate, this));
		model->onChildrenChanged.disconnect(boost::bind(&TableRoster::scheduleUpdate, this));
	}
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

const TableRoster::Item& TableRoster::getItem(const Index& index) const {
	return sections[index.section].items[index.row];
}
			
void TableRoster::handleUpdateTimerTick() {
	updateTimer->stop();
	updatePending = false;

	// Get a model for the new roster
	std::vector<Section> newSections;
	if (model) {
		foreach(RosterItem* item, model->getRoot()->getDisplayedChildren()) {
			if (GroupRosterItem* groupItem = boost::polymorphic_downcast<GroupRosterItem*>(item)) {
				//std::cerr << "* " << groupItem->getDisplayName() << std::endl;
				Section section(groupItem->getDisplayName());
				foreach(RosterItem* groupChildItem, groupItem->getDisplayedChildren()) {
					if (ContactRosterItem* contact = boost::polymorphic_downcast<ContactRosterItem*>(groupChildItem)) {
						//std::cerr << "  - " << contact->getDisplayJID() << std::endl;
						section.items.push_back(Item(contact->getDisplayName(), contact->getStatusText(), contact->getDisplayJID(), contact->getStatusShow(), contact->getAvatarPath()));
					}
				}
				newSections.push_back(section);
			}
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
		size_t end = update.insertedRows.size();
		update.insertedRows.resize(update.insertedRows.size() + itemInserts.size());
		std::transform(itemInserts.begin(), itemInserts.end(), update.insertedRows.begin() + end, CreateIndexForSection(sectionPostUpdates[i]));
		end = update.deletedRows.size();
		update.deletedRows.resize(update.deletedRows.size() + itemRemoves.size());
		std::transform(itemRemoves.begin(), itemRemoves.end(), update.deletedRows.begin() + end, CreateIndexForSection(sectionUpdates[i]));
		end = update.updatedRows.size();
		update.updatedRows.resize(update.updatedRows.size() + itemUpdates.size());
		std::transform(itemUpdates.begin(), itemUpdates.end(), update.updatedRows.begin() + end, CreateIndexForSection(sectionPostUpdates[i]));
	}
	
	// Switch the old model with the new
	sections.swap(newSections);

	/*
	std::cerr << "-S: ";
	for (size_t i = 0; i < update.deletedSections.size(); ++i) {
		std::cerr << update.deletedSections[i] << " ";
	}
	std::cerr << std::endl;
	std::cerr << "+S: ";
	for (size_t i = 0; i < update.insertedSections.size(); ++i) {
		std::cerr << update.insertedSections[i] << " ";
	}
	std::cerr << std::endl;
	std::cerr << "-R: ";
	for (size_t i = 0; i < update.deletedRows.size(); ++i) {
		std::cerr << update.deletedRows[i].section << "," << update.deletedRows[i].row << " ";
	}
	std::cerr << std::endl;
	std::cerr << "*R: ";
	for (size_t i = 0; i < update.updatedRows.size(); ++i) {
		std::cerr << update.updatedRows[i].section << "," << update.updatedRows[i].row << " ";
	}
	std::cerr << std::endl;
	std::cerr << "+R: ";
	for (size_t i = 0; i < update.insertedRows.size(); ++i) {
		std::cerr << update.insertedRows[i].section << "," << update.insertedRows[i].row << " ";
	}
	std::cerr << std::endl;
	*/

	// Emit the update
	onUpdate(update);
}

void TableRoster::scheduleUpdate() {
	if (!updatePending) {
		updatePending = true;
		updateTimer->start();
	}
}
