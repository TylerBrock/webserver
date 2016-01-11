#include <stdint.h>

typedef enum { OK, NOT_FOUND, ERROR } http_status;
static const char* HTTP_STATUS_STRING[] = { "OK", "Not Found", "Internal Server Error" };
static const uint16_t HTTP_STATUS_CODE[] = { 200, 404, 500 };
static const char* HTTP_VERSION = "HTTP/1.1";
