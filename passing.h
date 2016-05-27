#ifndef PASSING_H_INCLUDED
#define PASSING_H_INCLUDED

#define DEBUG_BUFFER 1024

typedef struct
{
    int n;      /// the number of times each core should rerun the pass procedure
    int * debug;
} pass_args;


#endif // PASSING_H_INCLUDED
