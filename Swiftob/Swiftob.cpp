/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiftob/Swiftob.h>

#include <iostream>
#include <string>

#include <boost/bind.hpp>

#include <Swiften/Base/String.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Presence/PresenceSender.h>

#include <Swiftob/Storage.h>
#include <Swiftob/Users.h>

po::options_description Swiftob::getOptionsDescription() {
	po::options_description result("Options");
	result.add_options()
		("path", po::value<std::string>(), "Configuration folder")
		("help", "produce help message")
		("init", "Reset everything (Really, everything, be careful, you only want to use this on first run).")
		("jid", po::value<std::string>(), "JID to use")
		("password", po::value<std::string>(), "password")
		("initial-owner", po::value<std::string>(), "Initial bot owner (JID)")
		;
	return result;
}

Swiftob::Swiftob(const po::variables_map& options) : options_(options), networkFactories_(&eventLoop_), quitting_(false) {
	path_ = options["path"].as<std::string>();
	client_ = new Swift::Client(Swift::JID(options["jid"].as<std::string>()), options["password"].as<std::string>(), &networkFactories_);
	storage_ = new Storage(boost::filesystem::path(path_) / "settings.txt");
	mucs_ = NULL;
	users_ = NULL;
	commands_ = NULL;
	lua_ = NULL;
	init();
	client_->onConnected.connect(boost::bind(&Swiftob::handleConnected, this));
	client_->onDisconnected.connect(boost::bind(&Swiftob::handleDisconnected, this, _1));
	client_->onMessageReceived.connect(boost::bind(&Swiftob::handleMessageReceived, this, _1));
	if (options_.count("init") > 0) {

	} else if (options_.count("jid") > 0 || options_.count("password") > 0 || options_.count("initial-owner") == 0) {
		std::cout << "Ignoring initial config options without --initial" << std::endl;
	}
	client_->setAlwaysTrustCertificates();
	client_->setSoftwareVersion("Swiftob", "pregit", "");
	client_->connect();
	eventLoop_.run();
}

void Swiftob::init() {
	delete mucs_;
	mucs_ = new MUCs(client_, storage_);
	delete users_;
	users_ = new Users(client_, mucs_);
	delete commands_;
	commands_ = new Commands(users_, client_, storage_, mucs_);
	commands_->onRestartRequested.connect(boost::bind(&Swiftob::handleRestartRequested, this));
	delete lua_;
	lua_ = new LuaCommands(commands_, path_, client_, networkFactories_.getTimerFactory(), mucs_);
}

void Swiftob::handleRestartRequested() {
	client_->disconnect();
	init();
}

void Swiftob::handleConnected() {
	std::cout << "Connected" << std::endl;
	if (options_.count("init") > 0) {}{ /* FIXME: Not ready for persistence yet*/
		users_->clearAll();
		users_->addUser(Users::User(Swift::JID(options_["initial-owner"].as<std::string>()), Users::User::Owner));
	}
	Swift::Presence::ref presence(new Swift::Presence());
	presence->setStatus("Online and botty");
	client_->getPresenceSender()->sendPresence(presence);
}

void Swiftob::handleDisconnected(const boost::optional<Swift::ClientError>& /*error*/) {
	std::cout << "Disconnected" << std::endl;
	/* FIXME: check if last connect was more than a minute ago. If so, go ahead and connect, if not then wait a minute before connecting.*/
	if (quitting_) {
		eventLoop_.stop();
	} else {
		client_->connect();
	}
}

void Swiftob::handleMessageReceived(Swift::Message::ref message) {
	Swift::Message::Type type = message->getType();
	if (type == Swift::Message::Error || type == Swift::Message::Headline) {
		std::cout << "Ignoring typed message" << std::endl;
		return;
	}
	std::string body = message->getBody().get_value_or("");
	std::cout << "Got message with body " << body << std::endl;
	if (body.size() == 0) {
		std::cout << "Not handling empty body" << std::endl;
		return;
	}

	/* Run through any full-message listeners */
	commands_->runListeners(message);

	/*Convert body into !command if it's not a MUC, and it misses the bang*/
	std::string bangBody(body);
	if (type != Swift::Message::Groupchat && body[0] != '!') {
		bangBody = "!" + body;
	}
	std::cout << "After banging, body is " << bangBody << std::endl;
	std::pair<std::string, std::string> split = Swift::String::getSplittedAtFirst(bangBody, ' ');
	std::string commandName(split.first);
	commandName = Swift::String::getSplittedAtFirst(commandName, '!').second;
	/*FIXME: remove leading bang in commandName*/
	if (commands_->hasCommand(commandName)) {
		std::cout << "Matched command " << commandName << std::endl;
		commands_->runCommand(commandName, split.second, message);
	}
}

Swiftob::~Swiftob() {
	delete lua_;
	delete commands_;
	delete storage_;
	delete users_;
	delete mucs_;
	delete client_;
}

int Swiftob::exec() {
	return 0;
}

