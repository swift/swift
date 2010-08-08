/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	/**
	 * Ad-Hoc Command (XEP-0050).
	 */
	class Command : public Payload {
		public:
			enum Status {Executing, Completed, Canceled, NoStatus};
			enum Action {Cancel, Execute, Complete, Prev, Next, NoAction};

			struct Note {
				enum Type {Info, Warn, Error};

				Note(String note, Type type) : note(note), type(type) {};

				String note;
				Type type;
			};

		public:
			Command(const String& node, const String& sessionID, Status status) { constructor(node, sessionID, NoAction, status);}
			Command(const String& node, const String& sessionID = "", Action action = Execute) { constructor(node, sessionID, action, NoStatus); }

			const String& getNode() const { return node_; }
			const String& getSessionID() const { return sessionID_; }
			Action getPerformedAction() const { return performedAction_; }
			void setExecuteAction(Action action) { executeAction_ = action; }
			Action getExecuteAction() const { return executeAction_; }
			Status getStatus() const { return status_; }
			void addAvailableAction(Action action) { availableActions_.push_back(action); }
			const std::vector<Action>& getAvailableActions() const { return availableActions_; }
			void addNote(const Note& note) { notes_.push_back(note); }
			const std::vector<Note>& getNotes() const { return notes_; }
			boost::shared_ptr<Payload> getPayload() const { return payload_; }
			void setPayload(boost::shared_ptr<Payload> payload) { payload_ = payload; }

		private:
			void constructor(const String& node, const String& sessionID, Action action, Status status) {
				node_ = node;
				sessionID_ = sessionID;
				performedAction_ = action;
				status_ = status;
				executeAction_ = NoAction;
			}

		private:
			String node_;
			String sessionID_;
			Action performedAction_;
			Status status_;
			Action executeAction_;
			std::vector<Action> availableActions_;
			std::vector<Note> notes_;
			boost::shared_ptr<Payload> payload_;
	};
}
