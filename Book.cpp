#include "User.h"
#include "Book.h"
#include "Recommendation.h"
#include "Review.h"

extern map<string, User> userMap;
extern map<string, Recommendation> recommendationMap;
extern map<string, Book> bookMap;
extern map<string, Review> reviewMap;

json::wvalue convertBookToJson(Book book) {
    json::wvalue j;
    j["id"] = book.getId();
    j["title"] = book.getTitle();
    j["author"] = book.getAuthor();
    j["genre"] = book.getGenre();
    j["isbn"] = book.getIsbn();
    return j;
}

string toLower(string input) {
    for (unsigned int i = 0; i < input.length(); i++) {
        input[i] = tolower(input[i]);
    }
    return input;
}

// Template helper method to remove entries associated with a specific book
template <typename T>
void removeEntriesWithBook(map<string, T>& m, const string& bookId) {
    map<string, T> updatedMap;
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->second.getBook().getId() != bookId) {
            updatedMap[it->first] = it->second;
        }
    }
    m = updatedMap;
}

response searchBooks(string searchStr) {
    vector<Book> foundBooks;
    string loweredSearch = toLower(searchStr);

    map<string, Book>::iterator it;
    for (it = bookMap.begin(); it != bookMap.end(); ++it) {
        Book b = it->second;
        if (toLower(b.getTitle()).find(loweredSearch) != string::npos ||
            toLower(b.getAuthor()).find(loweredSearch) != string::npos ||
            toLower(b.getGenre()).find(loweredSearch) != string::npos ||
            toLower(b.getIsbn()).find(loweredSearch) != string::npos) {
            foundBooks.push_back(b);
        }
    }

    json::wvalue json;
    int index = 0;
    for (unsigned int i = 0; i < foundBooks.size(); i++) {
        json[index++] = convertBookToJson(foundBooks[i]);
    }

    return response(json.dump());
}

response sortBooks(string sortKey) {
    vector< pair<string, Book> > sortedItems;

    map<string, Book>::iterator it;
    for (it = bookMap.begin(); it != bookMap.end(); ++it) {
        sortedItems.push_back(make_pair(it->first, it->second));
    }

    if (sortKey == "title") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Book> a, pair<string, Book> b) {
            return a.second.getTitle() < b.second.getTitle();
        });
    } else if (sortKey == "author") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Book> a, pair<string, Book> b) {
            return a.second.getAuthor() < b.second.getAuthor();
        });
    } else if (sortKey == "genre") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Book> a, pair<string, Book> b) {
            return a.second.getGenre() < b.second.getGenre();
        });
    } else if (sortKey == "isbn") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Book> a, pair<string, Book> b) {
            return a.second.getIsbn() < b.second.getIsbn();
        });
    }

    json::wvalue json;
    int index = 0;
    for (unsigned int i = 0; i < sortedItems.size(); i++) {
        json[index++] = convertBookToJson(sortedItems[i].second);
    }

    return response(json.dump());
}

response filterBooks(string key, string value) {
    vector<Book> filteredBooks;
    string loweredVal = toLower(value);

    map<string, Book>::iterator it;
    for (it = bookMap.begin(); it != bookMap.end(); ++it) {
        Book b = it->second;
        if ((key == "genre" && toLower(b.getGenre()) == loweredVal) ||
            (key == "author" && toLower(b.getAuthor()) == loweredVal)) {
            filteredBooks.push_back(b);
        }
    }

    json::wvalue json;
    int index = 0;
    for (unsigned int i = 0; i < filteredBooks.size(); i++) {
        json[index++] = convertBookToJson(filteredBooks[i]);
    }

    return response(json.dump());
}

response createBook(request req) {
    json::rvalue body = json::load(req.body);
    if (!body) {
        return response(400, "Invalid JSON");
    }

    string id = body["id"].s();

    Book book(body["id"].s(), body["title"].s(), body["author"].s(), body["genre"].s(), body["isbn"].s());
    bookMap[id] = book;

    // Step 1: Update existing Reviews that reference this Book
    for (auto it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        if (it->second.getBook().getId() == id) {
            it->second.setBook(book); // Reassign the updated book to existing review
        }
    }

    // Step 2: Remove all Recommendations associated with this Book
    removeEntriesWithBook(recommendationMap, id);

    // Step 3: Rebuild Recommendations
    vector<Recommendation> keptRecs;
    for (auto it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        keptRecs.push_back(it->second);
    }

    // Add new Recommendations based on User preferences
    for (auto it = userMap.begin(); it != userMap.end(); ++it) {
        User user = it->second;
        vector<string> preferences = user.getPreferences();
        for (int i = 0; i < (int)preferences.size(); i++) {
            if (book.getGenre() == preferences[i]) {
                Recommendation newRec("", user, book); // ID to be set during reindexing
                keptRecs.push_back(newRec);
                break;
            }
        }
    }

    // Step 4: Reindex all Recommendations
    map<string, Recommendation> updatedRecommendationMap;
    for (int i = 0; i < (int)keptRecs.size(); ++i) {
        stringstream ss;
        ss << setfill('0') << setw(3) << (i + 1);
        string recId = ss.str();
        keptRecs[i].setId(recId);
        updatedRecommendationMap[recId] = keptRecs[i];
    }
    recommendationMap = updatedRecommendationMap;

    return response(201, convertBookToJson(book).dump());
}

