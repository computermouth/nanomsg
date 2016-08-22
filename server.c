#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>

#include <nanomsg/nn.h>
#include <nanomsg/survey.h>

int server(const char *url)
{
    int fd; 

    /*  Create the socket. */
    fd = nn_socket (AF_SP, NN_SURVEYOR);

    if (fd < 0) {
        fprintf (stderr, "nn_socket: %s\n", nn_strerror (nn_errno ()));
        return (-1);
    }

    /*  Bind to the URL.  This will bind to the address and listen
        synchronously; new clients will be accepted asynchronously
        without further action from the calling program. */

    if (nn_bind (fd, url) < 0) {
        fprintf (stderr, "nn_bind: %s\n", nn_strerror (nn_errno ()));
        nn_close (fd);
        return (-1);
    }

	char srv_msg[] = "0";
	int sz_msg = strlen(srv_msg) + 1;

    for (;;) {

	int sbytes = nn_send (fd, srv_msg, sz_msg, 0);
	assert (sbytes == sz_msg);
	printf("sent %s\n", srv_msg);


	usleep( 1000 * 100 );
/*		char *buf = NULL;
		int bytes = nn_recv (fd, &buf, NN_MSG, 0);
		if (bytes == ETIMEDOUT) break;
		if (bytes >= 0)
		{
			printf ("SERVER: RECEIVED \"%s\" SURVEY RESPONSE\n", buf);
			nn_freemsg (buf);
		}
*/

	srv_msg[0] = '0' + (random() % 2);
    }

    /* NOTREACHED */
    nn_close (fd);
    return (-1);
}

int main(){

	int rc;
	rc = server("tcp://0.0.0.0:5555");

	exit (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
