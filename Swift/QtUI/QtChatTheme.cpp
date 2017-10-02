/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtChatTheme.h>

#include <QFile>

namespace Swift {

/**
 * Load Adium themes, as http://trac.adium.im/wiki/CreatingMessageStyles
 */
QtChatTheme::QtChatTheme(const QString& themePath) : qrc_(themePath[0] == ':'), themePath_(qrc_ ? themePath : themePath + "/Contents/Resources/") {
    QString fileNames[EndMarker];
    fileNames[Header] = "Header.html";
    fileNames[Footer] = "Footer.html";
    fileNames[Content] = "Content.html";
    fileNames[Status] = "Status.html";
    fileNames[Topic] = "Topic.html";
    fileNames[FileTransferRequest] = "FileTransferRequest.html";
    fileNames[IncomingContent] = "Incoming/Content.html";
    fileNames[IncomingNextContent] = "Incoming/NextContent.html";
    fileNames[IncomingContext] = "Incoming/Context.html";
    fileNames[IncomingNextContext] = "Incoming/NextContext.html";
    fileNames[OutgoingContent] = "Outgoing/Content.html";
    fileNames[OutgoingNextContent] = "Outgoing/NextContent.html";
    fileNames[OutgoingContext] = "Outgoing/Context.html";
    fileNames[OutgoingNextContext] = "Outgoing/NextContext.html";
    fileNames[Template] = "Template.html";
    fileNames[MainCSS] = "main.css";
    fileNames[Unread] = "Unread.html";
    fileNames[TemplateDefault] = ":/themes/Template.html";
    for (int i = 0; i < EndMarker; i++) {
        QString source;
        QFile sourceFile((i != TemplateDefault ? themePath_ : "") + fileNames[i]);
        if (sourceFile.exists() && sourceFile.open(QIODevice::ReadOnly)) {
            source = sourceFile.readAll();
            sourceFile.close();
        } else {
            //qWarning() << "Couldn't load file " << sourceFile.fileName();
        }
        fileContents_.append(source);
    }

    /* Fallbacks */
    if (fileContents_[Template].isEmpty()) fileContents_[Template] = fileContents_[TemplateDefault];
    if (fileContents_[Status].isEmpty()) fileContents_[Status] = fileContents_[Content];
    if (fileContents_[IncomingContent].isEmpty()) fileContents_[IncomingContent] = fileContents_[Content];
    if (fileContents_[IncomingNextContent].isEmpty()) fileContents_[IncomingNextContent] = fileContents_[IncomingContent];
    if (fileContents_[FileTransferRequest].isEmpty()) fileContents_[FileTransferRequest] = fileContents_[Status];
    if (fileContents_[IncomingContext].isEmpty()) fileContents_[IncomingContext] = fileContents_[IncomingContent];
    if (fileContents_[IncomingNextContext].isEmpty()) fileContents_[IncomingNextContext] = fileContents_[IncomingNextContent];
    if (fileContents_[OutgoingContent].isEmpty()) fileContents_[OutgoingContent] = fileContents_[IncomingContent];
    if (fileContents_[OutgoingContext].isEmpty()) fileContents_[OutgoingContext] = fileContents_[OutgoingContent];
    if (fileContents_[OutgoingNextContent].isEmpty()) fileContents_[OutgoingNextContent] = fileContents_[OutgoingContent];
    if (fileContents_[OutgoingNextContext].isEmpty()) fileContents_[OutgoingNextContext] = fileContents_[OutgoingNextContent];

}

QString QtChatTheme::getBase() const {
    return qrc_ ? "qrc" + themePath_ : "file://" + themePath_;
}

QString QtChatTheme::getUnread() const {
    return fileContents_[Unread].isEmpty() ? "<hr/>" : fileContents_[Unread];
}

}
