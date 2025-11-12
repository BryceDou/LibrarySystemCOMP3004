#ifndef CATALOGUE_H
#define CATALOGUE_H

#include <QList>
#include <QString>
#include "item.h"
#include "user.h"

class Catalogue {
public:
    QList<Item> items;
    QList<User> users;

    Item* findItem(int id);
    const Item* findItem(int id) const;

    User* findUserById(int id);
    const User* findUserById(int id) const;

    // Case-insensitive match on name
    User* findUserByName(const QString& name);

    void seedDefaultData(); // builds 20 items + 7 users
};

#endif // CATALOGUE_H
