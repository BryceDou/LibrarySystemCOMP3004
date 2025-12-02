#include "databasemanager.h"
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

// Singleton instance
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() {
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase(const QString &dbPath) {
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Error: connection with database failed" << m_db.lastError().text();
        return false;
    } else {
        qDebug() << "Database: connection ok";
    }
    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Database: connection closed";
    }
}

// Helper functions for enum conversions
QVariant DatabaseManager::itemTypeToVariant(ItemType type) const {
    return static_cast<int>(type);
}

ItemType DatabaseManager::variantToItemType(const QVariant& variant) const {
    return static_cast<ItemType>(variant.toInt());
}

QVariant DatabaseManager::availabilityToVariant(Availability status) const {
    return static_cast<int>(status);
}

Availability DatabaseManager::variantToAvailability(const QVariant& variant) const {
    return static_cast<Availability>(variant.toInt());
}

QVariant DatabaseManager::userTypeToVariant(UserType type) const {
    return static_cast<int>(type);
}

UserType DatabaseManager::variantToUserType(const QVariant& variant) const {
    return static_cast<UserType>(variant.toInt());
}

// Helper functions for QList<int> to/from QString
QString DatabaseManager::qListIntToString(const QList<int>& list) const {
    QStringList strList;
    for (int i : list) {
        strList << QString::number(i);
    }
    return strList.join(",");
}

QList<int> DatabaseManager::stringToQListInt(const QString& str) const {
    QList<int> list;
    if (str.isEmpty()) return list;
    QStringList strList = str.split(",", Qt::SkipEmptyParts);
    for (const QString& s : strList) {
        list << s.toInt();
    }
    return list;
}

// Placeholder for createTables - will implement next
bool DatabaseManager::createTables() {
    QSqlQuery query;

    // Create Item table
    if (!query.exec("CREATE TABLE IF NOT EXISTS items ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "type INTEGER,"
                    "title TEXT,"
                    "creator TEXT,"
                    "status INTEGER,"
                    "borrowerId INTEGER,"
                    "due TEXT,"
                    "dewey TEXT,"
                    "issue TEXT,"
                    "pub TEXT,"
                    "genre TEXT,"
                    "rating TEXT,"
                    "holdQueue TEXT)")) {
        qDebug() << "Error creating items table:" << query.lastError().text();
        return false;
    }

    // Create User table
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT,"
                    "type INTEGER,"
                    "loans TEXT,"
                    "holds TEXT)")) {
        qDebug() << "Error creating users table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database tables created successfully or already exist.";
    return true;
}

// Placeholder for other CRUD and seed functions
bool DatabaseManager::addItem(Item &item) {
    QSqlQuery query;
    query.prepare("INSERT INTO items (type, title, creator, status, borrowerId, due, dewey, issue, pub, genre, rating, holdQueue) "
                  "VALUES (:type, :title, :creator, :status, :borrowerId, :due, :dewey, :issue, :pub, :genre, :rating, :holdQueue)");
    query.bindValue(":type", itemTypeToVariant(item.type));
    query.bindValue(":title", item.title);
    query.bindValue(":creator", item.creator);
    query.bindValue(":status", availabilityToVariant(item.status));
    query.bindValue(":borrowerId", item.borrowerId);
    query.bindValue(":due", item.due.toString(Qt::ISODate));
    query.bindValue(":dewey", item.dewey);
    query.bindValue(":issue", item.issue);
    query.bindValue(":pub", item.pub.toString(Qt::ISODate));
    query.bindValue(":genre", item.genre);
    query.bindValue(":rating", item.rating);
    query.bindValue(":holdQueue", qListIntToString(item.holdQueue));

    if (!query.exec()) {
        qDebug() << "Error adding item:" << query.lastError().text();
        return false;
    }
    item.id = query.lastInsertId().toInt(); // Set the ID for the passed item
    return true;
}

