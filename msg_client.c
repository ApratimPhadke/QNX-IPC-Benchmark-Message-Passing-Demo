#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

typedef struct {
    char text[64];
} message_t;

int main() {

    int coid;
    coid = name_open("ipc_server", 0);

    if (coid == -1) {
        perror("name_open failed");
        return 1;
    }

    message_t msg;
    strcpy(msg.text, "Hello from client");

    char reply[64];
    int status;

    status = MsgSend(coid, &msg, sizeof(msg), reply, sizeof(reply));

    if (status == -1) {
        perror("MsgSend failed");
        return 1;
    }

    printf("Client got reply: %s\n", reply);
    fflush(stdout);

    name_close(coid);
    return 0;
}