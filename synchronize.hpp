#ifndef SYNCHRONIZE_H
#define SYNCHRONIZE_H

typedef struct synchronize {
  int doneListening;
  int startAccepting;
  int doneAccepting;
} sync_t;

#endif