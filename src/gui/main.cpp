#include <QApplication>

#include "Database.h"
#include "LoginWindow.h"

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);

    Database database("data/wellness.db");
    database.createTables();

    LoginWindow window(database);
    window.show();

    return application.exec();
}
