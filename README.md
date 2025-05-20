# 📚 Book Review & Recommendation API

A RESTful web service built in modern C++ with the Crow framework that enables users to review books and receive personalized recommendations based on their reading preferences.

> **Built by**: Mudit Mayank Jha and Britney Nguyen (Google STEP Intern)  

---

## 🚀 Project Overview

This API serves as the backend for a book-focused application, enabling:

- 📘 **Book Catalog Management**
- 🧑 **User Accounts with Genre Preferences**
- ✍️ **Book Reviews (rating + comment)**
- 🤖 **Personalized Book Recommendations**

The service reads/writes data from JSON files and provides full CRUD access to each resource.

---

## 🛠️ Tech Stack

| Layer         | Technology                      |
|---------------|----------------------------------|
| Language      | **C++17**                        |
| Framework     | [**Crow**](https://github.com/CrowCpp/Crow) (Fast C++ HTTP server) |
| JSON Library  | [Crow's built-in JSON](https://crowcpp.org/master/guides/json/) |
| Build System  | `g++` + `Makefile`               |
| Test Suite    | [**doctest**](https://github.com/doctest/doctest) for unit testing |

---

## 📦 Dependencies

This project assumes that the development environment (e.g., a Linux server) already includes:

- Crow and its built-in JSON support (included via `#include <crow>` and `#include <json>`)
- A C++17-compatible compiler (`g++`, `clang++`, etc.)
- `make` installed and available in PATH

> ℹ️ If building on a new machine, you'll need to install [Crow](https://github.com/CrowCpp/Crow) and ensure its headers are discoverable, or place the Crow headers in your project’s include path.

---

## 🧩 Architecture

The project is modularized into the following components:

- `User` – registration, genre preferences, account mgmt.
- `Book` – title, author, genre, ISBN
- `Review` – user-book rating and comment
- `Recommendation` – user-book suggestion based on genre match
- `UserBookInteraction` – abstract base class for `Review` and `Recommendation`

Each module includes:

- Model (`.h`)
- Logic and endpoints (`.cpp`)
- JSON (de)serialization
- File I/O and persistence

---

## 🔗 REST API Endpoints

### 📘 Books
```
POST   /api/books         → Add new book  
GET    /api/books         → List all (supports search, sort, filter)  
GET    /api/books/:id     → Get by ID  
PUT    /api/books/:id     → Update  
DELETE /api/books/:id     → Remove
```

### 👤 Users
```
POST   /api/users         → Register user  
GET    /api/users         → List all (search, sort, filter)  
GET    /api/users/:id     → Get by ID  
PUT    /api/users/:id     → Update  
DELETE /api/users/:id     → Remove
```

### ✍️ Reviews
```
POST   /api/reviews       → Submit review  
GET    /api/reviews       → List all (search, sort, filter)  
GET    /api/reviews/:id   → Get by ID  
PUT    /api/reviews/:id   → Update  
DELETE /api/reviews/:id   → Remove
```

### 📚 Recommendations
```
GET    /api/recommendations              → Get all (search, sort, filter)  
GET    /api/recommendations/:id         → Get by ID
```

> Recommendations are auto-generated on user/book creation and updated dynamically based on genre preferences.

---

## 🧪 Unit Testing

We used `doctest` to rigorously test all core classes:

- Constructors & Edge Cases
- JSON Save/Load Consistency
- Unicode & Long Input Handling
- CRUD Lifecycle Integrity

Run tests with:
```bash
make test
./test
```

---

## 💾 Data Persistence

All entities are saved to JSON files:
```bash
books.json           → Book records  
users.json           → User accounts  
reviews.json         → Review history  
recommendations.json → Personalized recs
```

They are **loaded at startup** and **persisted at shutdown**.

---

## 🧠 Design Considerations

- Modular OOP design via abstract base class (`UserBookInteraction`)
- Auto-sync between resources (e.g. deleting a book removes its reviews & recs)
- Clean ID reindexing to avoid orphaned references
- Optimized for readability, traceability, and extensibility

---

## ✅ How to Run

### Build
```bash
make
```

### Run API Server
```bash
./bookReviewAPI
```

Runs on: [http://localhost:18525](http://localhost:18525)

---

## 🏁 Sample Usage

Here’s how you’d register a user:

```bash
curl -X POST http://localhost:18525/api/users \
  -H "Content-Type: application/json" \
  -d '{
        "id": "u1",
        "name": "Alice",
        "email": "alice@example.com",
        "preferences": ["Fiction", "Romantic"]
      }'
```

---

## 🧑‍💻 Authors

- **Mudit Mayank Jha** – [GitHub](https://github.com/muditjha20)
- **Britney Nguyen** – Google STEP Intern

---

## 🏆 Highlights

- **Modular C++ backend project** using the Crow web framework
- REST API covering Users, Books, Reviews, and Recommendations
- Clean architecture with file-based persistence and dynamic sync between entities
- Rigorously tested with `doctest`
- Scalable backend foundation for future UI or database integration

---

## 📌 License

This project is for academic use. Contact authors for reuse or extension.
