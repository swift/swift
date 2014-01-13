/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

namespace Swift {
	class HighlightEditorWindow;

	class HighlightEditorWindowFactory {
	public :
		virtual ~HighlightEditorWindowFactory() {}

		virtual HighlightEditorWindow* createHighlightEditorWindow() = 0;
	};
}
