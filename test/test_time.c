#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


int main() {

	struct timeval start;
	struct timeval end;
	double trans_time; //calulate the time between the device is opened and it is closed

	gettimeofday(&start, NULL);

	for (int i = 0; i < 50000; ++i) {
		for (int j = 0; j < 50000; ++j) {
			;
		}
		printf("%d\n", i);
	}

	gettimeofday(&end, NULL);

	trans_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec);
	printf("Transmission time: %lf ms\n", trans_time);

}
