/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiftob/Commands.h>

#include <Swiften/Base/foreach.h>
#include <iostream>
#include <boost/bind.hpp>

#include <Swiften/Client/Client.h>

typedef std::pair<std::string, Commands::Command*> NamedCommand;

Commands::Commands(Users* users, Swift::Client* client, Storage* storage, MUCs* mucs) {
	users_ = users;
	client_ = client;
	mucs_ = mucs;
	storage_ = storage;
	resetCommands();
}

void Commands::resetCommands() {
	foreach (NamedCommand command, commands_) {
		delete command.second;
	}
	commands_.clear();
	registerCommand("quit", Owner, "Quit the bot", boost::bind(&Commands::handleQuitCommand, this, _1, _2, _3));
	registerCommand("help", Anyone, "Get help", boost::bind(&Commands::handleHelpCommand, this, _1, _2, _3));
	registerCommand("join", Owner, "Join a MUC", boost::bind(&Commands::handleJoinCommand, this, _1, _2, _3));
	registerCommand("part", Owner, "Leave a MUC", boost::bind(&Commands::handlePartCommand, this, _1, _2, _3));
	registerCommand("rehash", Owner, "Reload scripts", boost::bind(&Commands::handleRehashCommand, this, _1, _2, _3));
	onReset();
}

void Commands::registerCommand(const std::string& name, RoleList roles, const std::string& description, boost::function<void(const std::string& /*command*/, const std::string& /*params*/, Swift::Message::ref)> callback) {
	Command* command = new Command(roles, description);
	commands_[name] = command;
	command->onReceived.connect(callback);
}

bool Commands::hasCommand(const std::string& name) {
	return commands_.find(name) != commands_.end();
}

bool Commands::runCommand(const std::string& name, const std::string& params, Swift::Message::ref message) {
	Users::User::Role userRole = users_->getRoleForSender(message);
	Command* command = commands_[name];
	if (roleIn(userRole, command->getAllowedBy())) {
		command->onReceived(name, params, message);
		return true;
	} else {
		replyTo(message, "You may not run this command", true);
	}
	return false;
}

bool Commands::roleIn(const Users::User::Role userRole, RoleList roleList) {
	switch (roleList) {
		case Owner : return userRole == Users::User::Owner;
		case Anyone : return true;
	}
	std::cerr << "Unrecognised role list" << std::endl;
	return false;
}

void Commands::handleQuitCommand(const std::string& /*command*/, const std::string& /*params*/, Swift::Message::ref message) {
	replyTo(message, "Shutting down");
	std::cout << "Quitting at the behest of " << message->getFrom().toString() << std::endl;
	exit(0);
}

void Commands::setRehashError(const std::string& error) {
	if (!rehashError_.empty()) {
		rehashError_ += "; ";
	}
	rehashError_ += error;
}

void Commands::handleRehashCommand(const std::string& /*command*/, const std::string& /*params*/, Swift::Message::ref message) {
	rehashError_ = "";
	replyTo(message, "Rehashing now.");
	std::cout << "Rehashing at the behest of " << message->getFrom().toString() << std::endl;
	resetCommands();
	if (rehashError_.empty()) {
		replyTo(message, "Rehash complete");
	} else {
		replyTo(message, "I have suffered a tremendous failure: " + rehashError_);
	}
}

void Commands::handleJoinCommand(const std::string& /*command*/, const std::string& params, Swift::Message::ref message) {
	Swift::JID room(params);
	if (!room.isValid() || !room.getResource().empty() || room.getNode().empty()) {
		replyTo(message, "Can't join " + room.toString() + ", not a valid room JID.");
		return;
	}
	if (mucs_->contains(room)) {
		replyTo(message, "I'm already (trying to be?) in " + room.toString() + ".");
		return;
	}
	replyTo(message, "Trying to join " + room.toString() + ".");
	mucs_->join(room, boost::bind(&Commands::handleJoinCommandSuccess, this, room, message), boost::bind(&Commands::handleJoinCommandFailure, this, room, _1, message));
}

void Commands::handlePartCommand(const std::string& /*command*/, const std::string& params, Swift::Message::ref message) {
	Swift::JID room(params);
	if (!room.isValid() || !room.getResource().empty() || room.getNode().empty()) {
		replyTo(message, "Can't leave " + room.toString() + ", not a valid room JID.");
		return;
	}
	if (mucs_->contains(room)) {
		replyTo(message, "I'm not in " + room.toString() + ".");
		return;
	}
	replyTo(message, "Leaving " + room.toString() + ".");
	mucs_->part(room);
}

void Commands::handleJoinCommandSuccess(const Swift::JID& room, Swift::Message::ref message) {
	replyTo(message, "Joined " + room.toString());
}

void Commands::handleJoinCommandFailure(const Swift::JID& room, const std::string& error, Swift::Message::ref message) {
	replyTo(message, "Join to " + room.toString() + "failed. " + error);
}

void Commands::handleHelpCommand(const std::string& /*command*/, const std::string& /*params*/, Swift::Message::ref message) {
	Users::User::Role userRole = users_->getRoleForSender(message);
	std::string result("Available commands:");
	std::string bang = message->getType() == Swift::Message::Groupchat ? "\n!" : "\n";
	foreach (NamedCommand pair, commands_) {
		if (roleIn(userRole, pair.second->getAllowedBy())) {
			result += bang + pair.first + " - " + pair.second->getDescription();
		}
	}
	replyTo(message, result, true);
}

/**
 * \param outOfMUC Reply to the sender directly, don't spam MUCs with the reply
 */
void Commands::replyTo(Swift::Message::ref source, std::string replyBody, bool outOfMUC) {
	Swift::Message::ref reply(new Swift::Message());
	Swift::Message::Type type = source->getType();
	reply->setType(type);
	reply->setBody(type == Swift::Message::Groupchat ? source->getFrom().getResource() + ": " + replyBody : replyBody);
	Swift::JID to = source->getFrom();
	if (type == Swift::Message::Groupchat) {
		if (outOfMUC) {
			reply->setType(Swift::Message::Chat);
		} else {
			to = to.toBare();
		}
	}
	reply->setTo(to);
	if (client_->isAvailable()) {
		client_->sendMessage(reply);
	} else {
		std::cout << "Dropping '" + reply->getBody() + "' -> " + reply->getTo().toString() + " on the floor due to missing connection." << std::endl;
	}
}
