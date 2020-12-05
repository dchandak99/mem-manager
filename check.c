#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "alloc.h"

char *f()
{
	char b = 2;
	char *a = &b;

	char *c = a;
	c = c + 2;
	printf("%p\n", a);
	return a+2;
}

int main()
{	

	printf("%p\n", f());
}