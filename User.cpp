#include "User.h"
#include "Book.h"
#include "Recommendation.h"
#include "Review.h"

extern map<string, User> userMap;
extern map<string, Recommendation> recommendationMap;
extern map<string, Book> bookMap;
extern map<string, Review> reviewMap;

json::wvalue convertUserToJson(User user) {
    json::wvalue j;
    j["id"] = user.getId();
    j["name"] = user.getName();
    j["email"] = user.getEmail();
    j["preferences"] = user.getPreferences();
    return j;
}

// Template helper method to remove entries associated with a specific user
template <typename T>
void removeEntriesWithUser(map<string, T>& m, const string& userId) {
    map<string, T> updatedMap;
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->second.getUser().getId() != userId) {
            updatedMap[it->first] = it->second;
        }
    }
    m = updatedMap;
}

response searchUsers(string searchStr) {
    vector<User> foundUsers;
    string loweredSearch = toLower(searchStr);

    map<string, User>::iterator it;
    for (it = userMap.begin(); it != userMap.end(); ++it) {
        User u = it->second;
        if (toLower(u.getName()).find(loweredSearch) != string::npos ||
            toLower(u.getEmail()).find(loweredSearch) != string::npos) {
            foundUsers.push_back(u);
        }
    }

    json::wvalue json;
    int index = 0;
    for (unsigned int i = 0; i < foundUsers.size(); i++) {
        json[index++] = convertUserToJson(foundUsers[i]);
    }

    return std::move(response(json.dump()));
}

response sortUsers(string sortKey) {
    vector<pair<string, User> > sortedItems;

    map<string, User>::iterator it;
    for (it = userMap.begin(); it != userMap.end(); ++it) {
        sortedItems.push_back(make_pair(it->first, it->second));
    }

    if (sortKey == "name") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, User>& a, pair<string, User>& b) {
            return a.second.getName() < b.second.getName();
        });
    } else if (sortKey == "email") {
        sort(sortedItems.begin(), sortedItems.end(), [](pair<string, User>& a, pair<string, User>& b) {
            return a.second.getEmail() < b.second.getEmail();
        });
    }

    json::wvalue json;
    int index = 0;
    for (unsigned int i = 0; i < sortedItems.size(); i++) {
        json[index++] = convertUserToJson(sortedItems[i].second);
    }

    return std::move(response(json.dump()));
}

response filterUsers(string key, string value) {
    vector<User> filteredUsers;
    string loweredVal = toLower(value);

    map<string, User>::iterator it;
    for (it = userMap.begin(); it != userMap.end(); ++it) {
        User u = it->second;
        if ((key == "email" && toLower(u.getEmail()) == loweredVal)) {
            filteredUsers.push_back(u);
        }
    }

    json::wvalue json;
    int index = 0;
    for (unsigned int i = 0; i < filteredUsers.size(); i++) {
        json[index++] = convertUserToJson(filteredUsers[i]);
    }

    return std::move(response(json.dump()));
}

