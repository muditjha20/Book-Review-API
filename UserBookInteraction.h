#ifndef USERBOOKINTERACTION_H
#define USERBOOKINTERACTION_H

#include <string>
#include "User.h"
#include "Book.h"
#include <crow.h>

using namespace std;
using namespace crow;

class UserBookInteraction {
public:
    UserBookInteraction() {}
    UserBookInteraction(string id, User user, Book book)
        : interaction_ID(id), user(user), book(book) {}

    string getId() { return interaction_ID; }
    User getUser() { return user; }
    Book getBook() { return book; }

    void setId(string value) { interaction_ID = value;}
    void setUser(User value) { user = value; }
    void setBook(Book value) { book = value; }

protected:
    string interaction_ID;
    User user;
    Book book;
};

#endif
