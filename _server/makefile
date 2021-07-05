all:
    ppc-amigaos-c++ CSNode.cpp -c
    ppc-amigaos-c++ Server.cpp CSNode.o -o Server -lpthreads
    c++ CSNode.cpp -c
    c++ main3.cpp CSNode.o -o CSNode -pthread
