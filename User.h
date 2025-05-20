#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <map>
#include <crow.h>
#include "Book.h"

using namespace std;
using namespace crow;

class User {
public:
    User() {}
    User(string id, string name, string email, vector<string> preferences)
        : id(id), name(name), email(email), preferences(preferences) {}

    string getId() { return id; }
    string getName() { return name; }
    string getEmail() { return email; }
    vector<string> getPreferences() { return preferences; }

    void setName(string value) { name = value; }
    void setEmail(string value) { email = value; }
    void setPreferences(vector<string> value) { preferences = value; }

private:
    string id;
    string name;
    string email;
    vector<string> preferences;
};

// JSON conversion
json::wvalue convertUserToJson(User user);
// CRUD + extended functionality
response createUser(request req);
response readUser(string id);
response readAllUsers(request req);
void updateUser(request req, response& res, string id);
response deleteUser(string id);

void saveUserToFile(map<string, User> data, string filename);
map<string, User> loadUserFromFile(string filename);



#endif
