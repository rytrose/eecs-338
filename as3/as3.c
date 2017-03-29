#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

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
if(pipe(p2c1) == -1){
	perror("Pipe not created: ");
	return(1);
}
	
if(pipe(c12p) == -1){
	perror("Pipe not created: ");
	return(1);
}

if(pipe(p2c2) == -1){
	perror("Pipe not created: ");
	return(1);
}

if(pipe(c22p) == -1){
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

	// c1 buffer
	char c1buf[10];
	
	// c1 message
	char c1message[2];
	int i;

	printf("Computing games...\n");
	fflush(stdout);

	for(i = 0; i < ITERATIONS; i++){
		sprintf(c1message, "%i", (rand_lim(776) % 2) );
		if((write(c12p[WRITE], c1message, 2)) == -1){
			perror("Write failed: ");
			return(1);
		}
		struct timespec tim, tim2;
		tim.tv_sec = 0;
		tim.tv_nsec = 20000000L;
		nanosleep(&tim, &tim2);
	}
	
	close(c12p[WRITE]);
	
	if((read(p2c1[READ], c1buf, 10)) > 0){
		if(*c1buf == 'k'){
			close(p2c1[READ]);
			return(0);
		}
	}
	
	return(1);
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

	// c2 buffer
	char c2buf[10];
	
	while((read(p2c2[READ], c2buf, 10)) > 0) {
	
		if(*c2buf == 'k'){
			close(p2c2[READ]);
			return(0);
		}
		else{
			
			// c2 message
			char c2message[2];
			int l;
	
			for(l = 0; l < ITERATIONS; l++){
				sprintf(c2message, "%i", (rand_lim(3425) % 2));
				if((write(c22p[WRITE], c2message, 2)) == -1){
					perror("Write failed: ");
					return(1);
				}
				struct timespec tim, tim2;
				tim.tv_sec = 0;
				tim.tv_nsec = 20000000L;
				nanosleep(&tim, &tim2);
			}
		
			close(c22p[WRITE]);
		}
	}

	return(1);
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
	if(c1index > (ITERATIONS - 1))
		break;
}

close(c12p[READ]);

// Tell C2 to start writing
char p2c2message[2];
sprintf(p2c2message, "%i", rand_lim(1));
write(p2c2[WRITE], p2c2message, 2);

char c2responses[ITERATIONS];
int c2index = 0;

// Read the c2 responses
while((read(c22p[READ], buf2, 1024)) > 0){
	c2responses[c2index] = *buf2;
	c2index++;
	if(c2index > (ITERATIONS - 1))
		break;
}

close(c22p[READ]);



int j;
double c1time = 0.0;
double c2time = 0.0;

for(j = 0; j < ITERATIONS; j++){
	printf("Game %i:\n", j+1);
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

printf("----------\n");
printf("Score:\n");
printf("%i: %.1f years\n", c1_pid, c1time);
printf("%i: %.1f years\n", c2_pid, c2time);

// Tell the children to kill themselves
char p2c1kill[1] = {'k'};
if((write(p2c1[WRITE], p2c1kill, 1) == -1)){
	perror("Write failed: ");
	return(1);
}


char p2c2kill[1] = {'k'};
if((write(p2c2[WRITE], p2c2kill, 1 == -1))){
	perror("Write failed: ");
	return(1);
}

// Close pipes
close(p2c1[WRITE]);
close(p2c2[WRITE]);

sleep(1);

return (0);
}

// Returns a random integer between 0 and n
int rand_lim(int n)
{
int divisor = RAND_MAX/(n+1);
int retval;
time_t t;


do{
retval = rand() / divisor;
} while (retval > n);

return retval;
}

