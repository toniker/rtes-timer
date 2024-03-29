#include "time.h"
#include "unistd.h"
#include "sys/time.h"
#include "stdlib.h"
#include "stdio.h"


// illustration of accumulating drift

/*
  Tasks:
    - experiment with different computations and their impact on time difference
    - try to run the program with low/high "background" (other processes on the computer)
      does it have impact on the accumulation of drift?
    - create program with two or more threads
       does it have impact on accumulation of drift?
 */

int main() {

  struct timeval now, start;
  long int d1, d2;
  int sum=0, i, j;

  gettimeofday(&start, NULL);
  printf("Start: time %ld   %d\n", start.tv_sec, start.tv_usec);

  for (i = 0; i<8; i++) {

    // do some computation; e.g. compute the sum of first K numbers
    sum=0;
    for (j=0; j<10000; j++) sum += j;

    // measure time
    gettimeofday(&now, NULL);

    printf("Iter %d\tresult %d\n", i, sum);
    //  Simple output of time:
    //    printf("    current time\t%ld\t%ld\n",  now.tv_sec, now.tv_usec);

    // compute differerence
    d1 = now.tv_sec - start.tv_sec;
    if (now.tv_usec > start.tv_usec) {
      d2 = now.tv_usec - start.tv_usec;
    } else {
      d2 = 1000000 + now.tv_usec - start.tv_usec;
      d1++;
    }
    printf("    difference\t %ld\t%ld\n",  d1, d2);

    sleep(1);
  }

  return 0;
}
