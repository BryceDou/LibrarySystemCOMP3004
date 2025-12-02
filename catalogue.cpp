#include "catalogue.h"
#include <QDate>
#include <QDebug> // For logging database errors

// No longer needed as we're using the database manager for lookups
// static QString ci(const QString& s) { return s.trimmed().toLower(); }

Item Catalogue::getItem(int id) {
    return DatabaseManager::instance().getItemById(id);
}

User Catalogue::getUserById(int id) {
    return DatabaseManager::instance().getUserById(id);
}

User Catalogue::getUserByName(const QString& name) {
    return DatabaseManager::instance().getUserByName(name);
}

QList<Item> Catalogue::getAllItems() {
    return DatabaseManager::instance().getAllItems();
}

QList<User> Catalogue::getAllUsers() {
    return DatabaseManager::instance().getAllUsers();
}

void Catalogue::seedDefaultData() {
    // This will open the database, create tables if they don't exist,
    // clear existing data and then re-populate with default data.
    // The database file will be stored in a persistent location.
    // For now, let's use a fixed name for testing. In a real app, you might want to configure this.
    DatabaseManager::instance().clearAndSeedData();
        
}

