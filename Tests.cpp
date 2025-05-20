#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "User.h"
#include "Book.h"
#include "Review.h"
#include "Recommendation.h"
#include "crow.h"

#include <fstream>

using namespace std;

// External maps
extern map<string, User> userMap;
extern map<string, Book> bookMap;
extern map<string, Review> reviewMap;
extern map<string, Recommendation> recommendationMap;

// save/load function declarations
void saveUserToFile(map<string, User> data, string filename);
map<string, User> loadUserFromFile(string filename);
void saveBookToFile(map<string, Book> data, string filename);
map<string, Book> loadBookFromFile(string filename);

TEST_CASE("User class - Constructors") {
    SUBCASE("Default constructor initializes fields properly") {
        User user;
        CHECK(user.getId() == "");
        CHECK(user.getName() == "");
        CHECK(user.getEmail() == "");
        CHECK(user.getPreferences().empty());
    }

    SUBCASE("Parameterized constructor correctly assigns values") {
        vector<string> prefs = {"fiction", "science", "history"};
        User user("101", "Alice", "alice@example.com", prefs);

        CHECK(user.getId() == "101");
        CHECK(user.getName() == "Alice");
        CHECK(user.getEmail() == "alice@example.com");
        CHECK(user.getPreferences() == prefs);
    }
}

TEST_CASE("User class - Getter and Setter Methods") {
    vector<string> prefs = {"comedy", "drama"};
    User user("202", "Bob", "bob@example.com", prefs);

    SUBCASE("SetName updates name correctly") {
        user.setName("Robert");
        CHECK(user.getName() == "Robert");
    }

    SUBCASE("SetEmail updates email correctly") {
        user.setEmail("robert@example.com");
        CHECK(user.getEmail() == "robert@example.com");
    }

    SUBCASE("SetPreferences updates preferences correctly") {
        vector<string> newPrefs = {"adventure", "thriller"};
        user.setPreferences(newPrefs);
        CHECK(user.getPreferences() == newPrefs);
    }
}

TEST_CASE("User class - Edge Cases") {
    SUBCASE("Setting empty strings for name and email") {
        User user("303", "", "", {});
        CHECK(user.getName() == "");
        CHECK(user.getEmail() == "");
        CHECK(user.getPreferences().empty());

        user.setName("");
        user.setEmail("");

        CHECK(user.getName() == "");
        CHECK(user.getEmail() == "");
    }

    SUBCASE("Handling empty preference list") {
        User user("404", "Charlie", "charlie@example.com", {});
        CHECK(user.getPreferences().empty());
    }

    SUBCASE("Handling long strings") {
        string longString(1000, 'x');
        vector<string> longPrefs = {longString, longString};

        User user("505", longString, longString, longPrefs);

        CHECK(user.getName() == longString);
        CHECK(user.getEmail() == longString);
        CHECK(user.getPreferences() == longPrefs);

        string evenLongerString(2000, 'y');
        vector<string> newLongPrefs = {evenLongerString, evenLongerString};

        user.setName(evenLongerString);
        user.setEmail(evenLongerString);
        user.setPreferences(newLongPrefs);

        CHECK(user.getName() == evenLongerString);
        CHECK(user.getEmail() == evenLongerString);
        CHECK(user.getPreferences() == newLongPrefs);
    }

    SUBCASE("Handling special characters and Unicode") {
        string specialString = "ユーザー名 🌟 élève ñ ñandú 中文测试";
        vector<string> specialPrefs = {"mystery 📖", "sci-fi 🚀"};

        User user("606", specialString, specialString, specialPrefs);

        CHECK(user.getName() == specialString);
        CHECK(user.getEmail() == specialString);
        CHECK(user.getPreferences() == specialPrefs);
    }

    SUBCASE("Handling very large preference list") {
        vector<string> hugePrefs(1000, "fantasy");
        User user("707", "David", "david@example.com", hugePrefs);

        CHECK(user.getPreferences().size() == 1000);

        for (const auto& pref : user.getPreferences()) {
            CHECK(pref == "fantasy");
        }
    }
}