bool DatabaseManager::updateItem(const Item &item) {
    QSqlQuery query;
    query.prepare("UPDATE items SET type = :type, title = :title, creator = :creator, status = :status, "
                  "borrowerId = :borrowerId, due = :due, dewey = :dewey, issue = :issue, pub = :pub, "
                  "genre = :genre, rating = :rating, holdQueue = :holdQueue WHERE id = :id");
    query.bindValue(":id", item.id);
    query.bindValue(":type", itemTypeToVariant(item.type));
    query.bindValue(":title", item.title);
    query.bindValue(":creator", item.creator);
    query.bindValue(":status", availabilityToVariant(item.status));
    query.bindValue(":borrowerId", item.borrowerId);
    query.bindValue(":due", item.due.toString(Qt::ISODate));
    query.bindValue(":dewey", item.dewey);
    query.bindValue(":issue", item.issue);
    query.bindValue(":pub", item.pub.toString(Qt::ISODate));
    query.bindValue(":genre", item.genre);
    query.bindValue(":rating", item.rating);
    query.bindValue(":holdQueue", qListIntToString(item.holdQueue));

    if (!query.exec()) {
        qDebug() << "Error updating item:" << query.lastError().text();
        return false;
    }
    return query.exec();
}

bool DatabaseManager::removeItem(int itemId) {
    QSqlQuery query;
    query.prepare("DELETE FROM items WHERE id = :id");
    query.bindValue(":id", itemId);
    if (!query.exec()) {
        qDebug() << "Error removing item:" << query.lastError().text();
        return false;
    }
    return true;
}

Item DatabaseManager::getItemById(int itemId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM items WHERE id = :id");
    query.bindValue(":id", itemId);
    Item item;
    if (query.exec() && query.next()) {
        item.id = query.value("id").toInt();
        item.type = variantToItemType(query.value("type"));
        item.title = query.value("title").toString();
        item.creator = query.value("creator").toString();
        item.status = variantToAvailability(query.value("status"));
        item.borrowerId = query.value("borrowerId").toInt();
        item.due = QDate::fromString(query.value("due").toString(), Qt::ISODate);
        item.dewey = query.value("dewey").toString();
        item.issue = query.value("issue").toString();
        item.pub = QDate::fromString(query.value("pub").toString(), Qt::ISODate);
        item.genre = query.value("genre").toString();
        item.rating = query.value("rating").toString();
        item.holdQueue = stringToQListInt(query.value("holdQueue").toString());
    } else {
        qDebug() << "Error getting item by ID:" << query.lastError().text();
    }
    return item;
}

QList<Item> DatabaseManager::getAllItems() {
    QList<Item> items;
    QSqlQuery query("SELECT * FROM items");
    while (query.next()) {
        Item item;
        item.id = query.value("id").toInt();
        item.type = variantToItemType(query.value("type"));
        item.title = query.value("title").toString();
        item.creator = query.value("creator").toString();
        item.status = variantToAvailability(query.value("status"));
        item.borrowerId = query.value("borrowerId").toInt();
        item.due = QDate::fromString(query.value("due").toString(), Qt::ISODate);
        item.dewey = query.value("dewey").toString();
        item.issue = query.value("issue").toString();
        item.pub = QDate::fromString(query.value("pub").toString(), Qt::ISODate);
        item.genre = query.value("genre").toString();
        item.rating = query.value("rating").toString();
        item.holdQueue = stringToQListInt(query.value("holdQueue").toString());
        items.append(item);
    }
    return items;
}

QList<Item> DatabaseManager::findItems(const QString& searchText) {
    QList<Item> items;
    QSqlQuery query;
    // Search by title, creator, genre, dewey (case-insensitive)
    query.prepare("SELECT * FROM items WHERE "
                  "LOWER(title) LIKE LOWER(:searchText) OR "
                  "LOWER(creator) LIKE LOWER(:searchText) OR "
                  "LOWER(genre) LIKE LOWER(:searchText) OR "
                  "LOWER(dewey) LIKE LOWER(:searchText)");
    query.bindValue(":searchText", "%" + searchText + "%");

    if (query.exec()) {
        while (query.next()) {
            Item item;
            item.id = query.value("id").toInt();
            item.type = variantToItemType(query.value("type"));
            item.title = query.value("title").toString();
            item.creator = query.value("creator").toString();
            item.status = variantToAvailability(query.value("status"));
            item.borrowerId = query.value("borrowerId").toInt();
            item.due = QDate::fromString(query.value("due").toString(), Qt::ISODate);
            item.dewey = query.value("dewey").toString();
            item.issue = query.value("issue").toString();
            item.pub = QDate::fromString(query.value("pub").toString(), Qt::ISODate);
            item.genre = query.value("genre").toString();
            item.rating = query.value("rating").toString();
            item.holdQueue = stringToQListInt(query.value("holdQueue").toString());
            items.append(item);
        }
    } else {
        qDebug() << "Error finding items:" << query.lastError().text();
    }
    return items;
}

