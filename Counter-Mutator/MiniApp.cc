#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

int foo() {
	return 0;
}

int main(){
     
	char *str, *str2;
	int *intArray, test, test2;
	
	str = (char *) malloc(12);
	str2 = (char *) malloc(100);
	intArray = (int *) malloc(10); 
	strcpy(str, "Hello world");
	printf("String = %s,  Address = %u\n", str, str);

	for (int i; i < 5; i++) {
		foo();
	}

	return 0;
}

