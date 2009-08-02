#include "Slimber/MainController.h"

#include <boost/bind.hpp>
#include <iostream>

#include "Swiften/Base/foreach.h"
#include "Swiften/Application/Platform/PlatformApplication.h"
#include "Swiften/LinkLocal/LinkLocalService.h"
#include "Swiften/LinkLocal/LinkLocalServiceBrowser.h"
#include "Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.h"
#include "Slimber/Server.h"
#include "Slimber/FileVCardCollection.h"
#include "Slimber/MenuletController.h"
#include "Slimber/Menulet.h"

using namespace Swift;

MainController::MainController(Menulet* menulet) : menulet(menulet) {
	menuletController = new MenuletController(menulet);
	menuletController->onRestartRequested.connect(boost::bind(
			&MainController::handleRestartRequested, this));

	dnsSDQuerier = boost::shared_ptr<BonjourQuerier>(new BonjourQuerier());

	linkLocalServiceBrowser = new LinkLocalServiceBrowser(dnsSDQuerier);
	linkLocalServiceBrowser->onServiceAdded.connect(
			boost::bind(&MainController::handleServicesChanged, this));
	linkLocalServiceBrowser->onServiceRemoved.connect(
			boost::bind(&MainController::handleServicesChanged, this));
	linkLocalServiceBrowser->onServiceChanged.connect(
			boost::bind(&MainController::handleServicesChanged, this));

	vCardCollection = new FileVCardCollection(
			PlatformApplication("Slimber").getSettingsDir());

	server = new Server(5222, 5562, linkLocalServiceBrowser, vCardCollection);
	server->onStopped.connect(
			boost::bind(&MainController::handleServerStopped, this, _1));
	server->onSelfConnected.connect(
			boost::bind(&MainController::handleSelfConnected, this, _1));

	start();
}

MainController::~MainController() {
	delete menuletController;
	delete server;
	delete vCardCollection;
	linkLocalServiceBrowser->stop();
	delete linkLocalServiceBrowser;
	dnsSDQuerier->stop();
}

void MainController::start() {
	dnsSDQuerier->start();
	linkLocalServiceBrowser->start();

	handleSelfConnected(false);
	handleServicesChanged();

	server->start();
}

void MainController::stop() {
	server->stop();
	linkLocalServiceBrowser->stop();
	dnsSDQuerier->stop();
}

void MainController::handleSelfConnected(bool b) {
	if (b) {
		menuletController->setXMPPStatus("You are logged in", MenuletController::Online);
	}
	else {
		menuletController->setXMPPStatus("You are not logged in", MenuletController::Offline);
	}
}

void MainController::handleServicesChanged() {
	std::vector<String> names;
	foreach(const LinkLocalService& service, linkLocalServiceBrowser->getServices()) {
		names.push_back(service.getDescription());
	}
	menuletController->setUserNames(names);
}

void MainController::handleServerStopped(boost::optional<ServerError> error) {
	if (error) {
		menuletController->setXMPPStatus("XMPP Server Error", MenuletController::Offline);
	}
	else {
		menuletController->setXMPPStatus("XMPP Server Not Running", MenuletController::Offline);
	}
}

void MainController::handleRestartRequested() {
	std::cout << "RESTART!" << std::endl;
	stop();
	start();
}
