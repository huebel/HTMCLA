#include "htm.h"
#include <QtWidgets/QApplication>
#include <stdlib.h>
#include "MemManager.h"
#include "NetworkManager.h"

namespace htm {

MemManager mem_manager;

};

int main(int argc, char *argv[])
{
    // Create the NetworkManager
    htm::NetworkManager *networkManager = new htm::NetworkManager();

	// Create the application
	QApplication app(argc, argv);

	// Create and show the window
    htm::htm window(networkManager, NULL);
	window.show();

	// Run the application
	int result = app.exec();

	delete networkManager;

	return result;
}

