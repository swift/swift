/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindow.h>

namespace Swift {

    class Form;

    class MockFdpFormSubmitWindow : public FdpFormSubmitWindow {
        public:
            MockFdpFormSubmitWindow() : FdpFormSubmitWindow() {}
            virtual void show() override {}
            virtual void raise() override {}
            virtual void addNode(const std::string& node, const std::string& nodeName) override { nodeData.push_back(std::pair<std::string, std::string>(node, nodeName)); }
            virtual void clearNodeData() override { nodeData.clear(); }
            virtual void setFormData(const std::shared_ptr<Form>& form) override { templateForm_ = form; }
            virtual void showNodePlaceholder(NodeError nodeError) override { nodeError_ = nodeError; }
            virtual void showFormPlaceholder(TemplateError templateError) override { templateError_ = templateError; }
            virtual void handleSubmitServerResponse(bool submissionSuccess) override { submissionSuccess_ = submissionSuccess; }

            std::vector<std::pair<std::string, std::string>> nodeData;
            std::shared_ptr<Form> templateForm_ = nullptr;
            NodeError nodeError_ = NodeError::NoError;
            TemplateError templateError_ = TemplateError::NoError;
            bool submissionSuccess_ = false;
    };

}
