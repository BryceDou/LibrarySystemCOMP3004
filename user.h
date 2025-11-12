#ifndef USER_H
#define USER_H

#include <QString>
#include <QList>

enum class UserType { Patron, Librarian, Admin };

class User {
public:
    int id = -1;
    QString name;
    UserType type = UserType::Patron;
    QList<int> loans;   // item IDs
    QList<int> holds;   // item IDs

    // Keep 'why': guard against duplicates to keep UI consistent.
    void addLoan(int itemId)        { if (!loans.contains(itemId)) loans.append(itemId); }
    bool removeLoan(int itemId)     { return loans.removeAll(itemId) > 0; }
    bool hasLoan(int itemId)  const { return loans.contains(itemId); }

    void addHold(int itemId)        { if (!holds.contains(itemId)) holds.append(itemId); }
    bool removeHold(int itemId)     { return holds.removeAll(itemId) > 0; }
    bool hasHold(int itemId)  const { return holds.contains(itemId); }
};

QString toString(UserType t);

#endif // USER_H
