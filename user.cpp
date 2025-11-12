#include "user.h"

QString toString(UserType t) {
    switch (t) {
        case UserType::Patron:    return "Patron";
        case UserType::Librarian: return "Librarian";
        case UserType::Admin:     return "System Administrator";
    }
    return "User";
}
