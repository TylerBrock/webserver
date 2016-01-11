#include <stdlib.h>
#include <stdio.h>

#include "request.h"
#include "constants.h"

request* new_request() {
  request* request_ptr = calloc(1, sizeof(request));
  request_ptr->method = calloc(1, MAX_SIZE);
  request_ptr->target = calloc(1, MAX_SIZE);
  return request_ptr;
}

void destroy_request(request* request_ptr) {
  free(request_ptr->method);
  free(request_ptr->target);
  free(request_ptr);
}

void parse_request(char* buffer, request* request_ptr) {
  sscanf(buffer, "%s %s %hd/%hd",
    request_ptr->method,
    request_ptr->target,
    &request_ptr->version_major,
    &request_ptr->version_minor
  );
}
