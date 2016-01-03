#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static const int PORT = 3000;
static const int BUF_SIZE = 1024 * 1024;
static const int MAX_SIZE = 1024 * 1024;
static const short HTTP_STATUS_OK = 200;
static const short HTTP_STATUS_NOT_FOUND = 404;
static const char* HTTP_VERSION = "HTTP/1.1";

typedef struct {
  char* key;
  char* value;
} header;

typedef struct {
  char* method;
  char* target;
  short version_major;
  short version_minor;
} request;

typedef struct {
  short status;
  char* body;
} response;

request* new_request() {
  request* request_ptr = calloc(1, sizeof(request));
  request_ptr->method = calloc(1, MAX_SIZE);
  request_ptr->target = calloc(1, MAX_SIZE);
  return request_ptr;
}

response* new_response() {
  response* response_ptr = calloc(1, sizeof(response));
  response_ptr->body = calloc(1, MAX_SIZE);
  return response_ptr;
}

void destroy_request(request* request_ptr) {
  free(request_ptr->method);
  free(request_ptr->target);
  free(request_ptr);
}

void destroy_response(response* response_ptr) {
  free(response_ptr->body);
  free(response_ptr);
}

void make_response(request* request_ptr, response* response_ptr) {
  char relative_path[MAX_SIZE];
  sprintf(relative_path, ".%s", request_ptr->target);
  int file = open(relative_path, O_RDONLY);

  if (file < 0) {
    response_ptr->status = HTTP_STATUS_NOT_FOUND;
  } else {
    response_ptr->status = HTTP_STATUS_OK;
    read(file, response_ptr->body, MAX_SIZE - 1);
    close(file);
  }
  printf("[RESPONSE] %s %s %hd\n", request_ptr->method, request_ptr->target, response_ptr->status);
}

void parse_request(char* buffer, request* request_ptr) {
  sscanf(buffer, "%s %s %hd/%hd",
    request_ptr->method,
    request_ptr->target,
    &request_ptr->version_major,
    &request_ptr->version_minor
  );
  printf("[REQUEST] %s %s\n", request_ptr->method, request_ptr->target);
}

void error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void send_response(int client_socket, response* response_ptr) {
  char buffer[2048];
  sprintf(buffer, "%s %hd \r\n\r\n%s", HTTP_VERSION, response_ptr->status, response_ptr->body);
  int n = write(client_socket, buffer, strnlen(buffer, 2048));
  if (n < 0) {
    error("Error writing to socket\n");
  }
}

int make_server_socket() {
  int sock;
  struct sockaddr_in serv_addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    error("Error opening socket");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);

  if (bind(sock, (struct sockaddr *) (&serv_addr), sizeof(serv_addr)) < 0) {
    error("Cannot bind to port");
  }

  if (listen(sock, 5)) {
    error("Unable to listen");
  }

  return sock;
}

void handle_request(int client_socket) {
  int num_bytes;
  char* buffer_ptr;

  buffer_ptr = calloc(1, BUF_SIZE);
  if (buffer_ptr == NULL) {
    error("Error allocating buffer");
  }

  num_bytes = read(client_socket, buffer_ptr, BUF_SIZE - 1);

  if (num_bytes < 0) {
    error("Error reading from socket");
  }

  buffer_ptr[num_bytes] = 0;

  request* request_ptr = new_request();
  parse_request(buffer_ptr, request_ptr);
  free(buffer_ptr);

  // Build response
  response* response_ptr = new_response();
  make_response(request_ptr, response_ptr);
  send_response(client_socket, response_ptr);

  close(client_socket);
  destroy_request(request_ptr);
  destroy_response(response_ptr);
}

void run_server(int server_socket) {
  socklen_t client_addr_len;
  int client_socket;
  struct sockaddr_in cli_addr;
  client_addr_len = sizeof(cli_addr);

  while(1) {
    client_socket = accept(server_socket, (struct sockaddr *) &cli_addr, &client_addr_len);

    if (client_socket < 0) {
      error("Error accepting connection");
    }

    handle_request(client_socket);
  }
}

int main(int argc, char* argv[]) {
    int server_socket;

    server_socket = make_server_socket();
    run_server(server_socket);
    // TODO: close server socket in signal handler
    //close(server_socket);

    return EXIT_SUCCESS;
}
