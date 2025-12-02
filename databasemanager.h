#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QList>

#include "item.h"
#include "user.h"

class DatabaseManager {
public:
    static DatabaseManager& instance(); // Singleton instance

    bool openDatabase(const QString &dbPath);
    void closeDatabase();
    bool createTables();

    // Item operations
    bool addItem(Item &item);
    bool updateItem(const Item &item);
    bool removeItem(int itemId);
    Item getItemById(int itemId);
    QList<Item> getAllItems();
    QList<Item> findItems(const QString& query); // For searching items

    // User operations
    bool addUser(User &user);
    bool updateUser(const User &user);
    bool removeUser(int userId);
    User getUserById(int userId);
    User getUserByName(const QString& name);
    QList<User> getAllUsers();

    bool clearAndSeedData(); // For initial data population

private:
    DatabaseManager(); // Private constructor for singleton
    ~DatabaseManager();

    QSqlDatabase m_db;

    // Helper functions for conversions
    QVariant itemTypeToVariant(ItemType type) const;
    ItemType variantToItemType(const QVariant& variant) const;
    QVariant availabilityToVariant(Availability status) const;
    Availability variantToAvailability(const QVariant& variant) const;
    QVariant userTypeToVariant(UserType type) const;
    UserType variantToUserType(const QVariant& variant) const;

    QString qListIntToString(const QList<int>& list) const;
    QList<int> stringToQListInt(const QString& str) const;
};

#endif // DATABASEMANAGER_H
