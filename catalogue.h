#ifndef CATALOGUE_H
#define CATALOGUE_H

#include "item.h"
#include "patron.h"
#include <QList>

class catalogue {
public:
    QList<item>   items;
    QList<patron> patrons;

    item*   find_item(int id);
    patron* find_patron_by_name(const QString& name);

    // Load the default in-memory dataset (20 items + 7 users)
    void seed_default_data();
};

#endif // CATALOGUE_H
