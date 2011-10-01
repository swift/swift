/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

#include <string>
#include <Swiften/Elements/IQ.h>

namespace Swift {
	class IQChannel;
	class IQHandler;

	class IQRouter {
		public:
			IQRouter(IQChannel* channel);
			~IQRouter();

			/**
			 * Sets the JID of this IQ router.
			 *
			 * This JID is used by requests to check whether incoming
			 * results are addressed correctly.
			 */
			void setJID(const JID& jid) {
				jid_ = jid;
			}

			const JID& getJID() const {
				return jid_;
			}

			/**
			 * Sets the 'from' JID for all outgoing IQ stanzas.
			 *
			 * By default, IQRouter does not add a from to IQ stanzas, since
			 * this is automatically added by the server. This overrides this
			 * default behavior, which is necessary for e.g. components.
			 */
			void setFrom(const JID& from) {
				from_ = from;
			}

			void addHandler(IQHandler* handler);
			void removeHandler(IQHandler* handler);
			void addHandler(boost::shared_ptr<IQHandler> handler);
			void removeHandler(boost::shared_ptr<IQHandler> handler);

			/**
			 * Sends an IQ stanza.
			 *
			 * If a JID was specified using setFrom, the JID will
			 * be set as the 'from' address on iq before sending
			 * it.
			 */
			void sendIQ(boost::shared_ptr<IQ> iq);
			std::string getNewIQID();
			
			bool isAvailable();

			/**
			 * Checks whether the given jid is the account JID (i.e. it is either
			 * the bare JID, or it is the empty JID).
			 * Can be used to check whether a stanza is sent by the server on behalf
			 * of the user's account.
			 */
			bool isAccountJID(const JID& jid) {
				return jid.isValid() ? jid_.toBare().equals(jid, JID::WithResource) : true;
			}

		private:
			void handleIQ(boost::shared_ptr<IQ> iq);
			void processPendingRemoves();

		private:
			IQChannel* channel_;
			JID jid_;
			JID from_;
			std::vector< boost::shared_ptr<IQHandler> > handlers_;
			std::vector< boost::shared_ptr<IQHandler> > queuedRemoves_;
			bool queueRemoves_;
	};
}
