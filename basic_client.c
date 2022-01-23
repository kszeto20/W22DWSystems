#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  while (1){
    char *in = malloc(BUFFER_SIZE);
    printf("Send the server something: ");

    fgets(in, BUFFER_SIZE, stdin);
    write(to_server, in, BUFFER_SIZE);
    printf("Client: Wrote %s\n", in);

    char *out = malloc(BUFFER_SIZE);
    read(from_server, out, BUFFER_SIZE);
    printf("Client: Got back from server: %s\n", out);
    free(in);
    free(out);
  }

  return 0;
}
