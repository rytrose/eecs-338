#include <stdio.h>

int main()
{
	int i;
	
	printf("Fibonacci Numbers:\n");	

	for(i = 0; i < 15; i++){
		printf("Index (");
		printf("%i",i);
		printf("): ");
		printf("%i",fib(i));
		printf("\n");	
	}

	fflush(stdout);

	return 0;
	
}

int fib(int n)
{
	if(n < 0){
		return 0;
	}	
	if(n == 0){
		return 1;
	}
	if(n == 1){
		return 1;
	}
	else{
		return fib(n - 1) + fib(n - 2);
	}
}

