// Copyright @lucabotez

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "helpers.hpp"
#include "requests.hpp"
#include "nlohmann/json.hpp"

// function used for breaking down a string by a given delimiter
// (in our case we will be using it on the HTTP responses with the
// standard "\r\n" delimiter); returns a vector with all the
// substrings
std::vector<std::string> split_by_delimiter(std::string string, std::string delimiter) {
    std::vector<std::string> result; // the result vector
    std::string::size_type start, end, delimiter_length;

    start = 0;
    delimiter_length = delimiter.length();

    // extracting the needed substrings from the given string
    while ((end = string.find(delimiter, start)) != std::string::npos) {
        result.push_back(string.substr(start, end - start));
        start = end + delimiter_length;
    }

    result.push_back(string.substr(start));

    return result;
}

// function used to obtain the error code from an HTTP response
int get_error_code(std::string response) {
    // separating the response by the standard HTTP delimiter
    std::vector<std::string> tokens;
    tokens = split_by_delimiter(response, "\r\n");

    // accessing the first line of the HTTP response (tokens[0])
    // which contains the error code, splitting it one more time
    // by " " and obtaining the result in the second position
    // of the new delimitation
    std::string error_code = split_by_delimiter(tokens[0], " ")[1];

    // converting the error code to an integer
    return stoi(error_code);
}

// function used to obtain the authentication cookie from an HTTP
// response
std::string get_cookie(std::string response) {
    // separating the response by the standard HTTP delimiter
    std::vector<std::string> tokens;
    tokens = split_by_delimiter(response, "\r\n");

    // accesing the eighteenth line of the HTTP response (tokens[17])
    // which contains exactly the cookies, splitting it one more time
    // by " " and obtaining the result in the second position
    // of the new delimitation
    std::string session_cookie = split_by_delimiter(tokens[17], " ")[1];

    // removing the ';' from the cookie
    session_cookie.pop_back();

    return session_cookie;
}

// function used to obtain the library access token from an HTTP
// response
std::string get_token(std::string response) {
    // separating the response by the standard HTTP delimiter
    std::vector<std::string> tokens;
    tokens = split_by_delimiter(response, "\r\n");

    // accesing the last line of the HTTP response (tokens[21])
    // which contains the token field and its value in a JSON
    // format + converting the string into a JSON object
    std::string auth_field = tokens[21];
    nlohmann::json json_object = nlohmann::json::parse(auth_field);

    // obtaining the value of the token
    std::string access_token = json_object["token"];

    return access_token;
}

// function used to print all the books added by a user, in JSON format
// (get_books)
void print_books(std::string response) {
    // separating the response by the standard HTTP delimiter
    std::vector<std::string> tokens;
    tokens = split_by_delimiter(response, "\r\n");

    // accesing the last line of the HTTP response (tokens[21])
    // which contains the JSON array of books as a string +
    // converting the string into a JSON array of objects
    std::string books_string = tokens[21];
    nlohmann::json json_array = nlohmann::json::parse(books_string);

    // printing the books with an indendation of four spaces for visibility
    std::string books = json_array.dump(4);
    std::cout << books << "\n";
}

// function used to print a single book from the user library, in JSON
// format (get_book ID)
void print_book(std::string response) {
    // separating the response by the standard HTTP delimiter
    std::vector<std::string> tokens;
    tokens = split_by_delimiter(response, "\r\n");

    // accesing the last line of the HTTP response (tokens[21])
    // which contains the required book in JSON format as a string +
    // converting the string into a JSON object
    std::string book_string = tokens[21];
    nlohmann::json json_book = nlohmann::json::parse(book_string);

    // printing the book with an indendation of four spaces for visibility
    std::string book = json_book.dump(4);
    std::cout << book << "\n";
}

// helper function to determine if a string contains only digits
// (used for checking the validity of user input)
bool valid_integer(std::string string) {
    if (string.empty())
        return false;

    for (char ch : string)
        if (!std::isdigit(ch)) 
            return false;

    return true;
}

// helper function to determine if a string contains only alfanumerical
// characters (used for checking the validity of user input)
bool valid_username(std::string string) {
    if (string.empty())
        return false;

    for (char ch : string)
        if (!isalnum(ch))
            return false;

    return true;
}

// helper function to determine if a string contains only letters and
// spaces (used for checking the validity of user input)
bool valid_name(std::string string) {
    if (string.empty())
        return false;

    for (char ch : string)
        if (std::isdigit(ch)) 
            return false;

    return true;
}

