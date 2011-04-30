/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include <Swiften/Base/Platform.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Base/boost_bsignals.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Network/BoostNetworkFactories.h>

#include <Swiftob/Commands.h>
#include <Swiftob/LuaCommands.h>

namespace po = boost::program_options;

class Users;
class Storage;
class Swiftob {
	public:
		Swiftob(const po::variables_map& options);
		static po::options_description getOptionsDescription();
		int exec();
		~Swiftob();
	private:
		void handleConnected();
		void handleDisconnected(const boost::optional<Swift::ClientError>&);
		void handleMessageReceived(Swift::Message::ref);
	private:
		const po::variables_map options_;
		Swift::SimpleEventLoop eventLoop_;
		Swift::BoostNetworkFactories networkFactories_;
		Commands* commands_;
		LuaCommands* lua_;
		Storage* storage_;
		MUCs* mucs_;
		bool quitting_;
		Users* users_;
		Swift::Client* client_;
};
