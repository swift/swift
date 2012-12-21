/*
 * Copyright (c) 2012 Maciej Niedzielski
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swift/Controllers/UIEvents/UIEvent.h>

namespace Swift {

	class UIEventStream;

	class HighlightEditorWidgetFactory;
	class HighlightEditorWidget;

	class HighlightManager;

	class HighlightEditorController {
		public:
			HighlightEditorController(UIEventStream* uiEventStream, HighlightEditorWidgetFactory* highlightEditorWidgetFactory, HighlightManager* highlightManager);
			~HighlightEditorController();

			HighlightManager* getHighlightManager() const { return highlightManager_; }

		private:
			void handleUIEvent(boost::shared_ptr<UIEvent> event);

		private:
			HighlightEditorWidgetFactory* highlightEditorWidgetFactory_;
			HighlightEditorWidget* highlightEditorWidget_;
			HighlightManager* highlightManager_;
	};

}
