#include "item.h"

QString toString(ItemType t) {
    switch (t) {
        case ItemType::Fiction:    return "Fiction";
        case ItemType::NonFiction: return "Non-Fiction";
        case ItemType::Magazine:   return "Magazine";
        case ItemType::Movie:      return "Movie";
        case ItemType::VideoGame:  return "Video Game";
    }
    return "Unknown";
}

QString toString(Availability a) {
    return (a == Availability::Available) ? "Available" : "Checked out";
}
