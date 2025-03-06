Copyright @lucabotez

# HTTP WebClient

## Overview
**HTTP WebClient** is a C++ command-line client that interacts with a **REST API** to perform **user authentication, book management, and session handling**. It sends HTTP requests to a predefined server and processes the responses efficiently.

## Features
- **User authentication:** Register, login, and logout.
- **Library access management:** Enter and navigate the digital library.
- **Book operations:** Retrieve, add, and delete books.
- **Session handling:** Manages authentication cookies and access tokens.
- **Error handling:** Interprets server responses and displays appropriate messages.

## Implemented Commands

### **1. Register**
- Registers a new user by sending a **POST** request with a username and password.
- Ensures both fields contain only **alphanumeric characters**.
- Server response indicates success or failure (e.g., username already taken).

### **2. Login**
- Logs in a user by sending a **POST** request with credentials.
- Stores an **authentication cookie** upon successful login for session management.

### **3. Enter Library**
- Requests access to the **library system** using a **GET** request.
- Stores an **access token** for subsequent authenticated actions.

### **4. Get Books / Get Book**
- Fetches the list of all books (`get_books`) or retrieves a specific book by ID (`get_book`).
- Parses the **JSON response** and displays book details.

### **5. Add Book**
- Adds a book to the library by sending a **POST** request.
- Requires **title, author, genre, publisher, and page count**.
- Validates input before sending the request.

### **6. Delete Book**
- Deletes a book by sending a **DELETE** request with a **valid book ID**.
- Ensures the user has access to the library before performing the operation.

### **7. Logout**
- Logs out the current user by sending a **GET** request.
- Clears the stored **authentication cookie** and **access token**.

### **8. Exit**
- Stops program execution without displaying any additional messages.

## Notes
- Uses **HTTP requests** (`GET`, `POST`, `DELETE`) to communicate with the server.
- Stores **authentication cookies** and **access tokens** for session management.
- Implements **JSON parsing** for handling book data.
