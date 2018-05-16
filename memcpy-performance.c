#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#include "delaymemcpy.h"

#define ARRAY_SIZE 0x40000000 // 2^30=1GB

#define NUM_EVENTS 1000

/* Arrays declared as global, as this allows program to use more
   memory. Aligned to page boundaries so other global variables don't
   end up in same page. */
char __attribute__ ((aligned (0x1000))) array[ARRAY_SIZE];
char __attribute__ ((aligned (0x1000))) copy[ARRAY_SIZE];
char __attribute__ ((aligned (0x1000))) copy2[ARRAY_SIZE];

void *random_array(void *array, size_t size) {

  int i;
  size /= sizeof(long);
  for (i = 0; i < size; i++)
    ((long *)array)[i] = random();

  return array;
}

void print_array(char *array) {
  int i;
  printf("%p: \n\t", array);
  for (i = 0; i < 10; i++)
    printf("%02x ", (unsigned char) ++array[i]);
  printf("\n");
}

void sequence_of_events(char *dst, char *src, size_t size) {

  int event;
  volatile char last_data = 0;

  // Uses value to avoid warning or compilation optimizations
  (char) last_data;

  for (event = 0; event < NUM_EVENTS; event++) {

    switch(random() % 4) {
    case 0:
      last_data = src[random() % size];
      break;
    case 1:
      last_data = dst[random() % size];
      break;
    case 2:
      src[random() % size] = (char) random();
      break;
    case 3:
      dst[random() % size] = (char) random();
      break;
    }
  }
}

int main(void) {

  struct timeval start, end, diff;

  srandom(time(NULL));

  initialize_delay_memcpy_data();

  printf("Initializing array (%d elements)...\n", ARRAY_SIZE);
  random_array(array, ARRAY_SIZE);

  print_array(array);

  printf("Copying array (normal)...\n");
  gettimeofday(&start, NULL);
  memcpy(copy, array, ARRAY_SIZE);
  sequence_of_events(copy, array, ARRAY_SIZE);
  gettimeofday(&end, NULL);

  timersub(&end, &start, &diff);
  printf("%ld.%06ld\n", diff.tv_sec, diff.tv_usec);

  printf("Copying array (delayed)...\n");
  gettimeofday(&start, NULL);
  delay_memcpy(copy2, array, ARRAY_SIZE);
  sequence_of_events(copy2, array, ARRAY_SIZE);
  gettimeofday(&end, NULL);

  timersub(&end, &start, &diff);
  printf("%ld.%06ld\n", diff.tv_sec, diff.tv_usec);

  return 0;
}
