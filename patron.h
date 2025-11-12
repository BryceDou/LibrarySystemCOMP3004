#ifndef PATRON_H
#define PATRON_H

#include <QString>
#include <QList>

class patron {
public:
    int id{};
    QString name;
    QList<int> loans;   // item IDs
    QList<int> holds;   // item IDs
};

#endif // PATRON_H
