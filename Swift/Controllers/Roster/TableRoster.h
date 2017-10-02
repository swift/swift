/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/signals2.hpp>

#include <Swiften/Elements/StatusShow.h>
#include <Swiften/JID/JID.h>

namespace Swift {
    class Roster;
    class TimerFactory;
    class Timer;

    class TableRoster {
        public:
            struct Item {
                Item(const std::string& name, const std::string& description, const JID& jid, StatusShow::Type status, const boost::filesystem::path& avatarPath) : name(name), description(description), jid(jid), status(status), avatarPath(avatarPath) {
                }
                std::string name;
                std::string description;
                JID jid;
                StatusShow::Type status;
                boost::filesystem::path avatarPath;
            };

            struct Index {
                Index(size_t section = 0, size_t row = 0) : section(section), row(row) {
                }
                size_t section;
                size_t row;

                bool operator==(const Index& o) const {
                    return o.section == section && o.row == row;
                }
            };

            struct Update {
                std::vector<Index> updatedRows;
                std::vector<Index> insertedRows;
                std::vector<Index> deletedRows;
                std::vector<size_t> insertedSections;
                std::vector<size_t> deletedSections;
            };

            TableRoster(Roster* model, TimerFactory* timerFactory, int updateDelay);
            ~TableRoster();

            size_t getNumberOfSections() const;
            size_t getNumberOfRowsInSection(size_t section) const;

            const std::string& getSectionTitle(size_t);

            const Item& getItem(const Index&) const;

            boost::signals2::signal<void (const Update&)> onUpdate;

        private:
            void handleUpdateTimerTick();
            void scheduleUpdate();

        private:
            friend struct SectionNameEquals;
            struct Section {
                Section(const std::string& name) : name(name) {
                }

                std::string name;
                std::vector<Item> items;
            };

            Roster* model;
            std::vector<Section> sections;
            bool updatePending;
            std::shared_ptr<Timer> updateTimer;
    };
}
