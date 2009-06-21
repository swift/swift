#include <QApplication>

#include "QtSwift.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	bool netbookMode = argc > 1;
	Swift::QtSwift swift(netbookMode);
	return app.exec();
}
