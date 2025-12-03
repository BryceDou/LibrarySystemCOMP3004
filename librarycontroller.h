#ifndef LIBRARYCONTROLLER_H
#define LIBRARYCONTROLLER_H

#include <QString>
#include "item.h"
#include "user.h"

class Catalogue;

struct Result {
    bool ok;
    QString message;
    int aux; // optional (e.g., queue position)
    Result(bool o=false, const QString& m=QString(), int a=0) : ok(o), message(m), aux(a) {}
};

class LibraryController {
public:
    explicit LibraryController(Catalogue* cat);

    // --- Queries (no mutation) ---
    Result canBorrow(int userId, int itemId) const;
    Result canReturn(int userId, int itemId) const;
    Result canPlaceHold(int userId, int itemId) const;
    Result canCancelHold(int userId, int itemId) const;
    int    queuePosition(int userId, int itemId) const;

    // --- Commands (mutate state) ---
    Result borrow(int userId, int itemId);      
    Result returnItem(int userId, int itemId);
    Result placeHold(int userId, int itemId);   
    Result cancelHold(int userId, int itemId);
    Item findItem(int id) const;   
    User findUser(int id) const; 

private:  

    static const int kMaxLoans = 3;
    Catalogue* cat_;
};

#endif // LIBRARYCONTROLLER_H