bool DatabaseManager::addUser(User &user) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (name, type, loans, holds) VALUES (:name, :type, :loans, :holds)");
    query.bindValue(":name", user.name);
    query.bindValue(":type", userTypeToVariant(user.type));
    query.bindValue(":loans", qListIntToString(user.loans));
    query.bindValue(":holds", qListIntToString(user.holds));

    if (!query.exec()) {
        qDebug() << "Error adding user:" << query.lastError().text();
        return false;
    }
    user.id = query.lastInsertId().toInt(); // Set the ID for the passed user
    return true;
}

bool DatabaseManager::updateUser(const User &user) {
    QSqlQuery query;
    query.prepare("UPDATE users SET name = :name, type = :type, loans = :loans, holds = :holds WHERE id = :id");
    query.bindValue(":id", user.id);
    query.bindValue(":name", user.name);
    query.bindValue(":type", userTypeToVariant(user.type));
    query.bindValue(":loans", qListIntToString(user.loans));
    query.bindValue(":holds", qListIntToString(user.holds));

    if (!query.exec()) {
        qDebug() << "Error updating user:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::removeUser(int userId) {
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    if (!query.exec()) {
        qDebug() << "Error removing user:" << query.lastError().text();
        return false;
    }
    return true;
}

User DatabaseManager::getUserById(int userId) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE id = :id");
    query.bindValue(":id", userId);
    User user;
    if (query.exec() && query.next()) {
        user.id = query.value("id").toInt();
        user.name = query.value("name").toString();
        user.type = variantToUserType(query.value("type"));
        user.loans = stringToQListInt(query.value("loans").toString());
        user.holds = stringToQListInt(query.value("holds").toString());
    } else {
        qDebug() << "Error getting user by ID:" << query.lastError().text();
    }
    return user;
}

User DatabaseManager::getUserByName(const QString& name) {
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE LOWER(name) = LOWER(:name)");
    query.bindValue(":name", name);
    User user;
    if (query.exec() && query.next()) {
        user.id = query.value("id").toInt();
        user.name = query.value("name").toString();
        user.type = variantToUserType(query.value("type"));
        user.loans = stringToQListInt(query.value("loans").toString());
        user.holds = stringToQListInt(query.value("holds").toString());
    } else {
        qDebug() << "Error getting user by name:" << query.lastError().text();
    }
    return user;
}

QList<User> DatabaseManager::getAllUsers() {
    QList<User> users;
    QSqlQuery query("SELECT * FROM users");
    while (query.next()) {
        User user;
        user.id = query.value("id").toInt();
        user.name = query.value("name").toString();
        user.type = variantToUserType(query.value("type"));
        user.loans = stringToQListInt(query.value("loans").toString());
        user.holds = stringToQListInt(query.value("holds").toString());
        users.append(user);
    }
    return users;
}

