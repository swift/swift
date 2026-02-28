/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtFdpFormSubmitWindow.h>

#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <Swift/QtUI/QtFormWidget.h>
#include <Swift/QtUI/QtListWidget.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {
QtFdpFormSubmitWindow::QtFdpFormSubmitWindow(QWidget* parent) : QDialog(parent), FdpFormSubmitWindow() {
    layout_ = new QVBoxLayout(this);
    subLayout_ = new QHBoxLayout(this);

    initNodeViewLayout();
    initFormLayout();
    subLayout_->addLayout(nodeViewLayout_);
    subLayout_->addLayout(formLayout_);
    subLayout_->setStretchFactor(nodeViewLayout_, 2);
    subLayout_->setStretchFactor(formLayout_, 3);
    layout_->addLayout(subLayout_);

    submitButton_ = new QPushButton(tr("Submit Form"), this);
    submitButton_->setEnabled(false);
    okButton_ = new QPushButton(tr("Ok"), this);
    okButton_->hide();
    cancelButton_ = new QPushButton(tr("Cancel"), this);
    connect(submitButton_, &QPushButton::clicked, this, &QtFdpFormSubmitWindow::handleSubmitClicked);
    connect(okButton_, &QPushButton::clicked, this, &QWidget::close);
    connect(cancelButton_, &QPushButton::clicked, this, &QWidget::close);
    auto buttonSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    buttonLayout_ = new QHBoxLayout(this);
    buttonLayout_->addItem(buttonSpacer);
    buttonLayout_->addWidget(submitButton_);
    buttonLayout_->addWidget(cancelButton_);
    layout_->addLayout(buttonLayout_);

    setMinimumWidth(800);
    setMinimumHeight(600);

    this->setWindowTitle(tr("FDP Form Submission"));
}

QtFdpFormSubmitWindow::~QtFdpFormSubmitWindow() {
}

void QtFdpFormSubmitWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    onCloseEvent();
}

void QtFdpFormSubmitWindow::initNodeViewLayout() {
    nodeViewLayout_ = new QVBoxLayout(this);
    auto domainSearchLayout = new QHBoxLayout(this);
    pubSubDomainEdit_ = new QLineEdit(this);
    loadDomainButton_ = new QPushButton(tr("Load"), this);
    pubSubNodeView_ = new QtListWidget(this);
    pubSubDomainEdit_->setPlaceholderText(tr("Enter pubsub domain here"));
    pubSubNodeView_->setMinimumWidth(300);
    connect(loadDomainButton_, &QPushButton::clicked, this, &QtFdpFormSubmitWindow::handleLoadDomainButtonClicked);
    connect(pubSubNodeView_, &QListWidget::itemDoubleClicked, this, &QtFdpFormSubmitWindow::handlePubSubNodeViewItemDoubleClicked);
    domainSearchLayout->addWidget(pubSubDomainEdit_);
    domainSearchLayout->addWidget(loadDomainButton_);
    nodeViewLayout_->addLayout(domainSearchLayout);
    nodeViewLayout_->addWidget(pubSubNodeView_);
}

void QtFdpFormSubmitWindow::initFormLayout() {
    formPlaceholder_ = new QLabel(tr("No form loaded"));
    formPlaceholder_->setAlignment(Qt::AlignCenter);
    formPlaceholder_->setMinimumWidth(200);
    formPlaceholder_->setStyleSheet("QLabel { color : #AAAAAA; }");
    formLayout_ = new QVBoxLayout(this);
    formLayout_->addWidget(formPlaceholder_, Qt::AlignCenter);
}

void QtFdpFormSubmitWindow::show() {
    QDialog::show();
}

void QtFdpFormSubmitWindow::raise() {
    QDialog::raise();
}

void QtFdpFormSubmitWindow::addNode(const std::string& node, const std::string& nodeName) {
    auto listItem = new QListWidgetItem(P2QSTRING(nodeName));
    listItem->setData(Qt::UserRole, P2QSTRING(node));
    pubSubNodeView_->addItem(listItem);
}

void QtFdpFormSubmitWindow::clearNodeData() {
    pubSubNodeView_->clear();
    pubSubNodeView_->setWordWrap(false);
    disconnect(pubSubNodeView_, &QtListWidget::onResize, this, &QtFdpFormSubmitWindow::handleNodeListResize);
}

