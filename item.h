#ifndef ITEM_H
#define ITEM_H

#include <QString>
#include <QDate>
#include <QList>

enum class ItemType { Fiction, NonFiction, Magazine, Movie, VideoGame };
enum class Availability { Available, CheckedOut };

struct Item {
    int id = -1;
    ItemType type = ItemType::Fiction;
    QString title;
    QString creator;          // author/director/studio/etc.
    Availability status = Availability::Available;
    int borrowerId = -1;      // -1 = none
    QDate due;                // valid only when checked out

    // Type-specific optional fields (used per requirements)
    QString dewey;            // Non-fiction
    QString issue;            // Magazine issue number
    QDate   pub;              // Magazine publication date
    QString genre;            // Movie / VideoGame
    QString rating;           // Movie / VideoGame (e.g., PG-13, M)

    QList<int> holdQueue;     // FIFO queue of user IDs
};

QString toString(ItemType t);
QString toString(Availability a);

#endif // ITEM_H
