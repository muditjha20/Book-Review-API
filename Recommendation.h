#ifndef RECOMMENDATION_H
#define RECOMMENDATION_H

#include "UserBookInteraction.h"
#include <crow.h>
#include <map>

using namespace std;
using namespace crow;

class Recommendation : public UserBookInteraction {
public:
    Recommendation() : UserBookInteraction() {}
    Recommendation(string id, User user, Book book)
        : UserBookInteraction(id, user, book) {}
};

// Helpers
json::wvalue convertRecommendationToJson(Recommendation rec);

// CRUD Handlers
response createRecommendation(request req);
response readRecommendation(string id);
response readAllRecommendations(request req);
void updateRecommendation(request req, response& res, string id);
response deleteRecommendation(string id);

void saveRecommendationToFile(map<string, Recommendation> data, string filename);
map<string, Recommendation> loadRecommendationFromFile(string filename);

#endif
