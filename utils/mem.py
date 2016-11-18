#!/usr/bin/python

import os

if __name__ == "__main__":
    os.system("valgrind --tool=memcheck --leak-check=full --track-origins=yes ./build/condor test/bug.cb > memoryLogs.txt 2>&1")