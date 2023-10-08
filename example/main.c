#include <stdio.h>
#include "add.h"
#include "sub.h"

int main(void)
{
	printf("1 + 2 = %d\n", add(1, 2));
	printf("4 - 2 = %d\n", sub(4, 2));
	return 0;
}

