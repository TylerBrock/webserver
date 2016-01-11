#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "constants.h"
#include "http.h"
#include "request.h"
#include "response.h"

#define MAX_LENGTH 1024 * 1024

static const uint16_t DEFAULT_PORT = 3000;
static const int32_t BACKLOG = 5;

volatile sig_atomic_t stop = false;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    stop = true;
  }
}

void make_response(request* request_ptr, response* response_ptr) {
  char relative_path[MAX_LENGTH];
  sprintf(relative_path, ".%s", request_ptr->target);
  int file = open(relative_path, O_RDONLY);

  if (file < 0) {
    response_ptr->status = NOT_FOUND;
  } else {
    response_ptr->status = OK;
    read(file, response_ptr->body, MAX_SIZE - 1);
    close(file);
  }
  short status_code = HTTP_STATUS_CODE[response_ptr->status];
  printf("[RESPONSE] %s %s %hd\n", request_ptr->method, request_ptr->target, status_code);
}

void error(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void send_response(int client_socket, response* response_ptr) {
  char buffer[2048] = {0};
  short status_code = HTTP_STATUS_CODE[response_ptr->status];
  const char* status_str = HTTP_STATUS_STRING[response_ptr->status];
  sprintf(buffer, "%s %hd %s\r\n\r\n%s", HTTP_VERSION, status_code, status_str, response_ptr->body);
  ssize_t n = write(client_socket, buffer, strnlen(buffer, 2048));
  if (n < 0) {
    error("Error writing to socket\n");
  }
}

int make_server_socket() {
  int sock, reuse, result;
  struct sockaddr_in serv_addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0) {
    error("Error opening socket");
  }

  reuse = true;
  result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

  if (result < 0) {
    error("Error setting socket options");
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(DEFAULT_PORT);

  result = bind(sock, (struct sockaddr *) (&serv_addr), sizeof(serv_addr));
  if (result < 0) {
    error("Cannot bind to port");
  }

  result = listen(sock, BACKLOG);
  if (result < 0) {
    error("Unable to listen");
  }

  return sock;
}

void handle_request(int client_socket) {
  ssize_t num_bytes;
  char* buffer_ptr;

  buffer_ptr = calloc(1, MAX_SIZE);
  if (buffer_ptr == NULL) {
    error("Error allocating buffer");
  }

  num_bytes = read(client_socket, buffer_ptr, MAX_SIZE - 1);

  if (num_bytes < 0) {
    error("Error reading from socket");
  }

  buffer_ptr[num_bytes] = 0;

  request* request_ptr = new_request();
  parse_request(buffer_ptr, request_ptr);
  printf("[REQUEST] %s %s\n", request_ptr->method, request_ptr->target);
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
  struct sockaddr_in cli_addr;
  client_addr_len = sizeof(cli_addr);

  while(!stop) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(server_socket, &fds);

    //TODO: Consider using pselect to avoid race condition
    int result = select(server_socket + 1, &fds, NULL, NULL, NULL);

    if (result < 0 && errno != EINTR) {
      error("Error in select");
    }

    if (stop) {
      break;
    }

    if (!result) {
      continue;
    }

    int client_socket = accept(server_socket, (struct sockaddr *) &cli_addr, &client_addr_len);

    if (client_socket < 0) {
      error("Error accepting connection");
    }

    handle_request(client_socket);
  }
}

int main() {
  int server_socket;

  signal(SIGINT, sig_handler);
  server_socket = make_server_socket();
  run_server(server_socket);
  close(server_socket);

  return EXIT_SUCCESS;
}
