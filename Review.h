#ifndef REVIEW_H
#define REVIEW_H

#include <string>
#include <vector>
#include <map>
#include <crow.h>
#include "UserBookInteraction.h"
#include "User.h"
#include "Book.h"

using namespace std;
using namespace crow;

class Review : public UserBookInteraction {
public:
    Review() : UserBookInteraction() {}
    Review(string id, User user, Book book, int rating, string comment)
        : UserBookInteraction(id, user, book), rating(rating), comment(comment) {}

    int getRating() { return rating; }
    string getComment() { return comment; }

    void setRating(int value) { rating = value; }
    void setComment(string value) { comment = value; }

private:
    int rating;
    string comment;
};

// Helpers
json::wvalue convertReviewToJson(Review& review);

// CRUD Handlers
response createReview(request req);
response readReview(string id);
response readAllReviews(request req);
void updateReview(request req, response& res, string id);
response deleteReview(string id);

void saveReviewToFile(map<string, Review> data, string filename);
map<string, Review> loadReviewFromFile(string filename);

#endif