response createUser(request req) {
    json::rvalue body = json::load(req.body);
    if (!body) {
        return std::move(response(400, "Invalid JSON"));
    }

    string id = body["id"].s();

    vector<string> preferences;
    for (int i = 0; i < (int)body["preferences"].size(); i++) {
        preferences.push_back(body["preferences"][(size_t)i].s());
    }

    User user(id, body["name"].s(), body["email"].s(), preferences);
    userMap[id] = user;

    // Step 1: Update existing Reviews that reference this User
    for (auto it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        if (it->second.getUser().getId() == id) {
            it->second.setUser(user); // Reassign the updated user to existing review
        }
    }

    // Step 2: Remove all Recommendations associated with this User
    removeEntriesWithUser(recommendationMap, id);

    // Step 3: Rebuild Recommendations
    vector<Recommendation> keptRecs;
    for (auto it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        keptRecs.push_back(it->second);
    }

    // Add new Recommendations based on User preferences
    for (auto it = bookMap.begin(); it != bookMap.end(); ++it) {
        Book book = it->second;
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

    return std::move(response(201, convertUserToJson(user).dump()));
}

response readUser(string id) {
    map<string, User>::iterator it = userMap.find(id);
    if (it != userMap.end()) {
        User user = it->second;
        return std::move(response(convertUserToJson(user).dump()));
    }
    return std::move(response(404, "User Not Found"));
}

response readAllUsers(request req) {
    char* searchParam = req.url_params.get("search");
    char* sortParam = req.url_params.get("sort");
    char* filterKey = req.url_params.get("filterKey");
    char* filterValue = req.url_params.get("filterValue");

    if (searchParam) {
        return searchUsers(string(searchParam));
    }
    if (sortParam) {
        return sortUsers(string(sortParam));
    }
    if (filterKey && filterValue) {
        return filterUsers(string(filterKey), string(filterValue));
    }

    json::wvalue json;
    int index = 0;
    map<string, User>::iterator it;
    for (it = userMap.begin(); it != userMap.end(); ++it) {
        json[index++] = convertUserToJson(it->second);
    }

    return std::move(response(json.dump()));
}

void updateUser(request req, response& res, string id) {
    map<string, User>::iterator it = userMap.find(id);
    if (it == userMap.end()) {
        res.code = 404;
        res.end("User Not Found");
        return;
    }

    json::rvalue body = json::load(req.body);
    if (!body) {
        res.code = 400;
        res.end("Invalid JSON");
        return;
    }

    User& user = it->second;
    user.setName(body["name"].s());
    user.setEmail(body["email"].s());

    vector<string> preferences;
    for (unsigned int i = 0; i < body["preferences"].size(); i++) {
        preferences.push_back(body["preferences"][(size_t)i].s());
    }
    user.setPreferences(preferences);
        // Step 1: Update existing Reviews that reference this User
    for (auto it = reviewMap.begin(); it != reviewMap.end(); ++it) {
        if (it->second.getUser().getId() == id) {
            it->second.setUser(user); // Reassign the updated user to existing review
        }
    }

    // Step 2: Remove all Recommendations associated with this User
    removeEntriesWithUser(recommendationMap, id);

    // Step 3: Rebuild Recommendations
    vector<Recommendation> keptRecs;
    for (auto it = recommendationMap.begin(); it != recommendationMap.end(); ++it) {
        keptRecs.push_back(it->second);
    }

    // Add new Recommendations based on User preferences
    for (auto it = bookMap.begin(); it != bookMap.end(); ++it) {
        Book book = it->second;
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
    res.write(convertUserToJson(user).dump());
    res.end();
}

// Updated deleteUser function
response deleteUser(string id) {
    map<string, User>::iterator it = userMap.find(id);
    if (it == userMap.end()) {
        return response(404, "User not found");
    }

    // Step 1: Erase the user
    userMap.erase(it);

    // Step 2: Remove all reviews associated with the user
    removeEntriesWithUser(reviewMap, id);

    // Step 3: Remove all recommendations associated with the user
    removeEntriesWithUser(recommendationMap, id);

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

void saveUserToFile(map<string, User> data, string filename) {
    ofstream file(filename);
    if (file.is_open()) {
        json::wvalue json;
        int index = 0;
        map<string, User>::iterator it;
        for (it = data.begin(); it != data.end(); ++it) {
            json[index++] = convertUserToJson(it->second);
        }
        file << json.dump();
        file.close();
    }
}

map<string, User> loadUserFromFile(string filename) {
    map<string, User> data;
    ifstream file(filename);

    if (file.is_open()) {
        ostringstream ss;
        ss << file.rdbuf();
        file.close();

        json::rvalue json = json::load(ss.str());

        for (json::rvalue& item : json) {
            string id = item["id"].s();
            string name = item["name"].s();
            string email = item["email"].s();

            vector<string> preferences;
            for (json::rvalue& pref : item["preferences"]) {
                preferences.push_back(pref.s());
            }

            User user(id, name, email, preferences);
            data[user.getId()] = user;
        }
    }

    return data;
}
