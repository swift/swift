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
			QString getHeader() const {return fileContents_[Header];};
			QString getFooter() const {return fileContents_[Footer];};
			QString getContent() const {return fileContents_[Content];};
			QString getStatus() const {return fileContents_[Status];};
			QString getTopic() const {return fileContents_[Topic];};
			QString getFileTransferRequest() const {return fileContents_[FileTransferRequest];};
			QString getIncomingContent() const {return fileContents_[IncomingContent];};
			QString getIncomingNextContent() const {return fileContents_[IncomingNextContent];};
			QString getIncomingContext() const {return fileContents_[IncomingContext];};
			QString getIncomingNextContext() const {return fileContents_[IncomingNextContext];};
			QString getOutgoingContent() const {return fileContents_[OutgoingContent];};
			QString getOutgoingNextContent() const {return fileContents_[OutgoingNextContent];};
			QString getOutgoingContext() const {return fileContents_[OutgoingContext];};
			QString getOutgoingNextContext() const {return fileContents_[OutgoingNextContext];};
			QString getTemplate() const {return fileContents_[Template];}
			QString getMainCSS() const {return fileContents_[MainCSS];}
			QString getBase() const;

		private:
			enum files {Header = 0, Footer, Content, Status, Topic, FileTransferRequest, IncomingContent, IncomingNextContent, IncomingContext, IncomingNextContext, OutgoingContent, OutgoingNextContent, OutgoingContext, OutgoingNextContext, Template, MainCSS, TemplateDefault, EndMarker};
			bool qrc_;
			QList<QString> fileContents_;
			QString themePath_;
	};
}
