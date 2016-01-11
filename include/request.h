#ifndef REQUEST_H
#define REQUEST_H

typedef struct {
  char* method;
  char* target;
  short version_major;
  short version_minor;
} request;

request* new_request();
void destroy_request(request* request_ptr);
void parse_request(char* buffer, request* request_ptr);

#endif
