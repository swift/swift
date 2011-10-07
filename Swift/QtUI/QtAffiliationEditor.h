/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <vector>
#include <map>

#include <QDialog>
#include <Swift/QtUI/ui_QtAffiliationEditor.h>

#include <Swiften/JID/JID.h>
#include <Swiften/Elements/MUCOccupant.h>

namespace Swift {
	class QtAffiliationEditor : public QDialog {
		Q_OBJECT
		public:
			QtAffiliationEditor(QWidget* parent = NULL);
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