void QtFdpFormSubmitWindow::setFormData(const std::shared_ptr<Form>& form) {
    if (formWidget_) {
        formLayout_->removeWidget(formWidget_);
        formWidget_->deleteLater();
        formWidget_ = nullptr;
    }
    else if (!formPlaceholder_->isHidden()) {
        formLayout_->removeWidget(formPlaceholder_);
        formPlaceholder_->hide();
    }
    formWidget_ = new QtFormWidget(form);
    formWidget_->setEditable(true);
    formLayout_->addWidget(formWidget_);

    if (!okButton_->isHidden()) {
        buttonLayout_->removeWidget(okButton_);
        okButton_->hide();
    }
    if (submitButton_->isHidden()) {
        buttonLayout_->insertWidget(1, submitButton_);
        submitButton_->show();
    }
    submitButton_->setEnabled(true);
    cancelButton_->setEnabled(true);
}

void QtFdpFormSubmitWindow::showNodePlaceholder(NodeError nodeError) {
    pubSubNodeView_->clear();
    pubSubNodeView_->setWordWrap(true);
    auto listItem = new QListWidgetItem;
    auto placeholderText = P2QSTRING(getNodeErrorText(nodeError));
    listItem->setText(placeholderText);
    listItem->setTextAlignment(Qt::AlignCenter);
    listItem->setFlags(Qt::NoItemFlags);
    listItem->setSizeHint(QSize(listItem->sizeHint().width(), pubSubNodeView_->height()));
    connect(pubSubNodeView_, &QtListWidget::onResize, this, &QtFdpFormSubmitWindow::handleNodeListResize);
    pubSubNodeView_->addItem(listItem);
}

void QtFdpFormSubmitWindow::showFormPlaceholder(TemplateError templateError) {
    if (formWidget_) {
        formLayout_->removeWidget(formWidget_);
        formWidget_->deleteLater();
        formWidget_ = nullptr;
    }
    auto placeholderText = P2QSTRING(getTemplateErrorText(templateError));
    formPlaceholder_->setText(placeholderText);
    if (formPlaceholder_->isHidden()) {
        formLayout_->addWidget(formPlaceholder_, Qt::AlignCenter);
        formPlaceholder_->show();
    }

    if (!okButton_->isHidden()) {
        buttonLayout_->removeWidget(okButton_);
        okButton_->hide();
    }
    if (submitButton_->isHidden()) {
        buttonLayout_->insertWidget(1, submitButton_);
        submitButton_->show();
    }
    submitButton_->setEnabled(false);
    cancelButton_->setEnabled(true);
}

void QtFdpFormSubmitWindow::handleLoadDomainButtonClicked() {
    std::string domainUri = Q2PSTRING(pubSubDomainEdit_->text());
    onRequestPubSubNodeData(domainUri);
}

void QtFdpFormSubmitWindow::handlePubSubListViewTemplateSelected(const std::string& nodeName) {
    onRequestTemplateForm(nodeName);
}

void QtFdpFormSubmitWindow::handlePubSubNodeViewItemDoubleClicked(QListWidgetItem* item) {
    handlePubSubListViewTemplateSelected(Q2PSTRING(item->data(Qt::UserRole).toString()));
}

void QtFdpFormSubmitWindow::handleSubmitClicked() {
    auto form = formWidget_->getCompletedForm();
    formWidget_->setDisabled(true);
    submitButton_->setEnabled(false);
    onSubmitForm(form);
}

void QtFdpFormSubmitWindow::handleSubmitServerResponse(bool submissionSuccess) {
    if (submissionSuccess) {
        if (!submitButton_->isHidden()) {
            buttonLayout_->removeWidget(submitButton_);
            submitButton_->hide();
        }
        if (okButton_->isHidden()) {
            buttonLayout_->insertWidget(1, okButton_);
            okButton_->show();
        }
        cancelButton_->setEnabled(false);
    }
    else {
        formWidget_->setDisabled(false);
        submitButton_->setEnabled(true);
    }
}

void QtFdpFormSubmitWindow::handleNodeListResize() {
    auto placeholderItem = pubSubNodeView_->item(0);
    placeholderItem->setSizeHint(QSize(placeholderItem->sizeHint().width(), pubSubNodeView_->height()));
}

}
