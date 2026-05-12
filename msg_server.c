#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>

typedef struct {
    char text[64];
} message_t;

int main() {

    name_attach_t *attach;
    attach = name_attach(NULL, "ipc_server", 0);

    if (attach == NULL) {
        perror("name_attach failed");
        return 1;
    }

    printf("Server is ready and waiting\n");
    fflush(stdout);

    message_t msg;
    int rcvid;

    while (1) {
        rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);
        printf("Server received: %s\n", msg.text);
        fflush(stdout);
        MsgReply(rcvid, 0, "OK", 3);
    }

    return 0;
}