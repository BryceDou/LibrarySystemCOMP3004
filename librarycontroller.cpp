#include "librarycontroller.h"
#include "catalogue.h"
#include "item.h"
#include "user.h"
#include <QDate>

LibraryController::LibraryController(Catalogue* cat) : cat_(cat) {}

Item* LibraryController::findItem(int id) const {
    return cat_ ? cat_->findItem(id) : 0;
}
User* LibraryController::findUser(int id) const {
    return cat_ ? cat_->findUserById(id) : 0;
}

// ---------------------- Queries ----------------------
Result LibraryController::canBorrow(int userId, int itemId) const {
    Item* it = findItem(itemId);
    User* u  = findUser(userId);
    if (!it || !u) return Result(false, "Invalid selection.");

    if (it->status != Availability::Available)
        return Result(false, "Item is not available.");

    if (u->loans.size() >= kMaxLoans)
        return Result(false, "Maximum of 3 active loans reached.");

    // If a queue exists, only first-in-line can check out.
    if (!it->holdQueue.isEmpty() && it->holdQueue.first() != userId)
        return Result(false, "Another patron is first in the hold queue.");

    return Result(true);
}

Result LibraryController::canReturn(int userId, int itemId) const {
    Item* it = findItem(itemId);
    if (!it) return Result(false, "Invalid selection.");

    if (it->status == Availability::Available)
        return Result(false, "Item is already available.");

    if (it->borrowerId != userId)
        return Result(false, "You can only return items you borrowed.");

    return Result(true);
}

Result LibraryController::canPlaceHold(int userId, int itemId) const {
    Item* it = findItem(itemId);
    if (!it) return Result(false, "Invalid selection.");

    if (it->status != Availability::CheckedOut)
        return Result(false, "Holds allowed only on checked-out items.");

    if (it->holdQueue.contains(userId))
        return Result(false, "You are already in the hold queue.");

    return Result(true);
}

Result LibraryController::canCancelHold(int userId, int itemId) const {
    Item* it = findItem(itemId);
    if (!it) return Result(false, "Invalid selection.");

    if (!it->holdQueue.contains(userId))
        return Result(false, "You don't have a hold on this item.");

    return Result(true);
}

int LibraryController::queuePosition(int userId, int itemId) const {
    Item* it = findItem(itemId);
    if (!it) return -1;
    int idx = it->holdQueue.indexOf(userId);
    return (idx >= 0) ? (idx + 1) : -1;
}

// ---------------------- Commands ----------------------
Result LibraryController::borrow(int userId, int itemId) {
    Result chk = canBorrow(userId, itemId);
    if (!chk.ok) return chk;

    Item* it = findItem(itemId);
    User* u  = findUser(userId);

    it->status = Availability::CheckedOut;
    it->borrowerId = userId;
    it->due = QDate::currentDate().addDays(14);
    u->addLoan(it->id);

    // If user was first in queue, pop & clear their hold record.
    if (!it->holdQueue.isEmpty() && it->holdQueue.first() == userId) {
        it->holdQueue.pop_front();
        u->removeHold(it->id);
    }
    return Result(true, "Borrowed.");
}

Result LibraryController::returnItem(int userId, int itemId) {
    Result chk = canReturn(userId, itemId);
    if (!chk.ok) return chk;

    Item* it = findItem(itemId);
    User* u  = findUser(userId);

    it->status = Availability::Available;
    it->borrowerId = -1;
    it->due = QDate();
    u->removeLoan(it->id);
    return Result(true, "Returned.");
}

Result LibraryController::placeHold(int userId, int itemId) {
    Result chk = canPlaceHold(userId, itemId);
    if (!chk.ok) return chk;

    Item* it = findItem(itemId);
    User* u  = findUser(userId);

    it->holdQueue.append(userId);
    u->addHold(it->id);
    int pos = it->holdQueue.size();
    return Result(true, QString("Hold placed. You are #%1.").arg(pos), pos);
}

Result LibraryController::cancelHold(int userId, int itemId) {
    Result chk = canCancelHold(userId, itemId);
    if (!chk.ok) return chk;

    Item* it = findItem(itemId);
    User* u  = findUser(userId);

    it->holdQueue.removeAll(userId);
    u->removeHold(it->id);
    return Result(true, "Hold canceled.");
}
