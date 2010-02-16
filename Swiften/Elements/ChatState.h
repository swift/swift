#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/Elements/Payload.h"

namespace Swift {
	class ChatState : public Payload {
		public:
			enum ChatStateType {Active, Composing, Paused, Inactive, Gone};
			ChatState(ChatStateType state = Active) {
				state_ = state;
			}

			ChatStateType getChatState() { return state_; }
			void setChatState(ChatStateType state) {state_ = state;}

		private:
			ChatStateType state_;
	};
}
