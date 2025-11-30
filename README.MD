## What I did

myls.c — list files, with -l support
- Features:
 - myls → list entries in current directory
 - myls dir → list entries in dir
 - myls -l → long listing for current directory
 - myls -l dir → long listing for dir


mytail.c — efficient tail with lseek()
- Features:
 - seek near end, read a block, scan backwards to find N newlines, then print from there to EOF.
 - mytail -n <num> file


### How to build
To build all programs:
    make

To build only myls:
    make myls

To build only mytail:
    make mytail

To clean up binaries:
    make clean

Test:
- ./myls
- ./myls -l
- ./myls -l Test
- ./mytail -n 3 README.MD


### Collaborative Statements
- I used ChatGPT to help design and draft the initial structure of my myls.c and mytail.c programs, including argument parsing, use of system calls (opendir, readdir, stat, lseek, read, write), and example compile/run instructions. 
- I then modified, debugged, and tested the programs on my own, and I understand how all of the code works and could reproduce it independently without the tool.
