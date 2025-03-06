#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.hpp"
#include "requests.hpp"

// function implementations taken from the lab, with slight adjustments
// to the cookie parameter (an array of cookies is not necessary, as we
// only need to send one cookie, the authentication cookie) and the addition
// of a new parameter, auth_bearer (stores the JWT token)

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookie, int cookies_count, char *auth_bearer)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add headers and/or cookies, according to the protocol format
    if (cookies_count) {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // add the authentication bearer header, with the JWT token
    if (auth_bearer) {
        sprintf(line, "Authorization: Bearer %s", auth_bearer);
        compute_message(message, line);
    }

    // add final new line
    compute_message(message, "");

    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            int body_data_fields_count, char *cookie, int cookies_count,
                            char *auth_bearer)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add necessary headers (Content-Type and Content-Length are mandatory)
    // in order to write Content-Length you must first compute the message size
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    // add headers and/or cookies, according to the protocol format
    if (cookies_count) {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // add the authentication bearer header, with the JWT token
    if (auth_bearer) {
        sprintf(line, "Authorization: Bearer %s", auth_bearer);
        compute_message(message, line);
    }

    // add new line at end of header
    compute_message(message, "");

    // add the actual payload data
    compute_message(message, body_data);

    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char *cookie, int cookies_count, char *auth_bearer) {
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // add headers and/or cookies, according to the protocol format
    if (cookies_count) {
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }

    // add the authentication bearer header, with the JWT token
    if (auth_bearer) {
        sprintf(line, "Authorization: Bearer %s", auth_bearer);
        compute_message(message, line);
    }

    // add final new line
    compute_message(message, "");

    return message;
}
