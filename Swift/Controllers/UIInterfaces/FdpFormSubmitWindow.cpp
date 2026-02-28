/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindow.h>

#include <Swift/Controllers/Intl.h>

namespace Swift {

std::string FdpFormSubmitWindow::getNodeErrorText(NodeError nodeError) const {
    switch(nodeError) {
        case NodeError::DomainNotFound: return QT_TRANSLATE_NOOP("", "Error: No pubsub domain found");
        case NodeError::NoFdpNodesInDomain: return QT_TRANSLATE_NOOP("", "Error: Domain does not contain an FDP template node");
        case NodeError::NoError: return "";
    }
    return "";
}

std::string FdpFormSubmitWindow::getTemplateErrorText(TemplateError templateError) const {
    switch(templateError) {
        case TemplateError::CannotLocateForm: return QT_TRANSLATE_NOOP("", "Error: Could not locate template form");
        case TemplateError::InvalidPayload: return QT_TRANSLATE_NOOP("", "Error: Invalid payload returned from node");
        case TemplateError::RequestFailed: return QT_TRANSLATE_NOOP("", "Error: Pubsub request failed");
        case TemplateError::NoError: return "";
    }
    return "";
}

}
