#include "librarycontroller.h"
#include "catalogue.h"
#include "databasemanager.h"
#include <QDate>
#include <QDebug>

LibraryController::LibraryController(Catalogue* cat) : cat_(cat) {}

// ---------------------- Queries ----------------------
Item LibraryController::findItem(int id) const {
    for (const auto& it : cat_->getAllItems()) {
        if (it.id == id) return it;
    }
    return Item();
}

User LibraryController::findUser(int id) const {
    return cat_ ? cat_->getUserById(id) : User();
}

Result LibraryController::canBorrow(int userId, int itemId) const {
    Item it = findItem(itemId);
    User u = findUser(userId);

    if (it.id == 0 || u.id == 0) return Result(false, "Invalid selection.");
    if (it.status != Availability::Available) return Result(false, "Item is not available.");
    if (u.loans.size() >= kMaxLoans) return Result(false, "Maximum of 3 active loans reached.");
    if (!it.holdQueue.isEmpty() && it.holdQueue.first() != userId)
        return Result(false, "Another patron is first in the hold queue.");
    return Result(true);
}

Result LibraryController::canReturn(int userId, int itemId) const {
    Item it = findItem(itemId);
    if (it.id == 0) return Result(false, "Invalid selection.");
    if (it.status == Availability::Available) return Result(false, "Item is already available.");
    if (it.borrowerId != userId) return Result(false, "You can only return items you borrowed.");
    return Result(true);
}

Result LibraryController::canPlaceHold(int userId, int itemId) const {
    Item it = findItem(itemId);
    if (it.id == 0) return Result(false, "Invalid selection.");
    if (it.status != Availability::CheckedOut) return Result(false, "Holds allowed only on checked-out items.");
    if (it.holdQueue.contains(userId)) return Result(false, "You are already in the hold queue.");
    return Result(true);
}

Result LibraryController::canCancelHold(int userId, int itemId) const {
    Item it = findItem(itemId);
    if (it.id == 0) return Result(false, "Invalid selection.");
    if (!it.holdQueue.contains(userId)) return Result(false, "You don't have a hold on this item.");
    return Result(true);
}

int LibraryController::queuePosition(int userId, int itemId) const {
    Item it = findItem(itemId);
    if (it.id == 0) return -1;
    int idx = it.holdQueue.indexOf(userId);
    return (idx >= 0) ? (idx + 1) : -1;
}

// ---------------------- Commands ----------------------
Result LibraryController::borrow(int userId, int itemId) {
    Result chk = canBorrow(userId, itemId);
    if (!chk.ok) return chk;

    Item it = findItem(itemId);  // make a copy to modify
    User u = findUser(userId);

    it.status = Availability::CheckedOut;
    it.borrowerId = userId;
    it.due = QDate::currentDate().addDays(14);
    u.loans.append(it.id);

    if (!it.holdQueue.isEmpty() && it.holdQueue.first() == userId) {
        it.holdQueue.pop_front();
        u.holds.removeAll(it.id);
    }

    DatabaseManager::instance().updateItem(it);
    DatabaseManager::instance().updateUser(u);

    return Result(true, "Borrowed.");
}

Result LibraryController::returnItem(int userId, int itemId) {
    Result chk = canReturn(userId, itemId);
    if (!chk.ok) return chk;

    Item it = findItem(itemId);
    User u = findUser(userId);

    it.status = Availability::Available;
    it.borrowerId = -1;
    it.due = QDate();
    u.loans.removeAll(it.id);

    DatabaseManager::instance().updateItem(it);
    DatabaseManager::instance().updateUser(u);

    return Result(true, "Returned.");
}

Result LibraryController::placeHold(int userId, int itemId) {
    Result chk = canPlaceHold(userId, itemId);
    if (!chk.ok) return chk;

    Item it = findItem(itemId);
    User u = findUser(userId);

    it.holdQueue.append(userId);
    u.holds.append(it.id);

    DatabaseManager::instance().updateItem(it);
    DatabaseManager::instance().updateUser(u);

    int pos = it.holdQueue.size();
    return Result(true, QString("Hold placed. You are #%1.").arg(pos), pos);
}

Result LibraryController::cancelHold(int userId, int itemId) {
    Result chk = canCancelHold(userId, itemId);
    if (!chk.ok) return chk;

    Item it = findItem(itemId);
    User u = findUser(userId);

    it.holdQueue.removeAll(userId);
    u.holds.removeAll(it.id);

    DatabaseManager::instance().updateItem(it);
    DatabaseManager::instance().updateUser(u);

    return Result(true, "Hold canceled.");
}
