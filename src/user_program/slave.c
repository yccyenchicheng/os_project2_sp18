#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define PAGE_SIZE 4096
#define BUF_SIZE 512
#define MAP_SIZE PAGE_SIZE * 100

// note that in slave_device, we define
// slave_IOCTL_CREATESOCK 0x12345677
// slave_IOCTL_MMAP 0x12345678
// slave_IOCTL_EXIT 0x12345679

int main (int argc, char* argv[]) {
	if (argc != 4) {
		printf("Usage: sudo ./slave received_file <fcntl or mmap> master_ip\n");
		exit(0);
	}

	// get received_file path, method and master_ip from command line
	char received_file[50];
	char method[20];
	char master_ip[20];

	strcpy(received_file, argv[1]);
	strcpy(method, argv[2]);
	strcpy(master_ip, argv[3]);

	int slave_fd, file_fd; // the file_descriptor for slave_device and the output file
	if( (slave_fd = open("/dev/slave_device", O_RDWR)) < 0) { //should be O_RDWR for PROT_WRITE when mmap()
		perror("error: cannot open the slave_device.\n");
		return 1;
	}

	// for transmission time
	struct timeval start; 
	struct timeval end;
	double transmission_time; //calulate the time between the device is opened and it is closed

	// record the start of the transmission time
	gettimeofday(&start ,NULL);

	// open the output file to write
	if( (file_fd = open (received_file, O_RDWR | O_CREAT | O_TRUNC)) < 0) {
		perror("error: cannot open the ouput file to write\n");
		return 1;
	}

	if(ioctl(slave_fd, 0x12345677, master_ip) == -1)	{ //0x12345677 : connect to master in the device
		perror("error: ioclt cannot create the slave socket.\n");
		return 1;
	}


	// for fcntl
	char fcntl_buffer[BUF_SIZE];         
	size_t ret, file_sz = 0, offset = 0;

	// for mmap
	char *kernel_address, *file_address;

	switch(method[0]) {
		// for fcntl
		case 'f': 
			do {
				ret = read(slave_fd, fcntl_buffer, sizeof(fcntl_buffer)); // read from the the device
				write(file_fd, fcntl_buffer, ret); //write to the input file
				file_sz += ret;
			} while(ret > 0);
			break;

		// for mmap
		case 'm':
			while (1) {
				ret = ioctl(slave_fd, 0x12345678);

				if (ret == 0) {
					file_sz = offset;
					break;
				}

				posix_fallocate(file_fd, offset, ret);
				file_address = mmap(NULL, ret, PROT_WRITE, MAP_SHARED, file_fd, offset); // map data to file_address
				kernel_address = mmap(NULL, ret, PROT_READ, MAP_SHARED, slave_fd, offset); // map data to kernel_address
				memcpy(file_address, kernel_address, ret);                               // copy the memory between them
				//printf("file: %x\n", *file_address);
				//printf("kernel: %x\n", *kernel_address);
				//printf("file: %X\n", *file_address);
				//printf("kernel: %X\n", *kernel_address);
				//printf("file: %p\n", file_address);
				//printf("kernel: %p\n", kernel_address);
				offset += ret;
			}
			break;

	}

	ioctl(slave_fd, 7122); // display the memory mapped region of slave_device

	if (ioctl(slave_fd, 0x12345679) == -1) { // we have done receiving data, so we should close the connection
		perror("error: ioctl client cannot exit\n");
		return 1;
	}

	close(slave_fd);
	// transmission time ends when we closed the slave_device
	gettimeofday(&end, NULL);
	transmission_time = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)*0.001;
	printf("Transmission time: %lf ms, File size: %zd bytes\n", transmission_time, file_sz);

	close(file_fd);
	return 0;
}

