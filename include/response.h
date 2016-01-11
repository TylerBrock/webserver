#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct {
  http_status status;
  char* body;
} response;

response* new_response();
void destroy_response(response* response_ptr);

#endif