TEST_CASE("User save/load from file") {
    userMap.clear();

    User u("u1", "Alice", "alice@gmail.com", {"Fantasy", "Mystery"});
    userMap[u.getId()] = u;

    // Save
    saveUserToFile(userMap, "test_users.json");

    // Clear and load
    userMap.clear();
    userMap = loadUserFromFile("test_users.json");

    // Now check the loaded data
    REQUIRE(userMap.count("u1") == 1);
    User loaded = userMap["u1"];
    CHECK(loaded.getId() == "u1");
    CHECK(loaded.getName() == "Alice");
    CHECK(loaded.getEmail() == "alice@gmail.com");
    CHECK(loaded.getPreferences() == vector<string>{"Fantasy", "Mystery"});
}

TEST_CASE("Book class - Constructors") {
    SUBCASE("Default constructor initializes all fields to empty strings") {
        Book book;
        CHECK(book.getId() == "");
        CHECK(book.getTitle() == "");
        CHECK(book.getAuthor() == "");
        CHECK(book.getGenre() == "");
        CHECK(book.getIsbn() == "");
    }

    SUBCASE("Parameterized constructor correctly assigns values") {
        Book book("123", "1984", "George Orwell", "Dystopian", "9780451524935");

        CHECK(book.getId() == "123");
        CHECK(book.getTitle() == "1984");
        CHECK(book.getAuthor() == "George Orwell");
        CHECK(book.getGenre() == "Dystopian");
        CHECK(book.getIsbn() == "9780451524935");
    }
}

TEST_CASE("Book class - Getter and Setter Methods") {
    Book book("1", "Placeholder Title", "Placeholder Author", "Placeholder Genre", "Placeholder ISBN");

    SUBCASE("SetTitle updates title correctly") {
        book.setTitle("New Title");
        CHECK(book.getTitle() == "New Title");
    }

    SUBCASE("SetAuthor updates author correctly") {
        book.setAuthor("New Author");
        CHECK(book.getAuthor() == "New Author");
    }

    SUBCASE("SetGenre updates genre correctly") {
        book.setGenre("New Genre");
        CHECK(book.getGenre() == "New Genre");
    }

    SUBCASE("SetIsbn updates ISBN correctly") {
        book.setIsbn("New ISBN");
        CHECK(book.getIsbn() == "New ISBN");
    }
}

TEST_CASE("Book class - Edge Cases") {
    SUBCASE("Empty strings can be set and retrieved") {
        Book book("id", "", "", "", "");
        CHECK(book.getTitle() == "");
        CHECK(book.getAuthor() == "");
        CHECK(book.getGenre() == "");
        CHECK(book.getIsbn() == "");

        book.setTitle("");
        book.setAuthor("");
        book.setGenre("");
        book.setIsbn("");

        CHECK(book.getTitle() == "");
        CHECK(book.getAuthor() == "");
        CHECK(book.getGenre() == "");
        CHECK(book.getIsbn() == "");
    }

    SUBCASE("Long strings are handled correctly") {
        string longString(1000, 'a'); // 1000 'a' characters
        Book book("long", longString, longString, longString, longString);

        CHECK(book.getTitle() == longString);
        CHECK(book.getAuthor() == longString);
        CHECK(book.getGenre() == longString);
        CHECK(book.getIsbn() == longString);

        string newLongString(2000, 'b');
        book.setTitle(newLongString);
        book.setAuthor(newLongString);
        book.setGenre(newLongString);
        book.setIsbn(newLongString);

        CHECK(book.getTitle() == newLongString);
        CHECK(book.getAuthor() == newLongString);
        CHECK(book.getGenre() == newLongString);
        CHECK(book.getIsbn() == newLongString);
    }

    SUBCASE("Special characters and Unicode are preserved") {
        string specialString = "Título con acentos áéíóú ñ ç 中文测试 🚀";
        Book book("special", specialString, specialString, specialString, specialString);

        CHECK(book.getTitle() == specialString);
        CHECK(book.getAuthor() == specialString);
        CHECK(book.getGenre() == specialString);
        CHECK(book.getIsbn() == specialString);

        book.setTitle("🎉");
        book.setAuthor("🎉");
        book.setGenre("🎉");
        book.setIsbn("🎉");

        CHECK(book.getTitle() == "🎉");
        CHECK(book.getAuthor() == "🎉");
        CHECK(book.getGenre() == "🎉");
        CHECK(book.getIsbn() == "🎉");
    }
}

