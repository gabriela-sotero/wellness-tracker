#include "ConsoleUI.h"
#include "Database.h"

int main() {
    Database database("data/wellness.db");
    database.createTables();

    ConsoleUI app(database);
    app.run();

    return 0;
}