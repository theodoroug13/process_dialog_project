# Process Dialog Project — Shared Memory + Semaphores + Threads (C)

A multi-process “dialog” messaging system where multiple client processes can **join the same dialog** and **send/receive messages concurrently**.

**Core idea**
- **System V shared memory** stores dialogs + messages
- **POSIX semaphores** ensure synchronization (no race conditions, no busy waiting)
- Each client process spawns **2 POSIX threads**: **sender** + **receiver**

---

## Features

- Create a new dialog or join an existing one
- Concurrent send/receive per client (2-thread model)
- Safe shared-memory access via a global mutex semaphore
- Per-dialog/per-process signaling semaphores to **wake receivers** (avoid busy waiting)
- Message lifecycle guarantees:
  - each message is delivered **exactly once to each participant**
  - message is removed after **all participants have read it**

---

## High-level design

### Shared memory layout
Shared memory stores a single `shared_data_t` structure containing:
- a global semaphore guarding shared memory critical sections
- static arrays of:
  - `dialog_t` (dialog metadata + participants)
  - `message_t` (message metadata + fixed-size text buffer)
- `next_dialog_id` and `next_msg_id` for unique IDs

### Synchronization strategy
- **One global semaphore** protects all reads/writes to shared memory
- **Per-dialog semaphores** (one per possible participant) notify the correct receiver thread when messages arrive

### Message delivery correctness
To enforce “read once by everyone then delete”:
- `read_by[i]` tracks whether participant `i` has read the message
- `readers_left` counts how many participants still need to read it  
When `readers_left == 0`, the message slot is cleared.

---

## Code structure

### Headers
- `shared_data.h` — shared-memory structures (`dialog_t`, `message_t`, `shared_data_t`)
- `dialog.h` — API: `join_dialog(...)`, `send_message(...)`, `receive_message(...)`
- `thread_funcs.h` — thread context + shared `running` flag for termination

### Sources
- `init_shared.c` (**separate executable**)  
  Initializes shared memory + metadata via `ftok`. If a segment already exists, it is removed and recreated.
- `dialog.c`  
  - `join_dialog`: create when `< 0`, otherwise join existing; prevents duplicate join  
  - `send_message`: verifies membership, allocates a free message slot, writes message, signals participants  
  - `receive_message`: verifies membership, copies message, updates `read_by[]/readers_left`, clears message when fully read
- `thread_funcs.c`  
  - `sender_thread`: reads stdin and calls `send_message`, handles `TERMINATE`  
  - `receiver_thread`: waits on semaphore, calls `receive_message`, prints message; on `TERMINATE` cancels sender thread
- `main.c`  
  Builds client executable `exec`: attaches shared memory, starts sender/receiver threads, then joins and detaches.

---

## Build & run

A `Makefile` is provided.

```bash
make
cd build
./init_shared_memory
```
Then in different terminals:
```bash
# create a new dialog
./exec -1

# join an existing dialog
./exec <dialog_id>
```
Type messages and press Enter to send.

To terminate a dialog/client, send:
```bash
TERMINATE
```
## Notes / limitations

-  Uses static arrays for dialogs/messages (bounded capacity by design)

-  Message text is fixed-size (stored directly in shared memory)

-
-  Run init_shared_memory once before starting clients (it resets shared memory state)