response readBook(string id) {
    map<string, Book>::iterator it = bookMap.find(id);
    if (it != bookMap.end()) {
        Book book = it->second;
        return response(convertBookToJson(book).dump());
    }
    return response(404, "Book Not Found");
}

response readAllBooks(request req) {
    char* searchParam = req.url_params.get("search");
    char* sortParam = req.url_params.get("sort");
    char* filterKey = req.url_params.get("filterKey");
    char* filterValue = req.url_params.get("filterValue");

    if (searchParam) {
        return searchBooks(string(searchParam));
    }
    if (sortParam) {
        return sortBooks(string(sortParam));
    }
    if (filterKey && filterValue) {
        return filterBooks(string(filterKey), string(filterValue));
    }

    json::wvalue json;
    int index = 0;
    map<string, Book>::iterator it;
    for (it = bookMap.begin(); it != bookMap.end(); ++it) {
        json[index++] = convertBookToJson(it->second);
    }

    return response(json.dump());
}

void updateBook(request req, response& res, string id) {
    map<string, Book>::iterator it = bookMap.find(id);
    if (it == bookMap.end()) {
        res.code = 404;
        res.end("Book Not Found");
        return;
    }

    json::rvalue body = json::load(req.body);
    if (!body) {
        res.code = 400;
        res.end("Invalid JSON");
        return;
    }

    Book book = it->second;
    book.setTitle(body["title"].s());
    book.setAuthor(body["author"].s());
    book.setGenre(body["genre"].s());
    book.setIsbn(body["isbn"].s());

    bookMap[id] = book;

    // Step 1: Update existing Reviews that reference this Book
    for (auto it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        if (it->second.getBook().getId() == id) {
            it->second.setBook(book); // Reassign the updated book to existing review
        }
    }

    // Step 2: Remove all Recommendations associated with this Book
    removeEntriesWithBook(recommendationMap, id);

    // Step 3: Rebuild Recommendations
    vector<Recommendation> keptRecs;
    for (auto it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        keptRecs.push_back(it->second);
    }

    // Add new Recommendations based on User preferences
    for (auto it = userMap.begin(); it != userMap.end(); ++it) {
        User user = it->second;
        vector<string> preferences = user.getPreferences();
        for (int i = 0; i < (int)preferences.size(); i++) {
            if (book.getGenre() == preferences[i]) {
                Recommendation newRec("", user, book); // ID to be set during reindexing
                keptRecs.push_back(newRec);
                break;
            }
        }
    }

    // Step 4: Reindex all Recommendations
    map<string, Recommendation> updatedRecommendationMap;
    for (int i = 0; i < (int)keptRecs.size(); ++i) {
        stringstream ss;
        ss << setfill('0') << setw(3) << (i + 1);
        string recId = ss.str();
        keptRecs[i].setId(recId);
        updatedRecommendationMap[recId] = keptRecs[i];
    }
    recommendationMap = updatedRecommendationMap;

    res.code = 200;
    res.set_header("Content-Type", "application/json");
    res.write(convertBookToJson(book).dump());
    res.end();
}

// Updated deleteBook function
response deleteBook(string id) {
    map<string, Book>::iterator it = bookMap.find(id);
    if (it == bookMap.end()) {
        return response(404, "Book not found");
    }

    // Step 1: Erase the book
    bookMap.erase(it);

    // Step 2: Remove all reviews associated with the book
    removeEntriesWithBook(reviewMap, id);

    // Step 3: Remove all recommendations associated with the book
    removeEntriesWithBook(recommendationMap, id);

    // Step 4: Reindex remaining recommendations
    vector<Recommendation> keptRecs;
    for (auto it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        keptRecs.push_back(it->second);
    }

    map<string, Recommendation> reindexedMap;
    for (int i = 0; i < (int)keptRecs.size(); i++) {
        stringstream ss;
        ss << setfill('0') << setw(3) << (i + 1);
        string newId = ss.str();
        keptRecs[i].setId(newId);
        reindexedMap[newId] = keptRecs[i];
    }

    recommendationMap = reindexedMap;

    return response(204);
}


void saveBookToFile(map<string, Book> data, string filename) {
    ofstream file(filename);
    if (file.is_open()) {
        json::wvalue json;
        int index = 0;
        map<string, Book>::iterator it;
        for (it = data.begin(); it != data.end(); ++it) {
            json[index++] = convertBookToJson(it->second);
        }
        file << json.dump();
        file.close();
    }
}

map<string, Book> loadBookFromFile(string filename) {
    map<string, Book> data;
    ifstream file(filename);

    if (file.is_open()) {
        ostringstream ss;
        ss << file.rdbuf();
        file.close();

        json::rvalue json = json::load(ss.str());

        for (json::rvalue& item : json) {
            string id = item["id"].s();
            string title = item["title"].s();
            string author = item["author"].s();
            string genre = item["genre"].s();
            string isbn = item["isbn"].s();

            Book book(id, title, author, genre, isbn);
            data[book.getId()] = book;
        }
    }

    return data;
}
