#ifndef PATRON_H
#define PATRON_H

#include <QString>
#include <QList>

/**
 * Lightweight patron record.
 * - loans: list of item IDs currently borrowed by this patron
 * - holds: list of item IDs this patron has placed on hold (queue lives on item)
 */
class patron {
public:
    int id{};            // unique user id (e.g., 1..5)
    QString name;        // display name (e.g., "Alice")
    QList<int> loans;    // active loan item IDs
    QList<int> holds;    // active hold item IDs

    // --- convenience helpers ---

    // Borrowing rule: at most 'maxLoans' concurrent loans (default: 3)
    bool canBorrow(int maxLoans = 3) const { return loans.size() < maxLoans; }

    // Loans
    void addLoan(int itemId)          { if (!loans.contains(itemId)) loans.append(itemId); }
    bool removeLoan(int itemId)       { return loans.removeAll(itemId) > 0; }
    bool hasLoan(int itemId) const    { return loans.contains(itemId); }

    // Holds
    void addHold(int itemId)          { if (!holds.contains(itemId)) holds.append(itemId); }
    bool removeHold(int itemId)       { return holds.removeAll(itemId) > 0; }
    bool hasHold(int itemId) const    { return holds.contains(itemId); }

    // Clear session state if needed (e.g., on logout, if you want a hard reset)
    void clearAll()                   { loans.clear(); holds.clear(); }
};

#endif // PATRON_H
