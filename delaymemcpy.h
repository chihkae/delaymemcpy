#ifndef _DELAYMEMCPY_H_
#define _DELAYMEMCPY_H_

#include <string.h>

void initialize_delay_memcpy_data(void);
void *delay_memcpy(void *dst, void *src, size_t size);

#endif
