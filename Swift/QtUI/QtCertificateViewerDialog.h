/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QDialog>
#include <QList>
#include <QSslCertificate>
#include <QTreeWidgetItem>

#include <Swiften/TLS/Certificate.h>

namespace Ui {
class QtCertificateViewerDialog;
}

namespace Swift {

class QtCertificateViewerDialog : public QDialog {
    Q_OBJECT

    public:
        explicit QtCertificateViewerDialog(QWidget* parent = 0);
        ~QtCertificateViewerDialog();

        void setCertificateChain(const std::vector<Certificate::ref>& chain);

        static void displayCertificateChainAsSheet(QWidget* parent, const std::vector<Certificate::ref>& chain);

    private slots:
        void currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);

    private:
        void setCertificateDetails(const QSslCertificate& cert);

    private:
        Ui::QtCertificateViewerDialog *ui;
        QList<QSslCertificate> currentChain;
};

}
