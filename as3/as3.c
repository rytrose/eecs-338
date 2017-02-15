#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define ITERATIONS 10


char c1responses[ITERATIONS];
char c2responses[ITERATIONS];

int  main()
{
// pid variables
pid_t c1_pid;
pid_t c2_pid;

// Read buffers
char buf[1024];
char buf2[1024];

// Declare 4 pipes
int p2c1[2], c12p[2], p2c2[2], c22p[2];

// Initialize pipes
if(pipe(p2c1)){
	perror("Pipe not created: ");
	return(1);
}
	
if(pipe(c12p)){
	perror("Pipe not created: ");
	return(1);
}

if(pipe(p2c2)){
	perror("Pipe not created: ");
	return(1);
}

if(pipe(c22p)){
	perror("Pipe not created: ");
	return(1);
}

// Child 1 (c1)
c1_pid = fork();

// c1 block
if(c1_pid == 0)
{

	// Close p2c1 WRITE
	close(p2c1[WRITE]);

	// Close c12p READ
	close(c12p[READ]);

	// c1 message
	char c1message[2];
	int i;

	printf("Computing games...\n");
	fflush(stdout);

	for(i = 0; i < ITERATIONS; i++){
		sprintf(c1message, "%i", rand_lim(1));
		write(c12p[WRITE], c1message, 2);
		struct timespec tim, tim2;
		tim.tv_sec = 0;
		tim.tv_nsec = 80000000L;
		nanosleep(&tim, &tim2);
	}

	close(c12p[WRITE]);
	return(0);
}

// Child 2 (c1)
c2_pid = fork();

// c2 block
if(c2_pid == 0)
{

	// Close p2c2 WRITE
	close(p2c2[WRITE]);

	// Close c22p READ
	close(c22p[READ]);

	// c2 message
	char c2message[2];
	int i;

	for(i = 0; i < ITERATIONS; i++){
		sprintf(c2message, "%i", rand_lim(1));
		write(c22p[WRITE], c2message, 2);
		struct timespec tim, tim2;
		tim.tv_sec = 0;
		tim.tv_nsec = 80000000L;
		nanosleep(&tim, &tim2);
	}

	close(c22p[WRITE]);
	return(0);
}


// Close p2c1 READ
close(p2c1[READ]);
// Close c12p WRITE
close(c12p[WRITE]);
// Close p2c2 READ
close(p2c2[READ]);
// Close c22p WRITE
close(c22p[WRITE]);

char c1responses[ITERATIONS];
int c1index = 0;

// Read the c1 responses
while((read(c12p[READ], buf, 1024)) > 0){
	c1responses[c1index] = *buf;
	c1index++;
}

char c2responses[ITERATIONS];
int c2index = 0;

// Read the c2 responses
while((read(c22p[READ], buf2, 1024)) > 0){
	c2responses[c2index] = *buf2;
	c2index++;
}

int j;
double c1time = 0.0;
double c2time = 0.0;

int k1;
for(k1 = 0; k1 < ITERATIONS; k1++){
	printf("C1: %c\n", c1responses[k1]);
	fflush(stdout);
}

int k;
for(k = 0; k < ITERATIONS; k++){
	printf("C2: %c\n", c2responses[k]);
	fflush(stdout);
}

for(j = 0; j < ITERATIONS; j++){
	printf("Game %i:\n", j);
	fflush(stdout);
	
	// Both children defect
	if(c1responses[j] == '1' && c2responses[j] == '1'){
		printf("%i: Defect\n%i: Defect\n", c1_pid, c2_pid);
		fflush(stdout);

		c1time += 6;
		c2time += 6; 
	}
	// c1 cooperates and c2 defects
	else if(c1responses[j] == '0' && c2responses[j] == '1'){
		printf("%i: Cooperate\n%i: Defect\n", c1_pid, c2_pid);
		fflush(stdout);

		c1time += 10;
		c2time += 0;
	}
	// c1 defects and c2 cooperates
	else if(c1responses[j] == '1' && c2responses[j] == '0'){
		printf("%i: Defect\n%i: Cooperate\n", c1_pid, c2_pid);
		fflush(stdout);

		c1time += 0;
		c2time += 10;
	}
	// Both children cooperate
	else{
		printf("%i: Cooperate\n%i: Cooperate\n", c1_pid, c2_pid);
		fflush(stdout);

		c1time += 0.5;
		c2time += 0.5;
	}
}


return (0);
}

// Returns a random integer between 0 and n
int rand_lim(int n)
{
int divisor = RAND_MAX/(n+1);
int retval;

do{
retval = rand() / divisor;
} while (retval > n);

return retval;
}

