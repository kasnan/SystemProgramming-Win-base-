﻿/* simple.cpp
demonstrating assembly language code generated by the compiler */
#include <windows.h>
int sum(int x, int y);
int WINAPI wsum(int x, int y); 

void main() {
	int a, b, c; 
	a = 10;
	b = 20;
	c = a + b; 
	c = sum(a, b);
	c = wsum(a, b);
}
int sum(int x, int y) {
	int z;
	z = x + y; 
	return z;
}
int WINAPI wsum(int x, int y) { //모든 Windows API는 WINAPI 형식의 함수이다. int z;
	int z = x + y; 
	return z;
}