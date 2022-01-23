#include "pipe_networking.h"

/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  mkfifo(WKP, 0644);
  int from_client = open(WKP, O_RDONLY);
  remove(WKP);
  return from_client;
}

/*=========================
  server_connect
  args: int from_client
  handles the subserver portion of the 3 way handshake
  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  // message storage
  char in[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    in[i] = '\0';
  }

  // wait for client send in
  read(from_client, in, BUFFER_SIZE);
  printf("SUB: client sent %s\n", in);

  // open the pipe
  int to_client = open(in, O_WRONLY);
  // send conf message
  write(to_client, ACK, BUFFER_SIZE);
  printf("SUB: sent ACK message back to client\n");
  // wait for client response + grab it
  read(from_client, in, BUFFER_SIZE);
  // check for confirm message

  if (strcmp(in, ACK) == 0) {
    printf("SUB: Client connection successful\n");
  } else {
    printf("SUB: Connection failed\n");
    return 0;
  }

  return to_client;
}

/*=========================
  server_handshake
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {

  // opening comms with client
  mkfifo(WKP, 0644);
  int from_client = open(WKP, O_RDONLY);
  printf("Server: WKP opened\n");

  // message storage
  char in[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    in[i] = '\0';
  }

  // waits for input now
  read(from_client, in, BUFFER_SIZE);
  printf("Server: Reading from client: %s\n", in);

  // finished reading closes read side
  remove(WKP);
  printf("Server: WKP removed\n");

  // setting up secret pipe for confirmation
  *to_client = open(in, O_WRONLY);
  printf("Server: Secret Pipe opened\n");

  //printf("got here\n");
  // client is waiting for confirmation
  write(*to_client, ACK, strlen(ACK));
  printf("Server: Sent back: %s\n", ACK);

  //confirm message from client storage
  char got[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    got[i] = '\0';
  }
  //printf("Got here 2\n");
  // waiting for client response
  read(from_client, got, BUFFER_SIZE);
  //printf("Got here 3\n")
  // checking confirm message
  if ((strcmp(got, ACK)) == 0) {
    printf("Server: Three way handshake established\n");
  } else {
    printf("Client: Did not recieve %s, received %s\n", ACK, got);
    return 0;
  }
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {

  mkfifo(WKP, 0644);
  int from_server;

  char mess[BUFFER_SIZE];
  for (int i = 0; i < BUFFER_SIZE; i++) {
    mess[i] = '\0';
  }
  sprintf(mess, "%d", getpid());
  mkfifo(mess, 0644);


  *to_server = open(WKP, O_WRONLY);
  printf("Client: WKP opened\n");
  write(*to_server, mess, BUFFER_SIZE);
  printf("Client: Sent %s back to the server\n", mess);

  // confirm message
  from_server = open(mess, O_RDONLY);
  printf("Private pipe opened\n");
  char confirm[BUFFER_SIZE];

  for (int i = 0; i < BUFFER_SIZE; i++) {
    confirm[i] = '\0';
  }
  read(from_server, confirm, BUFFER_SIZE);
  if ((strcmp(confirm, ACK)) == 0) {
    printf("Client: Handshake success!\n");
  } else {
    printf("Client: Did not recieve %s, received %s\n", ACK, confirm);
    return 0;
  }

  remove(mess);
  write(*to_server, confirm, BUFFER_SIZE);
  printf("Client: Three way handshake established\n");
  return from_server;
}
