# CGI Implementation TODO

> 🟦 **SubnetMaster (you)** — network, poll, pipes, ServerManager
> 🟨 **Cooker (teammate)** — HTTP parsing, routing, response formatting
> 🟥 **Architect (teammate)** — config file parsing, ServerConfig struct

---

## ✅ Done
- **`CgiInfo` struct in `Client.hpp`**
  - `bool isCgi`, `int pipeRead`, `int pipeWrite`, `pid_t pid`, `time_t start_time`
  - Initialized in both constructors, copy constructor, and `operator=`
  - `getCgiInfo()` getter added

- **`CgiHandler()` coded and tested (`CgiHandler.cpp`)**
  - `pipe(pipeIn)` + `pipe(pipeOut)`
  - Child: `dup2`, `execve(interpreter, argv, envp)`, `exit(1)` on failure
  - Parent: closes unused ends, returns `pipeOut[0]`
  - `envp` built via `vector<string>` (lifetime) + `vector<char*>` (pointers)
  - Tested with `test_cgi.cpp` + `hello.py` ✅

- **`handleRequest()` updated in `ServerManager`**
  - Calls cooker (TODO) → reads `getCgiInfo()`
  - If `isCgi == true`:
    - Adds `pipeRead` to `_pollFds` (POLLIN) + `_cgiReadFds[pipeRead] = clientFd`
    - If `pipeWrite != -1` (POST): adds `pipeWrite` to `_pollFds` (POLLOUT) + `_cgiWriteFds[pipeWrite] = clientFd`
  - Else → `_pollFds[idx].events = POLLOUT`

- **`_cgiReadFds` and `_cgiWriteFds` declared in `ServerManager.hpp`**

---

## � SubnetMaster — To Do

### A. Add `writeCgiBody()` in `ServerManager`
- Called from `run()` when `POLLOUT` fires on a `_cgiWriteFds` fd
- `write(pipeWrite, body.c_str() + offset, remaining)` — chunk by chunk
- Track write offset (needs a `size_t _bodyWriteOffset` in `Client` or a separate map)
- When all bytes written:
  - `close(pipeWrite)`
  - Remove `pipeWrite` from `_pollFds` and `_cgiWriteFds`

### B. Add `readCgiResponse()` in `ServerManager`
- Called from `run()` when `POLLIN` fires on a `_cgiReadFds` fd
- `read(pipeRead, buf, sizeof(buf))` → `_clients[clientFd].store(chunk, RESPONSE)`
- On EOF (`read == 0`):
  - Call `cookCgiResponse(_clients[clientFd])` ← 🟨 Cooker implements this
  - `close(pipeRead)`
  - `waitpid(pid, WNOHANG)` ← reap zombie child
  - Remove `pipeRead` from `_pollFds` and `_cgiReadFds`
  - Set `clientFd` → `POLLOUT` in `_pollFds`
- On error (`read < 0`): send 500, cleanup

### F. Update `run()` in `ServerManager`
- POLLIN branch:
  ```
  if isListener       → acceptNewConnection()
  else if _cgiReadFds → readCgiResponse()
  else                → handleRequest()
  ```
- POLLOUT branch:
  ```
  if _cgiWriteFds → writeCgiBody()
  else            → sendResponse()
  ```
- Change `poll()` timeout:
  - `-1` if both `_cgiReadFds` and `_cgiWriteFds` are empty
  - `1000` (1 second) if any CGI active → enables timeout checks

### G. Add `checkCgiTimeouts()` in `ServerManager`
- Called after the `for` loop in `run()` when CGI maps non-empty
- Iterate `_cgiReadFds`, check `time(NULL) - _clients[clientFd].getCgiInfo().start_time`
- If > 5s:
  - `kill(pid, SIGKILL)` + `waitpid(WNOHANG)`
  - Close and remove `pipeRead` + `pipeWrite` from `_pollFds` and maps
  - Send 504 response → set clientFd to `POLLOUT`

### H. Add `checkClientTimeouts()` in `ServerManager`
- Called after the `for` loop in `run()`
- Iterate `_clients`, call `timeSinceLastActivity()`
- If > 30s → `closeConnection()`

### I. Complete `ServerManager.hpp` declarations
- Add: `writeCgiBody()`, `readCgiResponse()`, `checkCgiTimeouts()`, `checkClientTimeouts()`

---

## 🟨 Cooker — To Do (teammate)

### C. Code `cookCgi(Client& client, const ServerConfig& config)`
- Detects CGI from URI extension (e.g. `.py` → `/usr/bin/python3`)
- Fills `client.getCgiInfo()`: `isCgi`, `pipeRead`, `pipeWrite`, `pid`, `start_time`
- Calls `CgiHandler(data)` internally

### D. Code `cookResponse(Client& client, const ServerConfig& config)`
- Parses request from `client.getBuffer(REQUEST)`
- Routes: static file / redirect / autoindex / error page
- Stores HTTP response via `client.store(response, RESPONSE)`

### E. Code `cookCgiResponse(Client& client)`
- Receives raw CGI output already accumulated in `_responseBuffer`
- Parses CGI headers (before `\r\n\r\n`) + body (after)
- Builds full HTTP response: `HTTP/1.1 200 OK\r\n` + headers + `Content-Length` + body
- `client.clean(RESPONSE)` + `client.store(httpResponse, RESPONSE)`

---

## 🟥 Architect — To Do (teammate)

### J. Config parsing (`ServerConfig`)
- Parse `.conf` file → fill `ServerConfig` struct per virtual server
- Fields: `root`, `index`, `maxBodySize`, `allowedMethods`, `cgiExtensions`, `errorPages`, `redirections`, `autoindex`
- `ServerManager` holds `vector<ServerConfig>` → selects by `Host:` header

---

## Key Reminders
- `fork()` ONLY for CGI (subject rule)
- `read()`/`write()` on pipes ONLY after `poll()` signal (subject rule)
- `pipeOut[0]` is NOT a socket → never `accept()` or `recv()` on it, use `read()`
- `pipeIn[1]` is NOT a socket → use `write()`, not `send()`
- `Client::CgiInfo` is a nested type → use `Client::CgiInfo` outside the class
- `close(pipeWrite)` after writing → sends EOF to child's stdin
- `close(pipeRead)` after EOF → must happen in parent before `waitpid()`
- `waitpid(WNOHANG)` → avoid blocking, reap zombie without waiting
- Timeout check only runs when CGI maps non-empty (poll timeout = 1000ms)
- `_responseBuffer` used in two phases: raw CGI accumulation → replaced by formatted HTTP
