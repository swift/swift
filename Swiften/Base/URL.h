/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <boost/lexical_cast.hpp>

namespace Swift {

class URL {
	public:

		URL() : scheme(""), user(""), password(""), host(""), port(-1), path(""), isEmpty(true) {
		}

		URL(const std::string& urlString) {
			host = urlString;
			port = 80;
			scheme = "http";
			isEmpty = false;
			//FIXME
		}

		URL(const std::string& scheme, const std::string& host, int port, const std::string& path) : scheme(scheme), user(), password(), host(host), port(port), path(path), isEmpty(false) {

		}	

		/**
		 * Whether the URL is empty.
		 */
		bool empty() const {
			return isEmpty;
		}

		/**
		 * Scheme used for the URL (http, https etc.)
		 */
		const std::string& getScheme() const {
			return scheme;
		}

		/**
		 * Hostname
		 */
		const std::string& getHost() const {
			return host;
		}

		/**
		 * Port number
		 */
		int getPort() const {
			return port;
		}

		/**
		 * Path 
		 */
		const std::string& getPath() const {
			return path;
		}

		const std::string toString() const {
			if (isEmpty) {
				return "";
			}
			std::string result = scheme + "://";
			if (!user.empty()) {
				result += user;
				if (!password.empty()) {
					result += ":" + password;
				}
				result += "@";
			}
			result += host;
			if (port > 0) {
				result += ":";
				result += boost::lexical_cast<std::string>(port);
			}
			result += "/";
			result += path;
			return result;
		}

	private:
		std::string scheme;
		std::string user;
		std::string password;
		std::string host;
		int port;
		std::string path;
		bool isEmpty;
	};
}
