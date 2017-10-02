/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtAffiliationEditor.h>

#include <QInputDialog>
#include <QListWidgetItem>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
QtAffiliationEditor::QtAffiliationEditor(QWidget* parent) : QDialog(parent){
    ui_.setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui_.affiliation, SIGNAL(currentIndexChanged(int)), this, SLOT(handleCurrentIndexChanged(int)));
    connect(ui_.addJID, SIGNAL(clicked()), this, SLOT(handleAddClicked()));
    connect(ui_.removeJID, SIGNAL(clicked()), this, SLOT(handleRemoveClicked()));
}

QtAffiliationEditor::~QtAffiliationEditor() {

}

void QtAffiliationEditor::setAffiliations(MUCOccupant::Affiliation affiliation, const std::vector<JID>& jids) {
    affiliations_[affiliation] = jids;
    if (affiliationFromIndex(ui_.affiliation->currentIndex()) == affiliation) {
        handleCurrentIndexChanged(ui_.affiliation->currentIndex());
    }
}

const std::vector<std::pair<MUCOccupant::Affiliation, JID> >& QtAffiliationEditor::getChanges() const {
    return changes_;
}

void QtAffiliationEditor::handleCurrentIndexChanged(int index) {
    ui_.list->clear();
    for (const auto& jid : affiliations_[affiliationFromIndex(index)]) {
        ui_.list->addItem(P2QSTRING(jid.toString()));
    }
}

void QtAffiliationEditor::handleAddClicked() {
    bool ok = false;
    JID jid = JID(Q2PSTRING(QInputDialog::getText(this, tr("Add User"), tr("Added User's Address:"), QLineEdit::Normal, "", &ok)));
    if (ok && jid.isValid()) {
        //FIXME: validation
        MUCOccupant::Affiliation affiliation = affiliationFromIndex(ui_.affiliation->currentIndex());
        changes_.push_back(ChangePair(affiliation, jid));
        ui_.list->addItem(P2QSTRING(jid.toString()));
        affiliations_[affiliation].push_back(jid);
    }
}

void QtAffiliationEditor::handleRemoveClicked() {
    QListWidgetItem* item = ui_.list->currentItem();
    if (item) {
        JID jid(Q2PSTRING(item->text()));
        changes_.push_back(ChangePair(MUCOccupant::NoAffiliation, jid));
        std::vector<JID>& jids = affiliations_[affiliationFromIndex(ui_.affiliation->currentIndex())];
        jids.erase(std::remove(jids.begin(), jids.end(), jid), jids.end());
        handleCurrentIndexChanged(ui_.affiliation->currentIndex());
    }
}

MUCOccupant::Affiliation QtAffiliationEditor::affiliationFromIndex(int affiliation) {
    switch (affiliation) {
        case 0: return MUCOccupant::Owner;
        case 1: return MUCOccupant::Admin;
        case 2: return MUCOccupant::Member;
        case 3: return MUCOccupant::Outcast;
        default: return MUCOccupant::Outcast;
    }
}

}
