/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

/*
 * Copyright (c) 2013 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swift/QtUI/Roster/QtFilterWidget.h>

#include <QEvent>
#include <QKeyEvent>
#include <QLayout>
#include <QString>
#include <QVBoxLayout>

#include <Swift/Controllers/UIEvents/RequestChatUIEvent.h>
#include <Swift/Controllers/UIEvents/UIEventStream.h>

#include <Swift/QtUI/QtClosableLineEdit.h>
#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtFilterWidget::QtFilterWidget(QWidget* parent, QtTreeWidget* treeView, UIEventStream* eventStream, QBoxLayout* layout) : QWidget(parent), treeView_(treeView), eventStream_(eventStream), fuzzyRosterFilter_(nullptr), isModifierSinglePressed_(false) {
    int targetIndex = layout->indexOf(treeView);

    QVBoxLayout* vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(0);
    vboxLayout->setContentsMargins(0,0,0,0);

    filterLineEdit_ = new QtClosableLineEdit(this);
    filterLineEdit_->hide();
    vboxLayout->addWidget(filterLineEdit_);

    vboxLayout->addWidget(treeView);
    setLayout(vboxLayout);
    layout->insertWidget(targetIndex, this);

    filterLineEdit_->installEventFilter(this);
    treeView_->installEventFilter(this);

    sourceModel_ = treeView_->model();
}

QtFilterWidget::~QtFilterWidget() {
    filterLineEdit_->removeEventFilter(this);
    if (treeView_) {
        treeView_->removeEventFilter(this);
        if (treeView_->getRoster()) {
            treeView_->getRoster()->onFilterAdded.disconnect(boost::bind(&QtFilterWidget::handleFilterAdded, this, _1));
            treeView_->getRoster()->onFilterRemoved.disconnect(boost::bind(&QtFilterWidget::handleFilterRemoved, this, _1));
        }
    }
}

bool QtFilterWidget::eventFilter(QObject*, QEvent* event) {
    if (!treeView_) {
        return false;
    }

    if (event->type() == QEvent::KeyPress ||
        event->type() == QEvent::KeyRelease ||
        // InputMethodQuery got introduced in Qt 5.
#if QT_VERSION >= 0x050000
        event->type() == QEvent::InputMethodQuery ||
#endif
        event->type() == QEvent::InputMethod) {
        event->ignore();
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent) {
            if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
                return false;
            } else if ((keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right) && filterLineEdit_->text().isEmpty()) {
                return false;
            } else if (keyEvent->key() == Qt::Key_Alt && event->type() == QEvent::KeyPress) {
                isModifierSinglePressed_ = true;
            } else if ((keyEvent->key() == Qt::Key_Alt && event->type() == QEvent::KeyRelease && isModifierSinglePressed_)
                    || (keyEvent->key() == Qt::Key_Menu)) {
                QPoint itemOffset(2,2);
                QPoint contextMenuPosition = treeView_->visualRect(treeView_->currentIndex()).topLeft() + itemOffset;;
                QApplication::postEvent(treeView_, new QContextMenuEvent(QContextMenuEvent::Keyboard, contextMenuPosition, treeView_->mapToGlobal(contextMenuPosition)));
                return true;
            } else if (keyEvent->key() == Qt::Key_Return) {
                JID target = treeView_->selectedJID();
                if (target.isValid()) {
                    eventStream_->send(std::make_shared<RequestChatUIEvent>(target));
                }
                filterLineEdit_->setText("");
                updateRosterFilters();
            } else if (keyEvent->key() == Qt::Key_Escape) {
                filterLineEdit_->setText("");
            } else {
                isModifierSinglePressed_ = false;
            }
        }

        filterLineEdit_->event(event);

        if (event->type() == QEvent::KeyRelease) {
            updateRosterFilters();
        }
        return true;
    }
    return false;
}

void QtFilterWidget::popAllFilters() {
    std::vector<RosterFilter*> filters = treeView_->getRoster()->getFilters();
    for (auto filter : filters) {
        filters_.push_back(filter);
        treeView_->getRoster()->removeFilter(filter);
    }
    treeView_->getRoster()->onFilterAdded.connect(boost::bind(&QtFilterWidget::handleFilterAdded, this, _1));
    treeView_->getRoster()->onFilterRemoved.connect(boost::bind(&QtFilterWidget::handleFilterRemoved, this, _1));
}

void QtFilterWidget::pushAllFilters() {
    treeView_->getRoster()->onFilterAdded.disconnect(boost::bind(&QtFilterWidget::handleFilterAdded, this, _1));
    treeView_->getRoster()->onFilterRemoved.disconnect(boost::bind(&QtFilterWidget::handleFilterRemoved, this, _1));
    for (auto filter : filters_) {
        treeView_->getRoster()->addFilter(filter);
    }
    filters_.clear();
}

void QtFilterWidget::updateRosterFilters() {
    if (!treeView_) {
        return;
    }

    if (fuzzyRosterFilter_) {
        if (filterLineEdit_->text().isEmpty()) {
            // remove currently installed search filter and put old filters back
            treeView_->getRoster()->removeFilter(fuzzyRosterFilter_);
            delete fuzzyRosterFilter_;
            fuzzyRosterFilter_ = nullptr;
            pushAllFilters();
        } else {
            // remove currently intsalled search filter and put new search filter in place
            updateSearchFilter();
        }
    } else {
        if (!filterLineEdit_->text().isEmpty()) {
            // remove currently installed filters and put a search filter in place
            popAllFilters();
            updateSearchFilter();
        }
    }
    filterLineEdit_->setVisible(!filterLineEdit_->text().isEmpty());
}

void QtFilterWidget::updateSearchFilter() {
    if (!treeView_) {
        return;
    }

    if (fuzzyRosterFilter_) {
        treeView_->getRoster()->removeFilter(fuzzyRosterFilter_);
        delete fuzzyRosterFilter_;
        fuzzyRosterFilter_ = nullptr;
    }
    fuzzyRosterFilter_ = new FuzzyRosterFilter(Q2PSTRING(filterLineEdit_->text()));
    treeView_->getRoster()->addFilter(fuzzyRosterFilter_);
    treeView_->setCurrentIndex(sourceModel_->index(0, 0, sourceModel_->index(0,0)));
}

void QtFilterWidget::handleFilterAdded(RosterFilter* filter) {
    if (filter != fuzzyRosterFilter_) {
        filterLineEdit_->setText("");
        updateRosterFilters();
    }
}

void QtFilterWidget::handleFilterRemoved(RosterFilter* filter) {
    /* make sure we don't end up adding this one back in later */
    filters_.erase(std::remove(filters_.begin(), filters_.end(), filter), filters_.end());
    if (filter != fuzzyRosterFilter_) {
        filterLineEdit_->setText("");
        updateRosterFilters();
    }
}

}
