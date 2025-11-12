#include "catalogue.h"
#include <QDate>

item* catalogue::find_item(int id) {
    for (auto &it : items)
        if (it.id == id) return &it;
    return nullptr;
}

patron* catalogue::find_patron_by_name(const QString& name) {
    for (auto &p : patrons)
        if (p.name == name) return &p;
    return nullptr;
}

void catalogue::seed_default_data() {
    items.clear();
    patrons.clear();

    // Helper lambdas to keep the seeding code concise
    auto add_patron = [this](int id, const QString& name) {
        patron p; p.id = id; p.name = name; patrons.append(p);
    };
    auto add_fiction = [this](int id, const QString& title, const QString& author) {
        item it; it.id = id; it.type = item_type::fiction; it.title = title; it.creator = author; items.append(it);
    };
    auto add_nonfiction = [this](int id, const QString& title, const QString& author, const QString& dewey) {
        item it; it.id = id; it.type = item_type::nonfiction; it.title = title; it.creator = author; it.dewey = dewey; items.append(it);
    };
    auto add_magazine = [this](int id, const QString& title, const QString& issue, const QDate& pub) {
        item it; it.id = id; it.type = item_type::magazine; it.title = title; it.creator = "Editorial";
        it.issue = issue; it.pub = pub; items.append(it);
    };
    auto add_movie = [this](int id, const QString& title, const QString& director, const QString& genre, const QString& rating) {
        item it; it.id = id; it.type = item_type::movie; it.title = title; it.creator = director; it.genre = genre; it.rating = rating; items.append(it);
    };
    auto add_game = [this](int id, const QString& title, const QString& studio, const QString& genre, const QString& rating) {
        item it; it.id = id; it.type = item_type::videogame; it.title = title; it.creator = studio; it.genre = genre; it.rating = rating; items.append(it);
    };

    // Users: 5 patrons + librarian + admin (use names to route UI roles)
    add_patron(1, "Alice");
    add_patron(2, "Bob");
    add_patron(3, "Carol");
    add_patron(4, "Dave");
    add_patron(5, "Eve");
    add_patron(100, "Librarian");
    add_patron(101, "Admin");

    // Items: 5 fiction
    add_fiction(101, "The River",       "J. Hill");
    add_fiction(102, "Night Wind",      "S. Li");
    add_fiction(103, "Autumn Letters",  "M. Patel");
    add_fiction(104, "Glass Garden",    "R. Gomez");
    add_fiction(105, "Silent Harbor",   "K. Wu");

    // 5 nonfiction (with Dewey)
    add_nonfiction(201, "Deep Space",         "A. Chen",   "520.10");
    add_nonfiction(202, "Human Body Basics",  "N. Singh",  "612.00");
    add_nonfiction(203, "Modern Economics",   "T. Adams",  "330.01");
    add_nonfiction(204, "Wildlife of Sahara", "B. Moore",  "599.74");
    add_nonfiction(205, "Intro to AI",        "L. Turner", "006.30");

    // 3 magazines (issue + publication date)
    add_magazine(301, "Tech Monthly",  "Vol. 58 No. 3", QDate(2025,3,1));
    add_magazine(302, "Health Weekly", "Issue 742",     QDate(2025,2,15));
    add_magazine(303, "City Review",   "Mar 2025",      QDate(2025,3,5));

    // 3 movies (genre + rating)
    add_movie(401, "Stars Awake",   "Director Q", "Sci-Fi",   "PG-13");
    add_movie(402, "Broken Ties",   "I. Novak",   "Drama",    "R");
    add_movie(403, "Hidden Trails", "C. Rivera",  "Thriller", "PG-13");

    // 4 video games (genre + rating)
    add_game(501, "Sky Quest",      "Studio X",  "RPG",          "E10+");
    add_game(502, "Circuit Racer",  "Pulsar",    "Racing",       "E");
    add_game(503, "Dungeon Echoes", "TriForge",  "Action RPG",   "T");
    add_game(504, "Harbor Builder", "BrickSoft", "Simulation",   "E");
}