bool DatabaseManager::clearAndSeedData() {
    QSqlQuery query;
    // Clear existing data
    if (!query.exec("DELETE FROM items")) {
        qDebug() << "Error clearing items table:" << query.lastError().text();
        return false;
    }
    if (!query.exec("DELETE FROM users")) {
        qDebug() << "Error clearing users table:" << query.lastError().text();
        return false;
    }

    // Reset auto-increment IDs
    if (!query.exec("DELETE FROM sqlite_sequence WHERE name='items'")) {
        qDebug() << "Error resetting item sequence:" << query.lastError().text();
        return false;
    }
    if (!query.exec("DELETE FROM sqlite_sequence WHERE name='users'")) {
        qDebug() << "Error resetting user sequence:" << query.lastError().text();
        return false;
    }

    // Seed data - This part will mimic the Catalogue::seedDefaultData logic
    int id = 100;

    auto mkFic = [&](const QString& t, const QString& a){
        Item it; it.id=id++; it.type=ItemType::Fiction; it.title=t; it.creator=a; addItem(it);
    };
    auto mkNF  = [&](const QString& t, const QString& a, const QString& ddc){
        Item it; it.id=id++; it.type=ItemType::NonFiction; it.title=t; it.creator=a; it.dewey=ddc; addItem(it);
    };
    auto mkMag = [&](const QString& t, const QString& pubr, const QString& issue, const QDate& pubd){
        Item it; it.id=id++; it.type=ItemType::Magazine; it.title=t; it.creator=pubr; it.issue=issue; it.pub=pubd; addItem(it);
    };
    auto mkMov = [&](const QString& t, const QString& dir, const QString& genre, const QString& rating){
        Item it; it.id=id++; it.type=ItemType::Movie; it.title=t; it.creator=dir; it.genre=genre; it.rating=rating; addItem(it);
    };
    auto mkGame = [&](const QString& t, const QString& studio, const QString& genre, const QString& rating){
        Item it; it.id=id++; it.type=ItemType::VideoGame; it.title=t; it.creator=studio; it.genre=genre; it.rating=rating; addItem(it);
    };

    // 5 Fiction
    mkFic("The Silent Forest", "A. Greenwood");
    mkFic("Echoes of Dawn", "M. Rivera");
    mkFic("Paper Moons", "L. Chen");
    mkFic("Winter's Edge", "K. Patel");
    mkFic("Embers in Rain", "J. Alvarez");

    // 5 Non-Fiction (with Dewey)
    mkNF("A Brief History of Numbers", "R. Kumar", "510.9");
    mkNF("Ocean Currents Explained", "T. Suzuki", "551.46");
    mkNF("Mindset and Learning", "C. Dweck", "153.8");
    mkNF("Photography Essentials", "H. Nguyen", "770");
    mkNF("Modern Architecture", "E. Rossi", "720");

    // 3 Magazines (issue + publication date)
    mkMag("TechToday", "TechToday Media", "Vol. 32, No. 7", QDate(2025, 6, 1));
    mkMag("Science Monthly", "SciPub", "Issue 418",        QDate(2025, 5, 15));
    mkMag("Art & Design", "ArtDesk", "Spring 2025",        QDate(2025, 3, 20));

    // 3 Movies (genre + rating)
    mkMov("Chasing Horizons", "N. Park", "Adventure", "PG-13");
    mkMov("Quiet Rooms",      "S. Okafor", "Thriller",  "R");
    mkMov("Midnight Sketches","R. Haddad","Drama",     "PG");

    // 4 Video Games (genre + rating)
    mkGame("Solar Drift",     "Orion Studios", "Racing",      "E");
    mkGame("Verdant Realms",  "Hearthware",    "RPG",         "T");
    mkGame("Circuit Siege",   "BitForge",      "Strategy",    "E10+");
    mkGame("Neon Courier",    "Delta North",   "Action",      "T");

    // 7 Users: 5 patrons, 1 librarian, 1 admin
    auto addUserLambda = [&](int id_, const QString& name, UserType t){
        User u; u.id=id_; u.name=name; u.type=t; addUser(u);
    };
    int uid = 1;
    addUserLambda(uid++, "Alice",  UserType::Patron);
    addUserLambda(uid++, "Bob",    UserType::Patron);
    addUserLambda(uid++, "Carmen", UserType::Patron);
    addUserLambda(uid++, "Diego",  UserType::Patron);
    addUserLambda(uid++, "Eva",    UserType::Patron);
    addUserLambda(uid++, "Liam",   UserType::Librarian);
    addUserLambda(uid++, "Sara",   UserType::Admin);

    return true;
}
