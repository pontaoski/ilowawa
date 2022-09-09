#include <QApplication>
#include <QQmlEngine>
#include <KLocalizedString>

#include "app.h"
#include "foreigns.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	KLocalizedString::setApplicationDomain("org.kde.ilowawa");

	QApplication app(argc, argv);
	app.setDesktopFileName("org.kde.Ilowawa");
	SApp sApp;

	sApp.start();

	return QCoreApplication::exec();
}
