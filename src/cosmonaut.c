#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

struct global_config* configuration;
sig_atomic_t server_socket_fd;

#include "log.h"
#include "signals.h"
#include "networking.h"
#include "base_request_handler.h"
#include "configuration.h"
#include "action.h"


void action_index(http_request* request, http_response *response) {
  render_file(response, "index.html");
}

void action_about(http_request* request, http_response *response) {
  render_file(response, "about.html");
}

int main(int argc, char *argv[]) {
  int new_connection_fd;

  load_configuration(argc, argv);

  route("/", action_index);
  route("/about", action_about);

  server_socket_fd = bind_server_socket_fd();
  setup_signal_listeners(server_socket_fd);

  while(1) {
    new_connection_fd = accept_connection();

    if (!fork()) {
      close(server_socket_fd);

      handle_request(new_connection_fd);

      free_configuration();
      exit(0);
    }

    close(new_connection_fd);
  }

  return 0;
}
