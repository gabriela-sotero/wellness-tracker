#include <QApplication>

#include "Database.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    // QApplication owns the event loop; Database outlives the main window that borrows it.
    QApplication application(argc, argv);

    Database database("data/wellness.db");
    database.createTables();

    MainWindow window(database);
    window.show();

    return application.exec();
}
