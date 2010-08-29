/*
 * Copyright (c) 2010 Kevin Smith.
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <QString>
#include <QList>

namespace Swift {
	class QtChatTheme {
		public:
			QtChatTheme(const QString& themePath);
			QString getHeader() {return fileContents_[Header];};
			QString getFooter() {return fileContents_[Footer];};
			QString getContent() {return fileContents_[Content];};
			QString getStatus() {return fileContents_[Status];};
			QString getTopic() {return fileContents_[Topic];};
			QString getFileTransferRequest() {return fileContents_[FileTransferRequest];};
			QString getIncomingContent() {return fileContents_[IncomingContent];};
			QString getIncomingNextContent() {return fileContents_[IncomingNextContent];};
			QString getIncomingContext() {return fileContents_[IncomingContext];};
			QString getIncomingNextContext() {return fileContents_[IncomingNextContext];};
			QString getOutgoingContent() {return fileContents_[OutgoingContent];};
			QString getOutgoingNextContent() {return fileContents_[OutgoingNextContent];};
			QString getOutgoingContext() {return fileContents_[OutgoingContext];};
			QString getOutgoingNextContext() {return fileContents_[OutgoingNextContext];};
			QString getTemplate() {return fileContents_[Template];}
			QString getMainCSS() {return fileContents_[MainCSS];}
			QString getBase();

		private:
			enum files {Header = 0, Footer, Content, Status, Topic, FileTransferRequest, IncomingContent, IncomingNextContent, IncomingContext, IncomingNextContext, OutgoingContent, OutgoingNextContent, OutgoingContext, OutgoingNextContext, Template, MainCSS, TemplateDefault, EndMarker};
			bool qrc_;
			QList<QString> fileContents_;
			QString themePath_;
	};
}
