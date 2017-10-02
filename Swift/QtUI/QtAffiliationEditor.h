/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <vector>

#include <QDialog>

#include <Swiften/Elements/MUCOccupant.h>
#include <Swiften/JID/JID.h>

#include <Swift/QtUI/ui_QtAffiliationEditor.h>

namespace Swift {
    class QtAffiliationEditor : public QDialog {
        Q_OBJECT
        public:
            QtAffiliationEditor(QWidget* parent = nullptr);
            ~QtAffiliationEditor();
            void setAffiliations(MUCOccupant::Affiliation, const std::vector<JID>& jids);
            const std::vector<std::pair<MUCOccupant::Affiliation, JID> >& getChanges() const;
        private slots:
            void handleCurrentIndexChanged(int);
            void handleAddClicked();
            void handleRemoveClicked();
        private:
            typedef std::pair<MUCOccupant::Affiliation, JID> ChangePair;
            MUCOccupant::Affiliation affiliationFromIndex(int affiliation);
            Ui::QtAffiliationEditor ui_;
            std::map<MUCOccupant::Affiliation, std::vector<JID> > affiliations_;
            std::vector<ChangePair> changes_;
    };
}
