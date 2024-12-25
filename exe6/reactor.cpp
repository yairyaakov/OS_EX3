#include "reactor.hpp"
#include <sys/select.h>

using namespace std;

void *startReactor() {
    // Allocate memory for the Reactor structure
    Reactor *reactor = (Reactor *)malloc(sizeof(Reactor));
    if (!reactor) {
        perror("Failed to allocate memory for Reactor");
        return NULL;
    }

    // Initialize reactor members
    reactor->fd_size = 5;
    reactor->curr_index = 0;
    reactor->pfds = nullptr;
    reactor->funcs = nullptr;

    // Allocate memory for poll file descriptors
    reactor->pfds = (pollfd *)malloc(sizeof(pollfd) * reactor->fd_size);
    if (!reactor->pfds) {
        perror("Failed to allocate memory for pfds");
        free(reactor);
        return NULL;
    }

    // Allocate memory for function pointers
    reactor->funcs = (reactorFunc *)malloc(sizeof(reactorFunc) * reactor->fd_size);
    if (!reactor->funcs) {
        perror("Failed to allocate memory for funcs");
        free(reactor->pfds);
        free(reactor);
        return NULL;
    }

    // Initialize function pointers to null
    memset(reactor->funcs, 0, sizeof(reactorFunc) * reactor->fd_size);
    return reactor;
}

int addFdToReactor(void *reactor, int fd, reactorFunc func)
{   
  Reactor * reactorptr= (Reactor *)reactor;

    if(!reactorptr || !func) return -1;

   if (reactorptr->curr_index == reactorptr->fd_size) {
        reactorptr->fd_size *= 2; // Double it
         reactorptr->pfds = (pollfd *) realloc(reactorptr->pfds, sizeof(reactorptr->pfds) * (reactorptr->fd_size));
        reactorptr->funcs = (reactorFunc *)realloc(reactorptr->funcs, sizeof(reactorFunc) * (reactorptr->fd_size));
        if (!reactorptr->pfds || !reactorptr->funcs) {
            return -1;
        
     }
   }

   reactorptr->pfds[reactorptr->curr_index].fd = fd;
   reactorptr->pfds[reactorptr->curr_index].events = POLLIN;
   reactorptr->funcs[reactorptr->curr_index] = func;
   (reactorptr->curr_index  )++;
    return 0;
    
}

int removeFdFromReactor(void *reactor, int fd)
{
    Reactor * reactorptr= (Reactor *)reactor;
    for(int i=0; i<reactorptr->fd_size; i++){
     if(reactorptr->pfds[i].fd==fd)
     close(reactorptr->pfds[i].fd);
    // Copy the one from the end over this one
    reactorptr->pfds[i] = reactorptr->pfds[reactorptr->fd_size-1];
    (reactorptr->fd_size)--;
    return 0;
    }
    cout<< "Fd " << fd << " not found in reactor\n";
    return -1; // Return -1 if fd not found (error handling)
}

int stopReactor(void *reactor)
{
    Reactor * reactorptr = (Reactor *)reactor;
    free(reactorptr->pfds);
    free(reactorptr->funcs);
    free(reactorptr); return 0;
}
