#!/bin/sh

valgrind --track-origins=yes --tool=memcheck --leak-check=yes --show-reachable=yes --suppressions=valgrind/suppress.s ./bin/test_lib

