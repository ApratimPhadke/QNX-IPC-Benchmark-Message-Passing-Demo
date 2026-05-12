# QNX IPC Benchmark — Message Passing Demo

## Project Overview

This project demonstrates the core IPC (Inter-Process Communication) mechanism of the QNX Neutrino RTOS using:

* `MsgSend()`
* `MsgReceive()`
* `MsgReply()`

The project creates:

* A **server process** waiting for messages
* A **client process** sending messages synchronously

This is one of the most important architectural concepts in QNX RTOS.

---

## What This Project Demonstrates

### QNX Native Message Passing

QNX uses kernel-level synchronous message passing instead of traditional Linux-style IPC.

Communication flow:

```text
Client → Kernel → Server
Client ← Kernel ← Server Reply
```

The client blocks inside `MsgSend()` until the server replies.

---

## ELI10 Explanation

Imagine:

* Client = person making phone call
* Server = person answering call
* QNX Kernel = telephone network

The client:

1. calls server
2. waits
3. receives reply
4. continues execution

---

## Technologies Used

* QNX SDP 8.0
* QEMU
* QNX Neutrino RTOS
* C Programming
* QNX IPC APIs
* Cross Compilation (`qcc`)

---

# Project Structure

```text
qnx_projects/
│
├── qnx_ipc/
│   ├── msg_server.c
│   ├── msg_client.c
│   ├── msg_server
│   └── msg_client
│
├── qnx_vm/
│   ├── local/
│   │   └── snippets/
│   │       └── system_files.custom
│   │
│   └── output/
│       └── disk-qemu
```

---

# Step 1 — Install QEMU

## Ubuntu

```bash
sudo apt update
sudo apt install qemu-system-x86 tree
```

Verify:

```bash
qemu-system-x86_64 --version
```

---

# Step 2 — Install QNX SDP

Download:

* QNX SDP 8.0

Official website:

