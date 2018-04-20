/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

#include <Swiften/JID/JID.h>

namespace Swift {
    class FdpFormSubmitWindow;
    class FdpFormSubmitWindowFactory;
    class Form;
    class IQRouter;
    class UIEvent;
    class UIEventStream;

    class FdpFormSubmitController {
    public:
        FdpFormSubmitController(const JID& self, IQRouter* iqRouter, UIEventStream* uiEventStream, FdpFormSubmitWindowFactory* factory);
        ~FdpFormSubmitController();

    private:
        void handleUIEvent(const std::shared_ptr<UIEvent>& uiEvent);
        void createFormSubmitWindow();
        void closeFormSubmitWindow();
        void requestPubSubNodeData(const std::string& domainName);
        void requestTemplateForm(const std::string& nodeName);
        void submitForm(const std::shared_ptr<Form>& form);

        JID selfJID_;
        IQRouter* iqRouter_;
        UIEventStream* uiEventStream_;
        FdpFormSubmitWindowFactory* factory_;
        std::unique_ptr<FdpFormSubmitWindow> formSubmitWindow_;
        std::string currentDomain_;
        std::string currentTemplateNode_;

        boost::signals2::scoped_connection fdpFormSubmitWindowOpenUIEventConnection_;
    };
}
