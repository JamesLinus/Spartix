#Spartix
## An x86_64 Operating system
Spartix is an operating system, designed to run on x86_64.
It's designed to comply to the POSIX standard, implement some of the standard Unix API's while learning from the mistakes UNIX did. It folows the System V ABI.
All the system calls are exposed through int 0x80, although it will be changed to syscall over the time.
Spartix's purpose is to be function-heavy, and light-weight when possible. Bloat is a *NOT* a necessary evil for functionality, as software can be very functional, while remaining light-weight. Spartix will never turn into a glibc or a systemd, as those pieces of software add unnecessary crap to its binary.
## Development
If you want to help out in the development of Spartix, you can try joining #spartix on freenode, or fixing issues on GitHub's issue tracker.
