#include "Review.h"
#include "User.h"
#include "Book.h"

extern map<string, Review> reviewMap;

// Assume these are defined elsewhere
extern map<string, User> userMap;
extern map<string, Book> bookMap;

json::wvalue convertReviewToJson(Review& review) {
    json::wvalue j;
    j["id"] = review.getId();
    j["user"] = convertUserToJson(review.getUser());
    j["book"] = convertBookToJson(review.getBook());
    j["rating"] = review.getRating();
    j["comment"] = review.getComment();
    return j;
}

// -- Search, Filter, Sort --

response searchReviews(string searchStr) {
    vector<Review> foundReviews;
    string loweredSearch = toLower(searchStr);

    map<string, Review>::iterator it;
    for (it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        Review r = it->second;
        Book b = r.getBook();
        User u = r.getUser();
        if (
            toLower(b.getTitle()).find(loweredSearch) != string::npos ||
            toLower(b.getAuthor()).find(loweredSearch) != string::npos ||
            toLower(u.getName()).find(loweredSearch) != string::npos ||
            toLower(r.getComment()).find(loweredSearch) != string::npos
        ) {
            foundReviews.push_back(r);
        }
    }

    json::wvalue json;
    for (unsigned int i = 0; i < foundReviews.size(); i++) {
        json[i] = convertReviewToJson(foundReviews[i]);
    }

    return response(json.dump());
}

response filterReviews(string key, string value) {
    vector<Review> filtered;
    string loweredVal = toLower(value);

    map<string, Review>::iterator it;
    for (it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        Review r = it->second;
        Book b = r.getBook();
        User u = r.getUser();
        if ((key == "genre" && toLower(b.getGenre()) == loweredVal) ||
            (key == "author" && toLower(b.getAuthor()) == loweredVal) ||
            (key == "user" && toLower(u.getName()) == loweredVal)) {
            filtered.push_back(r);
        }
    }

    json::wvalue json;
    for (unsigned int i = 0; i < filtered.size(); i++) {
        json[i] = convertReviewToJson(filtered[i]);
    }

    return response(json.dump());
}

response sortReviews(string sortKey) {
    vector<pair<string, Review> > sortedItems;

    map<string, Review>::iterator it;
    for (it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        sortedItems.push_back(make_pair(it->first, it->second));
    }

    if (sortKey == "rating") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Review>& a, pair<string, Review>& b) {
            return a.second.getRating() > b.second.getRating();
        });
    } else if (sortKey == "title") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Review>& a, pair<string, Review>& b) {
            return a.second.getBook().getTitle() < b.second.getBook().getTitle();
        });
    } else if (sortKey == "user") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, Review>& a, pair<string, Review>& b) {
            return a.second.getUser().getName() < b.second.getUser().getName();
        });
    }

    json::wvalue json;
    for (unsigned int i = 0; i < sortedItems.size(); i++) {
        json[i] = convertReviewToJson(sortedItems[i].second);
    }

    return response(json.dump());
}

// -- CRUD --

response createReview(request req) {
    json::rvalue body = json::load(req.body);
    if (!body) {
        return response(400, "Invalid JSON");
    }

    string id = body["id"].s();
    string userId = body["user"]["id"].s();
    string bookId = body["book"]["id"].s();

    if (userMap.find(userId) == userMap.end()) {
        return response(404, "User not found");
    }
    if (bookMap.find(bookId) == bookMap.end()) {
        return response(404, "Book not found");
    }

    int rating = body["rating"].i();
    string comment = body["comment"].s();

    Review review(id, userMap[userId], bookMap[bookId], rating, comment);
    reviewMap[id] = review;

    return response(201, convertReviewToJson(review).dump());
}

response readReview(string id) {
    map<string, Review>::iterator it = reviewMap.find(id);
    if (it != reviewMap.end()) {
        return response(convertReviewToJson(it->second).dump());
    }
    return response(404, "Review not found");
}

response readAllReviews(request req) {
    char* searchParam = req.url_params.get("search");
    char* sortParam = req.url_params.get("sort");
    char* filterKey = req.url_params.get("filterKey");
    char* filterValue = req.url_params.get("filterValue");

    if (searchParam) {
        return searchReviews(string(searchParam));
    }
    if (sortParam) {
        return sortReviews(string(sortParam));
    }
    if (filterKey && filterValue) {
        return filterReviews(string(filterKey), string(filterValue));
    }

    json::wvalue json;
    int index = 0;
    map<string, Review>::iterator it;
    for (it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        json[index++] = convertReviewToJson(it->second);
    }

    return response(json.dump());
}

void updateReview(request req, response& res, string id) {
    map<string, Review>::iterator it = reviewMap.find(id);
    if (it == reviewMap.end()) {
        res.code = 404;
        res.end("Review not found");
        return;
    }

    json::rvalue body = json::load(req.body);
    if (!body) {
        res.code = 400;
        res.end("Invalid JSON");
        return;
    }

    Review& review = it->second;

    if (body.has("user")) {
        string userId = body["user"]["id"].s();
        if (userMap.find(userId) != userMap.end()) {
            review.setUser(userMap[userId]);
        }
    }

    if (body.has("book")) {
        string bookId = body["book"]["id"].s();
        if (bookMap.find(bookId) != bookMap.end()) {
            review.setBook(bookMap[bookId]);
        }
    }

    if (body.has("rating")) {
        review.setRating(body["rating"].i());
    }

    if (body.has("comment")) {
        review.setComment(body["comment"].s());
    }

    res.code = 200;
    res.set_header("Content-Type", "application/json");
    res.write(convertReviewToJson(review).dump());
    res.end();
}

response deleteReview(string id) {
    map<string, Review>::iterator it = reviewMap.find(id);
    if (it != reviewMap.end()) {
        reviewMap.erase(it);
        return response(204);
    }
    return response(404, "Review not found");
}

void saveReviewToFile(map<string, Review> data, string filename) {
    ofstream file(filename);
    if (file.is_open()) {
        json::wvalue json;
        int index = 0;
        map<string, Review>::iterator it;
        for (it = data.begin(); it != data.end(); ++it) {
            json[index++] = convertReviewToJson(it->second);
        }
        file << json.dump();
        file.close();
    }
}

map<string, Review> loadReviewFromFile(string filename) {
    map<string, Review> data;
    ifstream file(filename);

    if (file.is_open()) {
        ostringstream ss;
        ss << file.rdbuf();
        file.close();

        json::rvalue json = json::load(ss.str());

        for (json::rvalue& item : json) {
            string id = item["id"].s();

            // Deserialize User
            string userId = item["user"]["id"].s();
            string name = item["user"]["name"].s();
            string email = item["user"]["email"].s();
            vector<string> preferences;
            for (json::rvalue& pref : item["user"]["preferences"]) {
                preferences.push_back(pref.s());
            }
            User user(userId, name, email, preferences);

            // Deserialize Book
            string bookId = item["book"]["id"].s();
            string title = item["book"]["title"].s();
            string author = item["book"]["author"].s();
            string genre = item["book"]["genre"].s();
            string isbn = item["book"]["isbn"].s();
            Book book(bookId, title, author, genre, isbn);

            // Deserialize Review data
            int rating = item["rating"].i();
            string comment = item["comment"].s();

            Review review(id, user, book, rating, comment);
            data[review.getId()] = review;
        }
    }

    return data;
}

