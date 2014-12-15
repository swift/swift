/*
 * Copyright (c) 2010-2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Client/Storages.h>

namespace Swift {
	class VCardMemoryStorage;
	class CryptoProvider;

	/**
	 * An implementation of Storages for storing all
	 * controller data in memory.
	 */
	class MemoryStorages : public Storages {
		public:
			MemoryStorages(CryptoProvider*);
			~MemoryStorages();

			virtual VCardStorage* getVCardStorage() const;
			virtual AvatarStorage* getAvatarStorage() const;
			virtual CapsStorage* getCapsStorage() const;
			virtual RosterStorage* getRosterStorage() const;
			virtual HistoryStorage* getHistoryStorage() const;

		private:
			VCardMemoryStorage* vcardStorage;
			AvatarStorage* avatarStorage;
			CapsStorage* capsStorage;
			RosterStorage* rosterStorage;
			HistoryStorage* historyStorage;
	};
}
