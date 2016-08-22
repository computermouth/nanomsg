#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include <nanomsg/nn.h>
#include <nanomsg/survey.h>

#include <errno.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int i2c_dev_open( int address ) {
    int file;
    if ( (file = open("/dev/i2c-0", O_RDWR)) < 0 ) {
        printf("Failed to open the bus.");
        printf("Error: %s\n", strerror( errno ) );
        exit(1);
    }

    if ( ioctl(file, I2C_SLAVE_FORCE, address) < 0 ) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        printf("Error: %s\n", strerror( errno ) );
        exit(1);
    }
    return file;
}

int i2c_write(int file, char reg, char byte) {
    if( i2c_smbus_write_byte_data(file, reg, byte) < 0 )
        printf("Error: %s\n", strerror( errno ) );
    return 0;
}

int client(const char *url)
{

    const char AXP209 = 0x34;
    int file = i2c_dev_open( AXP209 );

    int fd, i; 

    /*  Create the socket. */
    fd = nn_socket (AF_SP, NN_RESPONDENT);

    if (fd < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        return (-1);
    }

    /*  Bind to the URL.  This will bind to the address and listen
        synchronously; new clients will be accepted asynchronously
        without further action from the calling program. */

    if (nn_connect (fd, url) < 0) {
        fprintf (stderr, "nn_bind: %s\n", nn_strerror (nn_errno ()));
        nn_close (fd);
        return (-1);
    }

    for (;;) {
		char *buf = NULL;
		int bytes = nn_recv (fd, &buf, NN_MSG, 0);
		if (bytes == ETIMEDOUT) break;

		if (strcmp(buf, "1")){
			i2c_write(file, 0x93, 7);
		}else{
			i2c_write(file, 0x93, 0);
		}

		if (bytes >= 0)
		{
			nn_freemsg (buf);
//			char *d = "ohai";
//			int sz_d = strlen(d) + 1;
//			printf("sending response\n");
//			int bytes = nn_send (fd, d, sz_d, 0);
//			assert (bytes == sz_d);
		}

    }

    /* NOTREACHED */
    nn_close (fd);
    return (-1);
}

int main(){

	int rc;
	rc = client("tcp://pink.local:5555");

	exit (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
