#include<stdio.h>
void printN(int n);
int main() {
	int i = 0;
	while (i < 5) {
		printN(i);
		i++;
	}
}
void printN(int n) {
	int i;
	for (i = 0; i < 10; i++)
		printf("%d%d%d%d%d%d%d%d\n", n, n, n, n, n, n, n, n);
}