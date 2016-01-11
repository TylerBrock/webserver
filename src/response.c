#include <stdint.h>
#include <stdlib.h>

#include "http.h"
#include "response.h"

static const uint32_t MAX_SIZE = 1024 * 1024;

response* new_response() {
  response* response_ptr = calloc(1, sizeof(response));
  response_ptr->body = calloc(1, MAX_SIZE);
  return response_ptr;
}

void destroy_response(response* response_ptr) {
  free(response_ptr->body);
  free(response_ptr);
}
