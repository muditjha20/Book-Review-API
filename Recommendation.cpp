#include "Recommendation.h"
#include "User.h"
#include "Book.h"

extern map<string, Recommendation> recommendationMap;
extern map<string, User> userMap;
extern map<string, Book> bookMap;

json::wvalue convertRecommendationToJson(Recommendation rec) {
    json::wvalue j;
    j["id"] = rec.getId();
    j["user"] = convertUserToJson(rec.getUser());
    j["book"] = convertBookToJson(rec.getBook());
    return j;
}

// -- Search, Filter, Sort --

response searchRecommendations(string searchStr) {
    vector<Recommendation> found;
    string lowered = toLower(searchStr);

    map<string, Recommendation>::iterator it;
    for (it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        Recommendation r = it->second;
        Book b = r.getBook();
        User u = r.getUser();
        if (
            toLower(b.getTitle()).find(lowered) != string::npos ||
            toLower(b.getAuthor()).find(lowered) != string::npos ||
            toLower(u.getName()).find(lowered) != string::npos
        ) {
            found.push_back(r);
        }
    }

    json::wvalue json;
    for (unsigned int i = 0; i < found.size(); i++) {
        json[i] = convertRecommendationToJson(found[i]);
    }

    return response(json.dump());
}

response filterRecommendations(string key, string value) {
    vector<Recommendation> filtered;
    string lowered = toLower(value);

    map<string, Recommendation>::iterator it;
    for (it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        Recommendation r = it->second;
        Book b = r.getBook();
        User u = r.getUser();
        if ((key == "genre" && toLower(b.getGenre()) == lowered) ||
            (key == "author" && toLower(b.getAuthor()) == lowered) ||
            (key == "user" && toLower(u.getName()) == lowered)) {
            filtered.push_back(r);
        }
    }

    json::wvalue json;
    for (unsigned int i = 0; i < filtered.size(); i++) {
        json[i] = convertRecommendationToJson(filtered[i]);
    }

    return response(json.dump());
}

response sortRecommendations(string sortKey) {
    vector<pair<string, Recommendation> > items;

    map<string, Recommendation>::iterator it;
    for (it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        items.push_back(make_pair(it->first, it->second));
    }

    if (sortKey == "title") {
        sort(items.begin(), items.end(), [](pair<string, Recommendation>& a, pair<string, Recommendation>& b) {
            return a.second.getBook().getTitle() < b.second.getBook().getTitle();
        });
    } else if (sortKey == "user") {
        sort(items.begin(), items.end(), [](pair<string, Recommendation>& a, pair<string, Recommendation>& b) {
            return a.second.getUser().getName() < b.second.getUser().getName();
        });
    }

    json::wvalue json;
    for (unsigned int i = 0; i < items.size(); i++) {
        json[i] = convertRecommendationToJson(items[i].second);
    }

    return response(json.dump());
}

// -- CRUD --

response createRecommendation(request req) {
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

    Recommendation rec(id, userMap[userId], bookMap[bookId]);
    recommendationMap[id] = rec;

    return response(201, convertRecommendationToJson(rec).dump());
}

response readRecommendation(string id) {
    map<string, Recommendation>::iterator it = recommendationMap.find(id);
    if (it != recommendationMap.end()) {
        return response(convertRecommendationToJson(it->second).dump());
    }
    return response(404, "Recommendation not found");
}

response readAllRecommendations(request req) {
    char* searchParam = req.url_params.get("search");
    char* sortParam = req.url_params.get("sort");
    char* filterKey = req.url_params.get("filterKey");
    char* filterValue = req.url_params.get("filterValue");

    if (searchParam) {
        return searchRecommendations(string(searchParam));
    }
    if (sortParam) {
        return sortRecommendations(string(sortParam));
    }
    if (filterKey && filterValue) {
        return filterRecommendations(string(filterKey), string(filterValue));
    }

    json::wvalue json;
    int index = 0;
    map<string, Recommendation>::iterator it;
    for (it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        json[index++] = convertRecommendationToJson(it->second);
    }

    return response(json.dump());
}

void updateRecommendation(request req, response& res, string id) {
    map<string, Recommendation>::iterator it = recommendationMap.find(id);
    if (it == recommendationMap.end()) {
        res.code = 404;
        res.end("Recommendation not found");
        return;
    }

    json::rvalue body = json::load(req.body);
    if (!body) {
        res.code = 400;
        res.end("Invalid JSON");
        return;
    }

    Recommendation& rec = it->second;

    if (body.has("user")) {
        string userId = body["user"]["id"].s();
        if (userMap.find(userId) != userMap.end()) {
            rec.setUser(userMap[userId]);
        }
    }

    if (body.has("book")) {
        string bookId = body["book"]["id"].s();
        if (bookMap.find(bookId) != bookMap.end()) {
            rec.setBook(bookMap[bookId]);
        }
    }

    res.code = 200;
    res.set_header("Content-Type", "application/json");
    res.write(convertRecommendationToJson(rec).dump());
    res.end();
}

response deleteRecommendation(string id) {
    map<string, Recommendation>::iterator it = recommendationMap.find(id);
    if (it != recommendationMap.end()) {
        recommendationMap.erase(it);
        return response(204);
    }
    return response(404, "Recommendation not found");
}

void saveRecommendationToFile(map<string, Recommendation> data, string filename) {
    ofstream file(filename);
    if (file.is_open()) {
        json::wvalue json;
        int index = 0;
        map<string, Recommendation>::iterator it;
        for (it = data.begin(); it != data.end(); ++it) {
            json[index++] = convertRecommendationToJson(it->second);
        }
        file << json.dump();
        file.close();
    }
}

map<string, Recommendation> loadRecommendationFromFile(string filename) {
    map<string, Recommendation> data;
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

            // Construct Recommendation
            Recommendation recommendation(id, user, book);
            data[recommendation.getId()] = recommendation;
        }
    }

    return data;
}

