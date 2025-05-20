#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>
#include <crow.h>

using namespace std;
using namespace crow;

class Book {
public:
    Book() {}
    Book(string id, string title, string author, string genre, string isbn)
        : id(id), title(title), author(author), genre(genre), isbn(isbn) {}

    string getId() { return id; }
    string getTitle() { return title; }
    string getAuthor() { return author; }
    string getGenre() { return genre; }
    string getIsbn() { return isbn; }

    void setTitle(string value) { title = value; }
    void setAuthor(string value) { author = value; }
    void setGenre(string value) { genre = value; }
    void setIsbn(string value) { isbn = value; }

private:
    string id;
    string title;
    string author;
    string genre;
    string isbn;
};

// Helpers
json::wvalue convertBookToJson(Book book);
string toLower(string input);

// CRUD Handlers
response createBook(request req);
response readBook(string id);
response readAllBooks(request req);
void updateBook(request req, response& res, string id);
response deleteBook(string id);

void saveBookToFile(map<string, Book> data, string filename);
map<string, Book> loadBookFromFile(string filename);

#endif
