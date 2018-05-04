/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <QListView>

namespace Swift {

class QtExpandedListView : public QListView {
public:
    QtExpandedListView(QWidget* parent);
    ~QtExpandedListView() override;

    void setModel(QAbstractItemModel* model) override;
    bool viewportEvent(QEvent* event) override;
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected slots:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private slots:
    void adjustHeightToModelChange();

private:
    void connectToModel(QAbstractItemModel* model);
    void disconnectFromModel(QAbstractItemModel* model);
};

}
