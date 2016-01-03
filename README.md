## Terrible Webservers

This is just for fun, I'm awful at C.

The idea here is to write and modify several http servers that exemplify various stages of
sophistication in an attempt to solve the C10K -> C10M problems, learn about C and the http spec.

It might also be fun to benchmark some of these.

### Order of business

 1. Write a server that serves one http request at a time
 2. Modify the server to fork on each request (small and easy change)
 3. Write a server that uses a thread per request
 3. Modify the threaded server to use a buffer pool to avoid unnecessary alloc/dealloc
 4. Modify the threaded server to use a thread pool
 5. Write a server that is event driven using epoll/kqueue
