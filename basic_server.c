#include "pipe_networking.h"

int main() {
  while (1) {

    int to_client;
    int from_client;

    from_client = server_setup(); // set up initial communication with read side

    int f = fork();
    if (f == 0) {
      to_client = server_connect(from_client); // write side of pipe to send back
      while (1) {
        char * mess = malloc(BUFFER_SIZE); // to manipulate
        char * temp = malloc(BUFFER_SIZE); // to return
        read(from_client, mess, BUFFER_SIZE);
          // do stuff to the input
        int i;
        for (i = strlen(mess) - 1; i >= 0; i--){
          if (*(mess + i) != 20){
            strncat(temp, mess + i, 1);
            strcat(temp, "!");
          }
        }
        write(to_client, temp, BUFFER_SIZE);
        free(mess);
        free(temp);
        printf("Server: Client's requests have been fulfilled\n");
      }
    } else {
    close(from_client); // terminate read side
    close(to_client);
    }
  }
  return 0;
}
