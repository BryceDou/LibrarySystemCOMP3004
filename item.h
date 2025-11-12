#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QDate>
#include <QList>

enum class item_type { fiction, nonfiction, magazine, movie, videogame };
enum class availability { available, checked_out };

class item {
public:
    int id{};
    item_type type{item_type::fiction};
    QString title;
    QString creator;
    availability status{availability::available};
    QDate due;                      // valid only when status == checked_out

    // Optional, type-specific fields
    QString dewey;                  // for nonfiction
    QString issue;                  // for magazine
    QDate   pub;                    // for magazine
    QString genre;                  // for movie/game
    QString rating;                 // for movie/game

    QList<int> holdQueue;           // FIFO of patron IDs
    // add to your item class
    int borrowerId = -1;   // -1 = not checked out

};

#endif // ITEM_H
