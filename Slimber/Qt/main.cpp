#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>

#include "QtMenulet.h"
#include "Slimber/MainController.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
QMessageBox::critical(0, QObject::tr("Systray"), QObject::tr("No system tray"));
		return 1;
	}

	QtMenulet menulet;
	MainController controller(&menulet);

	return app.exec();
}
