#ifndef _REQUESTS_
#define _REQUESTS_

// computes and returns a GET request string (query_params, cookies
// and auth_bearer can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							char *cookie, int cookies_count, char *auth_bearer);

// computes and returns a POST request string (cookies and auth_bearer can be NULL
// if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
							int body_data_fields_count, char* cookie, int cookies_count,
							char *auth_bearer);

// computes and returns a DELETE request string (cookies and auth_bearer can be NULL
// if not needed)
char *compute_delete_request(char *host, char *url, char *query_params,
							char *cookie, int cookies_count, char *auth_bearer);

#endif