[QNX Software Center](https://www.qnx.com/download/?utm_source=chatgpt.com)

Install:

* QNX SDP
* QNX Momentics tools
* QNX command-line tools

---

# Step 3 — Verify QNX Tools

Check:

```bash
which qcc
which mkqnximage
```

Expected:

```text
/home/username/qnx800/host/common/bin/mkqnximage
```

---

# Step 4 — Create IPC Project

## Create Folder

```bash
mkdir -p ~/qnx_projects/qnx_ipc
cd ~/qnx_projects/qnx_ipc
```

---

# Step 5 — Create Server

Create:

```bash
nano msg_server.c
```

Paste:

```c
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

        rcvid = MsgReceive(
                    attach->chid,
                    &msg,
                    sizeof(msg),
                    NULL);

        printf("Server received: %s\n", msg.text);
        fflush(stdout);

        MsgReply(
            rcvid,
            0,
            "OK",
            3);
    }

    return 0;
}
```

---

# Step 6 — Create Client

Create:

```bash
nano msg_client.c
```

Paste:

```c
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

    status = MsgSend(
                coid,
                &msg,
                sizeof(msg),
                reply,
                sizeof(reply));

    if (status == -1) {
        perror("MsgSend failed");
        return 1;
    }

    printf("Client got reply: %s\n", reply);

    fflush(stdout);

    name_close(coid);

    return 0;
}
```

---

# Step 7 — Compile Using QNX Compiler

```bash
qcc msg_server.c -o msg_server
qcc msg_client.c -o msg_client
```

Verify:

```bash
ls -lh
```

---

# Step 8 — Add Binaries Into QNX Image

Edit:

```bash
nano ~/qnx_projects/qnx_vm/local/snippets/system_files.custom
```

Add:

```text
/msg_server=/home/USERNAME/qnx_projects/qnx_ipc/msg_server
/msg_client=/home/USERNAME/qnx_projects/qnx_ipc/msg_client
```

Replace:

```text
USERNAME
```

with your Linux username.

---

# Step 9 — Rebuild QNX Image

```bash
cd ~/qnx_projects/qnx_vm

rm -rf output

mkqnximage --type=qemu
```

This rebuilds:

* filesystem
* RTOS image
* boot image
* embedded binaries

---

# Step 10 — Boot QNX VM

```bash
cd output

qemu-system-x86_64 \
-M pc-i440fx-7.2 \
-cpu Haswell \
-m 512 \
-hda disk-qemu \
-machine acpi=off,hpet=off \
-nographic
```

Wait for:

```text
#
```

prompt.

---

# Step 11 — Run Server

```bash
/system/msg_server &
```

Expected:

```text
Server is ready and waiting
```

The `&` runs the process in background.

---

# Step 12 — Run Client

```bash
/system/msg_client
```

Expected output:

```text
Server received: Hello from client
Client got reply: OK
```

---

# Successful Output

![Image](https://images.openai.com/static-rsc-4/FGgfYuZNvaHmN8HUh-tdHHoA4sDMqwnPhFC0vCa_nMOUQU6cJnroa_OZX_22RwGbXxQNvKgw_j0AuCgElAk-gbQoHFztA1-bUUYLKZNpLR-CaqwBeg84l8_uMyOo_EEpRi6scACRfeXMliXXw28zCAGwkghHb3IWRb8m7ti0lQWvJ92YWdhK9dxoKLbwQZ6N?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/-w1CK2lRtRxUt--tLTDkLmgE1u8Tau0H2B8ByNhXfKWlFO1ZinI_ym9Ot29eCSLEdH7ZVa4mPZsD7Z0i630bYjkeIODkLjRGbSS5hW6i-ACj41Tj8mOBNGCnN4zlodEK8vV_pt1n4sFehVqMXLwbi8sSKxWh7B4iKHfE28pse8nBbP8PrV10EHZ4zZ6vmCRS?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/BfHinqFBplxXVYLYEdglfSl8I3u10NHizE0hmwkTVNaQa17aJg5vHjCv5pG378PRJFebqDFRLnPN6qeTte7KZI0vh9erqdRCduM6MGhzjgtDJX_BGmTlq-fYlH_Dyv451Fe5YQ2BNvp_Pu2T75pwRJBbzhG9l35GT7ARHTNODXWpJY9KQM6vJeBJkIgjBjYd?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/uZhcP02b-X81Vgc294dH31X3zuzdYYdk6ifbm2LDR8cMemYH9QgQ6GsE32uFmbk8EnLU6eDNpDaMyk7RWJi7paeaiypuo_UBLsM08PD04PkhFtJTMq8M1sKLoxOy1EUJItWtYruQaPqMElbHjhW0iuOachKj3ATJncXHCeE-KTX9VVZZrTSBpOfebHEwZcqg?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/zt5l9b6IOJ6_yZwAxTqA6QPQKLxDavtJeNs8hGRc3CMvRHmJ9hfqRXJKXAPJ4p9qosYZp6QuLTNyHKBH59MLwuIPBjFgxyxN6Mt4lZIqH5sqzZNCf_2vZqxm0o7jmTIoH8omL5L3FknmDUznZ2E1hmKuZgziSonQmNxkDaiiTi_0MdqsleuKQmMhMTTg_XVN?purpose=fullsize)

![Image](https://images.openai.com/static-rsc-4/YDXYsiu1qkc98ug44tex6ZbQdG3YZl76OhHWU80thrs8X6hVUJ_vYqvYqSvk2fO_wllgrG5zTaA2OBeoB6XzANDhoZ7fb9mlzv1AlHsimgGw_lrJtxd8t8E7aJ_-yeeDAVW95e4Z9jmW4Tft468LMvBQQkJYoohv-YCkiidVMCE_F0kUdFOxX1oiM743sgAw?purpose=fullsize)

Actual project execution:

![QNX IPC Demo](sandbox:/mnt/data/b77bfa8a-8405-40d3-9cf7-47b5860d72ee.png)

---

# IPC Functions Explained

| Function        | Purpose                    |
| --------------- | -------------------------- |
| `name_attach()` | Register server name       |
| `name_open()`   | Connect client to server   |
| `MsgSend()`     | Send message synchronously |
| `MsgReceive()`  | Wait for incoming message  |
| `MsgReply()`    | Reply back to client       |

---

# Why QNX IPC Is Special

QNX message passing is:

* deterministic
* synchronous
* kernel-managed
* highly reliable

Used in:

* automotive ECUs
* avionics
* medical devices
* industrial automation
* robotics

---
