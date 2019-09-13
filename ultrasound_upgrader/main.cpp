#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include "UpgradeMainWindow.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption checkUpgradeOption(
		"check",
		QCoreApplication::translate("main", "check if need to upgrade!")
	);
	parser.addOption(checkUpgradeOption);

	QCommandLineOption upgradeOption(
		"upgrade",
		QCoreApplication::translate("main", "begin to upgrade application"),
		QCoreApplication::translate("main", "upgrade directory")
	);
	parser.addOption(upgradeOption);
	parser.process(a);

	bool bUpgrade = parser.isSet(upgradeOption);
	UpgradeMainWindow mainWindow(bUpgrade);
	mainWindow.setUpgradeDir(parser.value(upgradeOption));
	mainWindow.show();
	return a.exec();
}
