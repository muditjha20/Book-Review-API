# Project: C++ REST API

## Instructions

See course web page [Project: C++ REST API](https://cmsc240-s25.github.io/project.html).

## Group Member Names

#### Write your group members names below:

1. Britney Nguyen
2. Mudit Mayank Jha

## **Proposal**
### **Book Review & Recommendation API**  
#### **Description**  
The Book Review & Recommendation API is a web service designed to allow users to review books and receive personalized recommendations based on their preferences. This API will serve as a platform for book enthusiasts to share their opinions and discover new books tailored to their tastes.

#### **Resources**  
- **User**: Represents a user of the API.  
  - Attributes:  
    - `user_id` (int)  
    - `name` (string)  
    - `email` (string)  
    - `preferences` (list of genres) 

- **Book**: Represents a book available in the system.  
  - Attributes:  
    - `book_id` (int)  
    - `title` (string)  
    - `author` (string)  
    - `genre` (string)  
    - `ISBN` (string) 

- **Review**: Represents a user's review of a book.  
  - Attributes:  
    - `review_id` (int)  
    - `user` (`User` object) – Author of the review  
    - `book` (`Book` object) – Book being reviewed  
    - `rating` (int, 1-5)  
    - `comment` (string)  

- **Recommendation**: Represents book suggestions for a user.  
  - Attributes:  
    - `recommendation_id` (int)  
    - `user` (`User` object) – User receiving the recommendation  
    - `suggested_book` (`Book` object) – Book being recommended

#### **Core Features**  
- **User Management**: Users can register, update their preferences, and manage their accounts.
- **Book Catalog**: Users can browse and search for books by title, author, genre, and ISBN.
- **Book Reviews**: Users can submit, edit, delete, and view reviews of books.
- **Recommendations**: The system provides book recommendations based on user preferences and prior reviews.

