/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/signals2.hpp>

namespace Swift {

    class Form;

    class FdpFormSubmitWindow {

        public:
            enum class NodeError {
                DomainNotFound,
                NoFdpNodesInDomain,
                NoError,
            };
            enum class TemplateError {
                CannotLocateForm,
                InvalidPayload,
                RequestFailed,
                NoError,
            };

            virtual ~FdpFormSubmitWindow() {}

            virtual void show() = 0;
            virtual void raise() = 0;
            virtual void addNode(const std::string& node, const std::string& nodeName) = 0;
            virtual void clearNodeData() = 0;
            virtual void setFormData(const std::shared_ptr<Form>& form) = 0;
            virtual void showNodePlaceholder(NodeError nodeError) = 0;
            virtual void showFormPlaceholder(TemplateError templateError) = 0;
            virtual void handleSubmitServerResponse(bool submissionSuccess) = 0;

            boost::signals2::signal<void ()> onCloseEvent;
            boost::signals2::signal<void (const std::string&)> onRequestPubSubNodeData;
            boost::signals2::signal<void (const std::string&)> onRequestTemplateForm;
            boost::signals2::signal<void (const std::shared_ptr<Form>&)> onSubmitForm;

        protected:

            std::string getNodeErrorText(NodeError nodeError) const;
            std::string getTemplateErrorText(TemplateError templateError) const;
    };
}
