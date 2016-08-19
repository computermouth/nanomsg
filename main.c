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

	char srv_msg[] = "hello";
	int sz_msg = strlen(srv_msg) + 1;

	int i;

    for (;;) {
	int sbytes = nn_send (fd, srv_msg, sz_msg, 0);
	assert (sbytes == sz_msg);


		char *buf = NULL;
		int bytes = nn_recv (fd, &buf, NN_MSG, 0);
		if (bytes == ETIMEDOUT) break;
		if (bytes >= 0)
		{
			printf ("SERVER: RECEIVED \"%s\" SURVEY RESPONSE\n", buf);
			nn_freemsg (buf);
		}
        //~ uint8_t msg[2 * sizeof (uint32_t)];
        //~ uint32_t secs, subs;
        //~ int rc;

        //~ secs = (uint32_t) time (NULL);
        //~ subs = (uint32_t) nn_get_statistic (fd, NN_STAT_CURRENT_CONNECTIONS);

        //~ secs = htonl (secs);
        //~ subs = htonl (subs);

        //~ memcpy (msg, &secs, sizeof (secs));
        //~ memcpy (msg + sizeof (secs), &subs, sizeof (subs));

        //~ rc = nn_send (fd, msg, sizeof (msg), 0);
        //~ if (rc < 0) {
            //~ /*  There are several legitimate reasons this can fail.
                //~ We note them for debugging purposes, but then ignore
                //~ otherwise. */
            //~ fprintf (stderr, "nn_send: %s (ignoring)\n",
                //~ nn_strerror (nn_errno ()));
        //~ }
    }

    /* NOTREACHED */
    nn_close (fd);
    return (-1);
}

int main(){

	int rc;
	rc = server("ipc:///tmp/survey.ipc");

	exit (rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
