#include "catalogue.h"
#include <QDate>

static QString ci(const QString& s) { return s.trimmed().toLower(); }

Item* Catalogue::findItem(int id) {
    for (auto &it : items) if (it.id == id) return &it;
    return nullptr;
}
const Item* Catalogue::findItem(int id) const {
    for (auto const &it : items) if (it.id == id) return &it;
    return nullptr;
}
User* Catalogue::findUserById(int id) {
    for (auto &u : users) if (u.id == id) return &u;
    return nullptr;
}
const User* Catalogue::findUserById(int id) const {
    for (auto const &u : users) if (u.id == id) return &u;
    return nullptr;
}
User* Catalogue::findUserByName(const QString& name) {
    const QString needle = ci(name);
    for (auto &u : users) if (ci(u.name) == needle) return &u;
    return nullptr;
}

void Catalogue::seedDefaultData() {
    items.clear(); users.clear();

    int id = 100;

    auto mkFic = [&](const QString& t, const QString& a){
        Item it; it.id=id++; it.type=ItemType::Fiction; it.title=t; it.creator=a; items.push_back(it);
    };
    auto mkNF  = [&](const QString& t, const QString& a, const QString& ddc){
        Item it; it.id=id++; it.type=ItemType::NonFiction; it.title=t; it.creator=a; it.dewey=ddc; items.push_back(it);
    };
    auto mkMag = [&](const QString& t, const QString& pubr, const QString& issue, const QDate& pubd){
        Item it; it.id=id++; it.type=ItemType::Magazine; it.title=t; it.creator=pubr; it.issue=issue; it.pub=pubd; items.push_back(it);
    };
    auto mkMov = [&](const QString& t, const QString& dir, const QString& genre, const QString& rating){
        Item it; it.id=id++; it.type=ItemType::Movie; it.title=t; it.creator=dir; it.genre=genre; it.rating=rating; items.push_back(it);
    };
    auto mkGame = [&](const QString& t, const QString& studio, const QString& genre, const QString& rating){
        Item it; it.id=id++; it.type=ItemType::VideoGame; it.title=t; it.creator=studio; it.genre=genre; it.rating=rating; items.push_back(it);
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
    auto addUser = [&](int id_, const QString& name, UserType t){
        User u; u.id=id_; u.name=name; u.type=t; users.push_back(u);
    };
    int uid = 1;
    addUser(uid++, "Alice",  UserType::Patron);
    addUser(uid++, "Bob",    UserType::Patron);
    addUser(uid++, "Carmen", UserType::Patron);
    addUser(uid++, "Diego",  UserType::Patron);
    addUser(uid++, "Eva",    UserType::Patron);
    addUser(uid++, "Liam",   UserType::Librarian);
    addUser(uid++, "Sara",   UserType::Admin);
}
