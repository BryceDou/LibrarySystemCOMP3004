#ifndef CATALOGUE_H
#define CATALOGUE_H

#include <QString>
#include "item.h"
#include "user.h"
#include "databasemanager.h" // Include DatabaseManager

class Catalogue {
public:
    // QList<Item> items; // Removed
    // QList<User> users; // Removed

    Item getItem(int id); // Changed from findItem to getItem
    User getUserById(int id); // Changed from findUserById to getUserById

    // Case-insensitive match on name
    User getUserByName(const QString& name); // Changed from findUserByName to getUserByName

    QList<Item> getAllItems();
    QList<User> getAllUsers();

    void seedDefaultData(); // builds 20 items + 7 users
};

#endif // CATALOGUE_H
