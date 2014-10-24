/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/ResultSet.h>

namespace Swift {
	class SWIFTEN_API MAMFin : public Payload {
		public:
			MAMFin() : isComplete_(false), isStable_(true) {}
			virtual ~MAMFin();

			void setComplete(const bool isComplete) {
				isComplete_ = isComplete;
			}

			bool isComplete() const {
				return isComplete_;
			}

			void setStable(const bool isStable) {
				isStable_ = isStable;
			}

			bool isStable() const {
				return isStable_;
			}

			void setResultSet(boost::shared_ptr<ResultSet> resultSet) {
				resultSet_ = resultSet;
			}

			boost::shared_ptr<ResultSet> getResultSet() const {
				return resultSet_;
			}

			void setQueryID(const std::string& queryID) {
				queryID_ = queryID;
			}

			const boost::optional<std::string>& getQueryID() const {
				return queryID_;
			}


		private:
			bool isComplete_;
			bool isStable_;
			boost::shared_ptr<ResultSet> resultSet_;
			boost::optional<std::string> queryID_;
	};
}
