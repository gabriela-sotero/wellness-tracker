#include <QApplication>

#include "Database.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);

    Database database("data/wellness.db");
    database.createTables();

    MainWindow window(database);
    window.show();

    return application.exec();
}
