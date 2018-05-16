#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "delaymemcpy.h"

#define MAX_ARRAY_SIZE 0x40000000 // 2^30=1GB

/* Arrays declared as global, as this allows program to use more
   memory. Aligned to page boundaries so other global variables don't
   end up in same page. */
unsigned char __attribute__ ((aligned (0x1000))) array[0x100000];
unsigned char __attribute__ ((aligned (0x1000))) copy[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) A[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) B[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) C[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) D[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) F[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) J[0x100000];
unsigned char __attribute__ ((aligned(0x1000))) M[0x100000];
/* Initializes the array with a random set of values. */
void *random_array(void *array, size_t size) {

  int i;
  size /= sizeof(long);
  for (i = 0; i < size; i++)
    ((long *)array)[i] = random();

  return array;
}

/* Prints all values of the array */
void print_array(unsigned char *array, int size) {
  int i;
  for (i = 0; i < size; i++)
    printf("%02x ", array[i]);
  printf("\n");
}

int main(void) {

  srandom(time(NULL));

  initialize_delay_memcpy_data();

  printf("\nCopying one page of data, trigger copy via write dst...\n");

  random_array(array, 0x1000);
  printf("before copy\n");
  printf("Before copy: \n");
  print_array(array, 20);
  delay_memcpy(copy, array, 0x1000);
  copy[0]++; // Triggers copy
  printf("After copy : ");
  print_array(array, 20);
  printf("Destination: ");
  print_array(copy, 20);

  printf("\nCopying one page of data, trigger copy via write src...\n");
  random_array(array, 0x1000);
  printf("Before copy: ");
  print_array(array, 20);

  delay_memcpy(copy, array, 0x1000);
  array[0]++; // Triggers copy
  printf("After copy : ");
  print_array(array, 20);
  printf("Destination: ");
  print_array(copy, 20);


  printf("\nCopying two pages of data...\n");
  random_array(array, 0x2000);
  printf("Before copy: ");
  print_array(array, 20);

  delay_memcpy(copy, array, 0x2000);
  printf("After copy : ");
  print_array(array, 20);
  printf("Destination: \n");
  print_array(copy, 20);  // Triggers copy of first page only
  printf("2nd page: After copy :\n");
  print_array(array + 0x1800, 20);
  printf("Destination: ");
  print_array(copy + 0x1800, 20);  // Triggers copy of second page

  printf("\nCopying unaligned page of data...\n");
  random_array(array, 0x2000);
  printf("Before copy: ");
  print_array(array + 0x400, 20);
  delay_memcpy(copy + 0x400, array + 0x400, 0x1000);
  printf("After copy : ");
  print_array(array + 0x400, 20);
  printf("Destination: ");
  print_array(copy + 0x400, 20);  // Triggers copy of first page only

  /* MORE TESTS COME HERE */

  printf("\nmultiple pages...\n");
  random_array(array, 0x3000);
  printf("Before copy: ");
  print_array(array+0x1200,40);
  delay_memcpy(copy+0x1200, array+0x1200, 0x1000);
  printf("After copy : ");
  print_array(array+0x1200,20);
  printf("destination:");
  print_array(copy+0x1200, 20);


  printf("\ncausal dependencies: A copies to B, B copies to C, writing to B\n");
  random_array(A,0x1000);
  printf("Before copy: ");
  print_array(A,40);
  delay_memcpy(B,A,0x1000);
  delay_memcpy(C,B,0x1000);
  B[0]++;
  B[0]++;
  printf("After write to B[0]++");
  printf("Array A");
  print_array(A,40);
  printf("Array C");
  print_array(C,40);
  printf("Array B");
  print_array(B,40);


  printf("\ncausal dependencies: A copies to B, B copies to C, reading from C\n");
  random_array(A,0x1000);
  printf("Before copy: ");
  print_array(A,40);
  delay_memcpy(B,A,0x1000);
  delay_memcpy(C,B,0x1000);
  printf("Array C");
  print_array(C,40);

  printf("\ncausal dependencies: D copies to M, F copies to J, J copies to A,A to array, read from A\n");
  random_array(F,0x1000);
  printf("Before copy: ");
  printf("Array F");
  print_array(F,40);
  delay_memcpy(J,F,0x1000);
  delay_memcpy(A,J,0x1000);
  delay_memcpy(array,A,0x1000);
  printf("After copy");
  printf("array:\n");
  print_array(array,40);
  printf("Array M\n");
  print_array(M,40);
  printf("Array D\n");
  print_array(D,40);


  printf("\ncausal dependencies: D copies to F, F copies to J, J copies to A, A copies to M, read from M\n");
  random_array(D,0x1000);
  printf("Before copy: \n");
  print_array(D,40);
  delay_memcpy(F,D,0x1000);
  delay_memcpy(J,F,0x1000);
  delay_memcpy(A,J,0x1000);
  delay_memcpy(M,A,0x1000);
  printf("After copy\n");
  print_array(D,40);
  printf("Array M\n");
  print_array(M,40);

  printf("\n output dependencies: A to B, C to B, write to B\n");
  random_array(A,0x1000);
  printf("Before copy Array A:\n");
  print_array(A,50);
  delay_memcpy(B,A,0x1000);
  printf("Before copy Array C:\n");
  random_array(C,0x2000);
  print_array(C,80);
  delay_memcpy(B,C,0x2000);
  B[0]++;
  B[1]++;
  B[20]++;
  B[130]++;
  printf("after write to B((B[0]++,B[1]++,B[20]++,B[130]++))\n");
  printf("Array B:\n");
  print_array(B,80);
  printf("Array c after\n");
  print_array(C,80);


  printf("\n output dependencies: A to B, C to B, read from B\n");
  random_array(A,0x1000);
  printf("Before copy Array A: \n");
  print_array(A,80);
  delay_memcpy(B,A,0x1000);
  random_array(C,0x2000);
  printf("Before copy Array C:\n");
  print_array(C,80);
  delay_memcpy(B,C,0x2000);
  printf("after reading from B\n");
  printf("Array A\n");
  print_array(A,80);
  printf("Array B:\n");
  print_array(B,80);
  printf("Array C\n");
  print_array(C,80);

  printf("\n same source different destinations: A to B, A to C, write to A\n");
  random_array(A, 0x1000);
  printf("Before copy Array A: \n");
  print_array(A,50);
  delay_memcpy(B,A,0x1000);
  delay_memcpy(C,A,0x1000);
  A[0]++;
  A[1]++;
  A[2]++;
  printf("Array A:\n");
  print_array(A,50);
  printf("After writing to A\n");
  printf("Array B:\n");
  print_array(B,50);
  printf("Array C:\n");
  print_array(C,50);

  printf("\nNormal segfault...\n");
  int* p = NULL;
  printf("Null pointer,%d",*p);
  /*
  random_array(array,0x2000);
  delay_memcpy(copy,array,0x2000);
  print_array(copy+0x2005,10);//Trigers segfault, out of boundaries
*/

  return 0;
}
