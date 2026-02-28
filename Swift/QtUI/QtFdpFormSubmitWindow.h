/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <QDialog>

#include <Swift/Controllers/UIInterfaces/FdpFormSubmitWindow.h>

class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidgetItem;
class QPushButton;
class QTextEdit;
class QVBoxLayout;

namespace Swift {

    class Form;
    class QtFormWidget;
    class QtListWidget;
    class QtPubSubNodeController;

    class QtFdpFormSubmitWindow : public QDialog, public FdpFormSubmitWindow {
        Q_OBJECT

        public:
            QtFdpFormSubmitWindow(QWidget* parent = nullptr);
            virtual ~QtFdpFormSubmitWindow() override;

        protected:
            virtual void closeEvent(QCloseEvent* event) override;

        private:
            void initNodeViewLayout();
            void initFormLayout();
            virtual void show() override;
            virtual void raise() override;
            virtual void addNode(const std::string& node, const std::string& nodeName) override;
            virtual void clearNodeData() override;
            virtual void setFormData(const std::shared_ptr<Form>& form) override;
            virtual void showNodePlaceholder(NodeError nodeError) override;
            virtual void showFormPlaceholder(TemplateError templateError) override;
            virtual void handleSubmitServerResponse(bool submissionSuccess) override;

        private slots:
            void handleLoadDomainButtonClicked();
            void handlePubSubListViewTemplateSelected(const std::string& nodeName);
            void handlePubSubNodeViewItemDoubleClicked(QListWidgetItem* item);
            void handleSubmitClicked();
            void handleNodeListResize();

        private:
            QVBoxLayout* layout_;
            QHBoxLayout* subLayout_;
            QHBoxLayout* buttonLayout_;
            QVBoxLayout* nodeViewLayout_;
            QVBoxLayout* formLayout_;
            QLineEdit* pubSubDomainEdit_;
            QPushButton* loadDomainButton_;
            QtListWidget* pubSubNodeView_;
            QLabel* formPlaceholder_;
            QTextEdit* nodePlaceholderTextEdit_ = nullptr;
            QtFormWidget* formWidget_ = nullptr;
            QPushButton* cancelButton_;
            QPushButton* submitButton_ = nullptr;
            QPushButton* okButton_ = nullptr;
    };
}
