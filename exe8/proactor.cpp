#include "proactor.hpp"
#include <sys/select.h>


pthread_t startProactor(int sockfd, proactorFunc threadFunc)
{
    pthread_t tid; 
    int * clientptr= &sockfd;

    if (pthread_create(&tid, NULL, threadFunc, clientptr) != 0) {
        perror("pthread_create");
        close(sockfd);  
    }
        return tid;
}

int stopProactor(pthread_t tid)
{
    return pthread_cancel(tid);
    return 0;
}