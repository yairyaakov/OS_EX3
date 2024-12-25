#ifndef REACTOR_HPP
#define REACTOR_HPP
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/select.h>

typedef void* (*proactorFunc)(void * sockfd);

    pthread_t startProactor (int sockfd, proactorFunc threadFunc);
    int stopProactor(pthread_t tid);
 

#endif // REACTOR_HPP