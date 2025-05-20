#include "Book.h"
#include "User.h"
#include "Review.h"
#include "Recommendation.h"
#include <crow.h>
#include <vector>

using namespace crow;
using namespace std;

map<string, Book> bookMap;
map<string, User> userMap;
map<string, Review> reviewMap;
map<string, Recommendation> recommendationMap;

int main() {
    bookMap = loadBookFromFile("books.json");
    userMap = loadUserFromFile("users.json");
    reviewMap = loadReviewFromFile("reviews.json");
    recommendationMap = loadRecommendationFromFile("recommendations.json");

    SimpleApp app;

    // User endpoints
    CROW_ROUTE(app, "/api/users").methods(HTTPMethod::POST)(createUser);
    CROW_ROUTE(app, "/api/users").methods(HTTPMethod::GET)(readAllUsers);
    CROW_ROUTE(app, "/api/users/<string>").methods(HTTPMethod::GET)(readUser);
    CROW_ROUTE(app, "/api/users/<string>").methods(HTTPMethod::PUT)(updateUser);
    CROW_ROUTE(app, "/api/users/<string>").methods(HTTPMethod::DELETE)(deleteUser);

    // Book endpoints
    CROW_ROUTE(app, "/api/books").methods(HTTPMethod::POST)(createBook);
    CROW_ROUTE(app, "/api/books").methods(HTTPMethod::GET)(readAllBooks);
    CROW_ROUTE(app, "/api/books/<string>").methods(HTTPMethod::GET)(readBook);
    CROW_ROUTE(app, "/api/books/<string>").methods(HTTPMethod::PUT)(updateBook);
    CROW_ROUTE(app, "/api/books/<string>").methods(HTTPMethod::DELETE)(deleteBook);

    // Review endpoints
    CROW_ROUTE(app, "/api/reviews").methods(HTTPMethod::POST)(createReview);
    CROW_ROUTE(app, "/api/reviews").methods(HTTPMethod::GET)(readAllReviews);
    CROW_ROUTE(app, "/api/reviews/<string>").methods(HTTPMethod::GET)(readReview);
    CROW_ROUTE(app, "/api/reviews/<string>").methods(HTTPMethod::PUT)(updateReview);
    CROW_ROUTE(app, "/api/reviews/<string>").methods(HTTPMethod::DELETE)(deleteReview);

    // Recommendation endpoints (GET only)
    CROW_ROUTE(app, "/api/recommendations").methods(HTTPMethod::GET)(readAllRecommendations);
    CROW_ROUTE(app, "/api/recommendations/<string>").methods(HTTPMethod::GET)(readRecommendation);

    app.port(18525).multithreaded().run();
    
    saveBookToFile(bookMap, "books.json");
    saveUserToFile(userMap, "users.json");
    saveReviewToFile(reviewMap, "reviews.json");
    saveRecommendationToFile(recommendationMap, "recommendations.json");
    return 0;
}
