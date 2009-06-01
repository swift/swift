#include <QApplication>

#include "QtSwift.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	Swift::QtSwift swift;
	return app.exec();
}