TEST_CASE("Book save/load from file") {
    bookMap.clear();

    Book b("b1", "Dune", "Frank Herbert", "Sci-Fi", "9780441172719");
    bookMap[b.getId()] = b;

    // Save
    saveBookToFile(bookMap, "test_books.json");

    // Clear and load
    bookMap.clear();
    bookMap = loadBookFromFile("test_books.json");

    // Now check the loaded data
    REQUIRE(bookMap.count("b1") == 1);
    Book loaded = bookMap["b1"];
    CHECK(loaded.getId() == "b1");
    CHECK(loaded.getTitle() == "Dune");
    CHECK(loaded.getAuthor() == "Frank Herbert");
    CHECK(loaded.getGenre() == "Sci-Fi");
    CHECK(loaded.getIsbn() == "9780441172719");
}

TEST_CASE("Review class - Constructors") {
    SUBCASE("Default constructor initializes fields to defaults") {
        Review review;
        CHECK(review.getRating() == 0);
        CHECK(review.getComment() == "");
    }

    SUBCASE("Parameterized constructor correctly assigns fields") {
        User user("1", "Alice", "alice@example.com", {"fiction"});
        Book book("b1", "1984", "George Orwell", "Dystopian", "1234567890");
        Review review("r1", user, book, 5, "Excellent!");

        CHECK(review.getId() == "r1");
        CHECK(review.getUser().getId() == "1");
        CHECK(review.getBook().getId() == "b1");
        CHECK(review.getRating() == 5);
        CHECK(review.getComment() == "Excellent!");
    }
}

TEST_CASE("Review class - Getters and Setters") {
    User user("2", "Bob", "bob@example.com", {"history"});
    Book book("b2", "Sapiens", "Yuval Noah Harari", "Non-fiction", "0987654321");
    Review review("r2", user, book, 3, "Good read");

    SUBCASE("SetRating updates rating") {
        review.setRating(4);
        CHECK(review.getRating() == 4);
    }

    SUBCASE("SetComment updates comment") {
        review.setComment("Thought-provoking");
        CHECK(review.getComment() == "Thought-provoking");
    }
}

TEST_CASE("Review class - Edge Cases") {
    User user("3", "Charlie", "charlie@example.com", {});
    Book book("b3", "Invisible Man", "Ralph Ellison", "Literature", "1122334455");

    SUBCASE("Negative and large ratings (logical edge)") {
        Review review("r3", user, book, -1, "Too dark");
        CHECK(review.getRating() == -1);

        review.setRating(100);
        CHECK(review.getRating() == 100); // Accepts value, even if logically invalid
    }

    SUBCASE("Empty and special character comments") {
        Review review("r4", user, book, 4, "");
        CHECK(review.getComment() == "");

        string special = "🔥💡✅ El mejor libro jamás leído! 中文测试 ñ ñandú";
        review.setComment(special);
        CHECK(review.getComment() == special);
    }

    SUBCASE("Very long comment") {
        string longComment(10000, 'x');
        Review review("r5", user, book, 5, longComment);
        CHECK(review.getComment().length() == 10000);
    }
}

TEST_CASE("Review save/load from file") {
    reviewMap.clear();

    // Create User and Book first
    User user("u1", "Alice", "alice@example.com", {"fantasy", "mystery"});
    Book book("b1", "1984", "George Orwell", "Dystopian", "1234567890");

    // Create Review
    Review r("r1", user, book, 5, "Amazing book!");
    reviewMap[r.getId()] = r;

    // Save to file
    saveReviewToFile(reviewMap, "test_reviews.json");

    // Clear and reload
    reviewMap.clear();
    reviewMap = loadReviewFromFile("test_reviews.json");

    REQUIRE(reviewMap.count("r1") == 1);

    Review loaded = reviewMap["r1"];
    CHECK(loaded.getId() == "r1");
    CHECK(loaded.getUser().getId() == "u1");
    CHECK(loaded.getBook().getId() == "b1");
    CHECK(loaded.getRating() == 5);
    CHECK(loaded.getComment() == "Amazing book!");
}

