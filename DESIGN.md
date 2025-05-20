# Project Design Document
![image](https://github.com/user-attachments/assets/a5fa3abe-212a-4e41-a834-e9fc113e1b41)


## Introduction
The **Book Review & Recommendation API** is a web service designed to support book lovers in discovering new reads and sharing their opinions. It provides RESTful endpoints to manage users, books, reviews, and recommendations. The API enables users to register, update their preferences, browse book catalogs, submit reviews, and receive personalized book recommendations, making it an all-in-one backend solution for any book-focused application.


## Background/Context
As the digital world transforms how readers discover literature, platforms that offer tailored book recommendations and allow for community-driven reviews are becoming increasingly valuable. The Book Review & Recommendation API meets these needs by enabling the structured storage, retrieval, and recommendation of books based on user preferences and feedback. It serves as the backbone for mobile or web applications that aim to improve reader engagement and satisfaction.

## Stakeholders
- **Users:** Book enthusiasts who interact with the API through a client-facing app or website. They expect an easy way to find books that match their tastes, leave reviews, and view others' opinions.

- **System Administrators/Developers:**  Responsible for managing book data, ensuring uptime, and implementing algorithmic improvements for recommendations. The quality of the API’s performance and its data structure directly affects the end-user experience.



## Functional Requirements

### **Resource Creation**
- The service shall allow creation of users, books, reviews, and recommendations via `POST` requests.
- All submitted data must pass validation before creation.
- On success, the API shall return `201 Created` with the resource in the response body.

### **Resource Retrieval**
- The service shall support retrieval of all and individual users, books, reviews, and recommendations via `GET` requests.
- On success, the API shall return `200 OK` with the requested data.

### **Resource Update**
- The service shall allow updates to users, books, and reviews via `PUT` requests.
- Data must be validated before update.
- On success, the API shall return `200 OK` with the updated resource.

### **Resource Deletion**
- The API shall support deletion of users, books, and reviews via `DELETE` requests.
- On success, the API shall return `204 No Content`.
- If unauthorized, the API shall return `403 Forbidden`.

### **Data Validation**
- All inputs shall be validated according to predefined schemas.
- Validation failures will return `400 Bad Request` with a descriptive error message.

### **Authentication and Authorization**
- Creating, updating, or deleting resources shall require authentication.
- Authorization mechanisms shall ensure users can only modify their own data.

### **Error Handling**
- The API shall use standard HTTP status codes and provide meaningful error messages:
  - `400 Bad Request` for validation errors.
  - `401 Unauthorized` or `403 Forbidden` for access issues.
  - `404 Not Found` for missing resources.
  - `500 Internal Server Error` for unexpected conditions.

## Use Case Description
### **User**
- **Create (POST):**  
  As a new user, I want to register with my name, email, and reading preferences so that I can receive tailored book recommendations.

- **Read (GET):**  
  As a user, I want to view my profile to confirm my preferences are correct.

- **Update (PUT):**  
  As a user, I want to update my preferences when my interests change.

- **Delete (DELETE):**  
  As a user, I want to delete my account if I no longer use the service.



### **Book**
- **Create (POST):**  
  As an admin, I want to add new books to the catalog to keep the system current.

- **Read (GET):**  
  As a user, I want to browse and search for books by title, author, genre, or ISBN.

- **Update (PUT):**  
  As an admin, I want to update book details to correct errors or add new data.

- **Delete (DELETE):**  
  As an admin, I want to remove books that are no longer available.



### **Review**
- **Create (POST):**  
  As a user, I want to submit a rating and comment on a book I’ve read.

- **Read (GET):**  
  As a user, I want to see all reviews for a book before deciding to read it.

- **Update (PUT):**  
  As a user, I want to update my review if my opinion has changed.

- **Delete (DELETE):**  
  As a user, I want to delete my review if I no longer want to share it.



### **Recommendation**
- **Read (GET):**  
  As a user, I want to get personalized book suggestions based on my preferences and review history.

## List Of Resources
- **User**: Represents a user of the API.  
  - Attributes:  
    - `user_id` (string)  
    - `name` (string)  
    - `email` (string)  
    - `preferences` (list of genres) 

- **Book**: Represents a book available in the system.  
  - Attributes:  
    - `book_id` (string)  
    - `title` (string)  
    - `author` (string)  
    - `genre` (string)  
    - `ISBN` (string) 

- **Review**: Represents a user's review of a book.  
  - Attributes:  
    - `review_id` (string)  
    - `user` (`User` object) – Author of the review  
    - `book` (`Book` object) – Book being reviewed  
    - `rating` (int, 1-5)  
    - `comment` (string)  

- **Recommendation**: Represents book suggestions for a user.  
  - Attributes:  
    - `recommendation_id` (string)  
    - `user` (`User` object) – User receiving the recommendation  
    - `suggested_book` (`Book` object) – Book being recommended

## List of End Points
### **Users**
- `POST /api/users`  
  ```
  {
    "user_id": "1",
    "name": "Alice Johnson",
    "email": "alice.johnson@richmond.edu",
    "preferences": ["Fantasy", "Science Fiction"]
  }
  ```

- `GET /api/users`  
- `GET /api/users/{user_id}`  
- `PUT /api/users/{user_id}`  
  ```
  {
    "user_id": "1",
    "name": "Alice J.",
    "email": "alice.j@richmond.edu",
    "preferences": ["Mystery", "Historical Fiction"]
  }
  ```

- `DELETE /api/users/{user_id}`



### **Books**
- `POST /api/books`  
  ```
  {
    "book_id": "101",
    "title": "The Great Adventure",
    "author": "John Smith",
    "genre": "Adventure",
    "ISBN": "978-3-16-148410-0"
  }
  ```

- `GET /api/books`  
- `GET /api/books/{book_id}`  
- `PUT /api/books/{book_id}`  
  ```
  {
    "book_id": "101",
    "title": "The Grand Adventure",
    "author": "John S. Smith",
    "genre": "Action-Adventure",
    "ISBN": "978-3-16-148410-0"
  }
  ```

- `DELETE /api/books/{book_id}`



### **Reviews**
- `POST /api/reviews`  
  ```
  {
    "review_id": "1",
    "user": {
      "user_id": "1"
    },
    "book": {
      "book_id": "101"
    },
    "rating": 5,
    "comment": "A thrilling read from start to finish!"
  }
  ```

- `GET /api/reviews`  
- `GET /api/reviews/{review_id}`  
- `PUT /api/reviews/{review_id}`  
  ```
  {
    "review_id": "1",
    "user": {
      "user_id": "1"
    },
    "book": {
      "book_id": "101"
    },
    "rating": 5,
    "comment": "A thrilling read from start to finish!"
  }
  ```

- `DELETE /api/reviews/{review_id}`



### **Recommendations**
- `GET /api/recommendations/{user_id}`  
  **Description**: Retrieve book recommendations tailored to the user’s preferences and review history.  
  **Example Response**:
  ```
  [
    {
      "recommendation_id": "201",
      "user": {
        "user_id": "1"
      },
      "suggested_book": {
        "book_id": "104"
      }
    },
    {
      "recommendation_id": "202",
      "user": {
        "user_id": "1"
      },
      "suggested_book": {
        "book_id": "105"
      }
    }
  ]
  ```



## **Error Handling Strategies**

- **Validation Errors:**  
  Return `400 Bad Request` with details on what failed.

- **Authentication/Authorization Errors:**  
  Return `401 Unauthorized` for missing/invalid auth; `403 Forbidden` for access violations.

- **Not Found Errors:**  
  Return `404 Not Found` when a requested resource doesn't exist.

- **Server Errors:**  
  Return `500 Internal Server Error` for unexpected backend issues.

## UML Diagrams
![image](https://github.com/user-attachments/assets/7758ec2d-f13c-4547-91e7-694887b71ce2)




