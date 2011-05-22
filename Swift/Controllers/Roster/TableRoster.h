/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>
#include <Swiften/Base/boost_bsignals.h>

#include <Swiften/JID/JID.h>

namespace Swift {
	class Roster;

	class TableRoster {
		public:
			struct Item {
				std::string name;
				std::string description;
				JID jid;
			};
			struct Index {
				Index(size_t section, size_t row) : section(section), row(row) { 
				}
				size_t section;
				size_t row;
			};

			TableRoster(Roster* model);
			
			size_t getNumberOfSections() const;
			size_t getNumberOfRowsInSection(size_t section) const;

			const std::string& getSectionTitle(size_t);

			Item getItem(const Index&) const;

			boost::signal<void ()> onBeginUpdates;
			boost::signal<void (const std::vector<Index>&)> onRowsInserted;
			boost::signal<void (const std::vector<Index>&)> onRowsDeleted;
			boost::signal<void (const std::vector<size_t>&)> onSectionsInserted;
			boost::signal<void (const std::vector<size_t>&)> onSectionsDeleted;
			boost::signal<void ()> onEndUpdates;

		private:
			Roster* model;
	};
}