TEST_CASE("Recommendation class - Constructors") {
    SUBCASE("Default constructor initializes fields to empty/default values") {
        Recommendation rec;
        CHECK(rec.getId() == "");
        CHECK(rec.getUser().getId() == "");
        CHECK(rec.getBook().getId() == "");
    }

    SUBCASE("Parameterized constructor assigns values correctly") {
        User user("u10", "Eve", "eve@example.com", {"romance", "drama"});
        Book book("b10", "Pride and Prejudice", "Jane Austen", "Romance", "123456789X");
        Recommendation rec("rec10", user, book);

        CHECK(rec.getId() == "rec10");
        CHECK(rec.getUser().getId() == "u10");
        CHECK(rec.getBook().getId() == "b10");
    }
}

TEST_CASE("Recommendation class - Getters and Setters") {
    User user("u20", "Frank", "frank@example.com", {"adventure"});
    Book book("b20", "Treasure Island", "Robert Louis Stevenson", "Adventure", "1123581321");
    Recommendation rec("rec20", user, book);

    SUBCASE("SetId updates ID correctly") {
        rec.setId("newRecId");
        CHECK(rec.getId() == "newRecId");
    }

    SUBCASE("SetUser updates User correctly") {
        User newUser("u21", "Fred", "fred@example.com", {"horror"});
        rec.setUser(newUser);
        CHECK(rec.getUser().getId() == "u21");
        CHECK(rec.getUser().getName() == "Fred");
    }

    SUBCASE("SetBook updates Book correctly") {
        Book newBook("b21", "Dracula", "Bram Stoker", "Horror", "6677889900");
        rec.setBook(newBook);
        CHECK(rec.getBook().getId() == "b21");
        CHECK(rec.getBook().getTitle() == "Dracula");
    }
}

TEST_CASE("Recommendation class - Edge Cases") {
    User user("u30", "Grace", "grace@example.com", {"mystery"});
    Book book("b30", "Sherlock Holmes", "Arthur Conan Doyle", "Mystery", "9988776655");

    SUBCASE("Handling of special characters and Unicode in user and book") {
        string specialName = "ユーザー 🚀 中文测试 ñ ç";
        string specialTitle = "Título Especial 📚🌟";

        User specialUser("u40", specialName, "special@example.com", {"sci-fi", "fantasy"});
        Book specialBook("b40", specialTitle, "Autor Especial", "Fantasia", "abcdef123456");

        Recommendation rec("rec40", specialUser, specialBook);

        CHECK(rec.getUser().getName() == specialName);
        CHECK(rec.getBook().getTitle() == specialTitle);
    }

    SUBCASE("Very long ID strings") {
        string longId(500, 'R');
        Recommendation rec(longId, user, book);

        CHECK(rec.getId() == longId);

        string newLongId(1000, 'S');
        rec.setId(newLongId);
        CHECK(rec.getId() == newLongId);
    }
}

TEST_CASE("Recommendation interactions - Combined behavior") {
    SUBCASE("Recommendation correctly links User and Book objects") {
        User user("u50", "Helen", "helen@example.com", {"poetry"});
        Book book("b50", "Leaves of Grass", "Walt Whitman", "Poetry", "123321123321");

        Recommendation rec("rec50", user, book);

        CHECK(rec.getUser().getEmail() == "helen@example.com");
        CHECK(rec.getBook().getAuthor() == "Walt Whitman");
    }
}

TEST_CASE("Recommendation save/load from file") {
    recommendationMap.clear();

    // Create User and Book first
    User user("u1", "Alice", "alice@example.com", {"fantasy", "mystery"});
    Book book("b2", "Dune", "Frank Herbert", "Sci-Fi", "9780441172719");

    // Create Recommendation
    Recommendation rec("rec1", user, book);
    recommendationMap[rec.getId()] = rec;

    // Save to file
    saveRecommendationToFile(recommendationMap, "test_recommendations.json");

    // Clear and reload
    recommendationMap.clear();
    recommendationMap = loadRecommendationFromFile("test_recommendations.json");

    REQUIRE(recommendationMap.count("rec1") == 1);

    Recommendation loaded = recommendationMap["rec1"];
    CHECK(loaded.getId() == "rec1");
    CHECK(loaded.getUser().getId() == "u1");
    CHECK(loaded.getBook().getId() == "b2");
}


