#include "stdio.h"

int main() {
	printf("Hello World!\n");
	int arr[10];
	for (int i = 0; i < 10; i ++) {
		arr[i] = i;
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
