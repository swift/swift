#include "Swift/QtUI/QtAboutWidget.h"

#include <QCoreApplication>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <QtGlobal>


#include "Swiften/Application/Application.h"

namespace Swift {
QtAboutWidget* QtAboutWidget::instance() {
	if (!instance_) {
		instance_ = new QtAboutWidget();
	}
	return instance_;
}

QtAboutWidget::QtAboutWidget() : QWidget() {
	resize(180, 240);
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setAlignment(Qt::AlignHCenter);
	setLayout(mainLayout);
	
	QLabel* iconLabel = new QLabel(this);
	iconLabel->setPixmap(QIcon(":/logo-shaded-text.256.png").pixmap(90, 90));
	mainLayout->addWidget(iconLabel);
	
	QLabel* appNameLabel = new QLabel("<font size='+1'><b>" + QCoreApplication::applicationName() + "</b></font>", this);
	//appNameLabel->setTextFormat
	mainLayout->addWidget(appNameLabel);
	
	QLabel* versionLabel = new QLabel(QString("<font size='-1'>Version ") + QCoreApplication::applicationVersion() + "</font>", this);
	mainLayout->addWidget(versionLabel);
	QString buildString = QString("<font size='-1'><centre>Built with: Qt version ") + QT_VERSION_STR;
	buildString += QString("</centre><br/><centre>Running with Qt version ") + qVersion();
	buildString += "</centre></font>";
	QLabel* buildLabel = new QLabel(buildString, this);
	mainLayout->addWidget(buildLabel);
	
}

QtAboutWidget* QtAboutWidget::instance_ = NULL;
}
