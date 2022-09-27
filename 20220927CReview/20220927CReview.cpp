#include<stdio.h>

int main() {
	int a = 7, b = 20, c = 30;
	a = b = c;//대입연산자
	while (a < 50) {
		printf("a = %d\n", a);
		a++;
	}
	return 0;
}