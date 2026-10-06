#include "ConsoleUI.h"
#include "Database.h"

int main() {
    // Keep Database alive for the entire lifetime of ConsoleUI and its service.
    Database database("data/wellness.db");
    database.createTables();

    ConsoleUI app(database);
    app.run();

    return 0;
}
