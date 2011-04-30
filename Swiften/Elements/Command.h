/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Form.h>

namespace Swift {
	/**
	 * Ad-Hoc Command (XEP-0050).
	 */
	class Command : public Payload {
		public:
			typedef boost::shared_ptr<Command> ref;

			enum Status {Executing, Completed, Canceled, NoStatus};
			enum Action {Cancel, Execute, Complete, Prev, Next, NoAction};

			struct Note {
				enum Type {Info, Warn, Error};

				Note(std::string note, Type type) : note(note), type(type) {};

				std::string note;
				Type type;
			};

		public:
			Command(const std::string& node, const std::string& sessionID, Status status) { constructor(node, sessionID, NoAction, status);}
			Command(const std::string& node = "", const std::string& sessionID = "", Action action = Execute) { constructor(node, sessionID, action, NoStatus); }

			const std::string& getNode() const { return node_; }
			void setNode(const std::string& node) { node_ = node; }

			const std::string& getSessionID() const { return sessionID_; }
			void setSessionID(const std::string& id) { sessionID_ = id; }

			Action getAction() const { return action_; }
			void setAction(Action action) { action_ = action; }

			void setExecuteAction(Action action) { executeAction_ = action; }
			Action getExecuteAction() const { return executeAction_; }

			Status getStatus() const { return status_; }
			void setStatus(Status status) { status_ = status; }

			void addAvailableAction(Action action) { availableActions_.push_back(action); }
			const std::vector<Action>& getAvailableActions() const { return availableActions_; }
			void addNote(const Note& note) { notes_.push_back(note); }
			const std::vector<Note>& getNotes() const { return notes_; }

			Form::ref getForm() const { return form_; }
			void setForm(Form::ref payload) { form_ = payload; }

		private:
			void constructor(const std::string& node, const std::string& sessionID, Action action, Status status) {
				node_ = node;
				sessionID_ = sessionID;
				action_ = action;
				status_ = status;
				executeAction_ = NoAction;
			}

		private:
			std::string node_;
			std::string sessionID_;
			Action action_;
			Status status_;
			Action executeAction_;
			std::vector<Action> availableActions_;
			std::vector<Note> notes_;
			Form::ref form_;
	};
}