int main () {
    // variables used for HTTP communication
    char *message;
    char *response;

    // JSON object used for sending data through HTTP
    nlohmann::json json_object;

    // fixed server address, content type and port
    std::string server_addr = "34.246.184.49";
    std::string content_type = "application/json";
    int port = 8080;

    // other variables to store important data
    int sockfd, error_code;
    std::string command, auth_cookie, access_token, url;

    while (1) {
        // reading the given command
        std::getline(std::cin, command);

        // register command case
        if (command == "register") {
            // reading the user and pass from stdin
            std::string username, password;

            std::cout << "username=";
            std::getline(std::cin, username);

            std::cout << "password=";
            std::getline(std::cin, password);

            // validating the username
            if (!valid_username(username)) {
                std::cout << "ERROR - invalid username";
                continue;
            }

            // storing the values in a JSON object and preparing
            // a char* to the JSON dump to send through HTTP
            json_object["username"] = username;
            json_object["password"] = password;

            std::string json_string = json_object.dump();
            char *body_data = (char *)json_string.c_str();

            // setting up the url
            url = "/api/v1/tema/auth/register";

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_post_request((char *)server_addr.c_str(), (char *)url.c_str(),
            (char *)content_type.c_str(), body_data, 2, NULL, 0, NULL);
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 201)
                std::cout << "SUCCESS - user registered\n";

            if (error_code == 400)
                std::cout << "ERROR - username taken\n";

            free(response);
            continue;
        }

        // login command case
        if (command == "login") {
            // prevent logging in again if already logged in
            if (!auth_cookie.empty()) {
                std::cout << "ERROR - already logged in\n";
                continue;
            }

            // reading the user and pass from stdin
            std::string username, password;

            std::cout << "username=";
            std::getline(std::cin, username);

            std::cout << "password=";
            std::getline(std::cin, password);

            // validating the username
            if (!valid_username(username)) {
                std::cout << "ERROR - invalid username";
                continue;
            }

            // storing the values in a JSON object and preparing
            // a char* to the JSON dump to send through HTTP
            json_object["username"] = username;
            json_object["password"] = password;

            std::string json_string = json_object.dump();
            char *body_data = (char *)json_string.c_str();

            // setting up the url
            url = "/api/v1/tema/auth/login";

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_post_request((char *)server_addr.c_str(), (char *)url.c_str(),
            (char *)content_type.c_str(), body_data, 2, NULL, 0, NULL);
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                std::cout << "SUCCESS - user logged in\n";

                // saving the authentication cookie
                auth_cookie = get_cookie(string_response);
            }

            if (error_code == 400)
                std::cout << "ERROR - wrong credentials\n";

            free(response);
            continue;
        }

        // enter_library command case
        if (command == "enter_library") {
            // prevents the user from accessing the library if not logged in
            if (auth_cookie.empty()) {
                std::cout << "ERROR - user not logged in\n";
                continue;
            }

            // prevents the user from reaccessing the library if access had already
            // been granted
            if (!access_token.empty()) {
                std::cout << "ERROR - user already has access\n";
                continue;
            }

            // setting up the url
            url = "/api/v1/tema/library/access";

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_get_request((char *)server_addr.c_str(), (char *)url.c_str(), 
                    NULL, (char *)auth_cookie.c_str(), 1, NULL);
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                std::cout << "SUCCESS - library access granted\n";

                // obtaining the JWT token to prove that we have access
                // to the library in future commands
                access_token = get_token(string_response);
            }

            if (error_code == 400)
                std::cout << "ERROR - wrong credentials\n";

            free(response);
            continue;
        }

        // get_books command case
        if (command == "get_books") {
            // prevents the user from getting the books if access has not
            // been granted
            if (access_token.empty()) {
                std::cout << "ERROR - user does not have access\n";
                continue;
            }

            // setting up the url
            url = "/api/v1/tema/library/books";

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_get_request((char *)server_addr.c_str(), (char *)url.c_str(), 
                    NULL, NULL, 0, (char *)access_token.c_str());
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                // printing all the books
                print_books(response);
            } else {
                std::cout << "ERROR - operation failed";
            }

            free(response);
            continue;
        }

        // get_book command case
        if (command == "get_book") {
            // prevents the user from getting the book if access has not
            // been granted
            if (access_token.empty()) {
                std::cout << "ERROR - user does not have access\n";
                continue;
            }

            // reading the ID from stdin
            std::string id;

            std::cout << "id=";
            std::getline(std::cin, id);

            // validating the user input
            if (!valid_integer(id)) {
                std::cout << "ERROR - wrong input\n";
                continue;
            }

            // setting up the url
            url = "/api/v1/tema/library/books/" + id;

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_get_request((char *)server_addr.c_str(), (char *)url.c_str(), 
                    NULL, NULL, 0, (char *)access_token.c_str());
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                // printing all the books
                print_book(string_response);
            } else if (error_code == 404) {
                std::cout << "ERROR - non-existent id";
            } else {
                std::cout << "ERROR - operation failed";
            }

            free(response);
            continue;
        }

        // add_book command case
        if (command == "add_book") {
            // prevents the user from adding the book if access has not
            // been granted
            if (access_token.empty()) {
                std::cout << "ERROR - user does not have access\n";
                continue;
            }

            // reading the book data from stdin
            std::string title, author, genre, publisher, page_count;

            std::cout << "title=";
            std::getline(std::cin, title);

            std::cout << "author=";
            std::getline(std::cin, author);

            std::cout << "genre=";
            std::getline(std::cin, genre);

            std::cout << "publisher=";
            std::getline(std::cin, publisher);

            std::cout << "page_count=";
            std::getline(std::cin, page_count);

            // validates the input
            if (!valid_integer(page_count)) {
                std::cout << "ERROR - wrong input\n";
                continue;
            }

            if (!valid_name(author)) {
                std::cout << "ERROR - wrong input\n";
                continue;
            }

            if (!valid_name(genre)) {
                std::cout << "ERROR - wrong input\n";
                continue;
            }

            // storing the values in a JSON object and preparing
            // a char* to the JSON dump to send through HTTP
            json_object["title"] = title;
            json_object["author"] = author;
            json_object["genre"] = genre;
            json_object["publisher"] = publisher;
            json_object["page_count"] = stoi(page_count);

            std::string json_string = json_object.dump();
            char *body_data = (char *)json_string.c_str();

            // setting up the url
            url = "/api/v1/tema/library/books";

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_post_request((char *)server_addr.c_str(), (char *)url.c_str(),
            (char *)content_type.c_str(), body_data, 2, NULL, 0, (char *)access_token.c_str());
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                std::cout << "SUCCESS - book added\n";
            } else {
                std::cout << "ERROR - operation failed";
            }

            free(response);
            continue;
        }

        // delete_book command case
        if (command == "delete_book") {
            // prevents the user from getting the book if access has not
            // been granted
            if (access_token.empty()) {
                std::cout << "ERROR - user does not have access\n";
                continue;
            }

            // reading the ID from stdin
            std::string id;

            std::cout << "id=";
            std::getline(std::cin, id);

            // validating the user input
            if (!valid_integer(id)) {
                std::cout << "ERROR - wrong input\n";
                continue;
            }

            // setting up the url
            url = "/api/v1/tema/library/books/" + id;

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_delete_request((char *)server_addr.c_str(), (char *)url.c_str(), 
                    NULL, NULL, 0, (char *)access_token.c_str());
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                std::cout << "SUCCESS - book deleted";
            } else if (error_code == 404) {
                std::cout << "ERROR - non-existent id";
            } else {
                std::cout << "ERROR - operation failed";
            }

            free(response);
            continue;
        }

        // logout command case
        if (command == "logout") {
            // prevents the user from logging out if not logged in
            if (auth_cookie.empty()) {
                std::cout << "ERROR - user not logged in\n";
                continue;
            }

            // setting up the url
            url = "/api/v1/tema/auth/logout";

            // opening the connection
            sockfd = open_connection((char *)server_addr.c_str(), port, AF_INET, SOCK_STREAM, 0);

            // preparing and sending the message
            message = compute_get_request((char *)server_addr.c_str(), (char *)url.c_str(), 
                    NULL, (char *)auth_cookie.c_str(), 1, (char *)access_token.c_str());
            send_to_server(sockfd, message);

            // obtaining the HTTP response
            response = receive_from_server(sockfd);

            // closing the connection
            close_connection(sockfd);

            std::string string_response(response);

            // getting and interpreting the error code
            error_code = get_error_code(string_response);

            if (error_code == 200) {
                std::cout << "SUCCESS - logged out\n";

                // clearing the previously stored authentication cookie and
                // access token for eventual future logins
                auth_cookie.clear();
                access_token.clear();
            } else {
                std::cout << "ERROR - operation failed";
            }

            free(response);
            continue;
        }

        // exit command case
        if (command == "exit") {
            break;
        }
        
        // unknown command case
        std::cout << "ERROR - Invalid command\n";
    }
    return 0;
}
