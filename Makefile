all: bookReviewAPI test

bookReviewAPI: bookReviewAPI.o User.o Book.o Review.o Recommendation.o
	g++ -Wall bookReviewAPI.o User.o Book.o Review.o Recommendation.o -o bookReviewAPI

test: Tests.o User.o Book.o Review.o Recommendation.o globals.o
	g++ -Wall Tests.o User.o Book.o Review.o Recommendation.o globals.o -o test

bookReviewAPI.o: bookReviewAPI.cpp User.h Book.h Review.h Recommendation.h
	g++ -c bookReviewAPI.cpp

globals.o: globals.cpp User.h Book.h Review.h Recommendation.h
	g++ -c globals.cpp

User.o: User.cpp User.h
	g++ -c User.cpp

Book.o: Book.cpp Book.h
	g++ -c Book.cpp

Review.o: Review.cpp Review.h
	g++ -c Review.cpp

Recommendation.o: Recommendation.cpp Recommendation.h
	g++ -c Recommendation.cpp

Tests.o: Tests.cpp User.h Book.h Review.h Recommendation.h
	g++ -c Tests.cpp

clean:
	rm -f *.o bookReviewAPI test
