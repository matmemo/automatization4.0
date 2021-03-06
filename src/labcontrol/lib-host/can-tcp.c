#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "can-tcp.h"
#include "debug.h"
#include "tcp_server.h"

int listen_socket;
cann_conn_t *cann_conns_head = NULL;


#ifndef max
 #define max(a,b) (((a) > (b)) ? (a) : (b))
#endif


//Convert a struct sockaddr address to a string, IPv4 and IPv6:

static char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
	switch (sa->sa_family) {
		case AF_INET:
			inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
			          s, maxlen);
			break;

		case AF_INET6:
			inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
			          s, maxlen);
			break;

		default:
			strncpy(s, "Unknown AF", maxlen);
			return NULL;
	}

	return s;
}

/*****************************************************************************
 * Connection management
 */

/* open listening socket and initialize */
void cann_listen(char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	char buf[200];
	int ret, one = 1;

	signal(SIGPIPE, SIG_IGN);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;  // Allow IPv4 or IPv6 - ipv4 is mapped into an v6 addr -> ai_v4mapped
	hints.ai_socktype = SOCK_STREAM;		// Datagram socket
	hints.ai_flags = AI_PASSIVE | AI_V4MAPPED | AI_NUMERICSERV | AI_ALL;     // For wildcard IP address

	s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "No listening addresses available (getaddrinfo: %s)\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	//dump address info
	for (rp = result; rp != NULL; rp = rp->ai_next)
		debug(5, "Found Address %s\t\t(family: %s, socktype %i: protocol %i)",
			get_ip_str((struct sockaddr *)rp->ai_addr,
			buf, sizeof(buf)),
			(rp->ai_family == AF_INET)?"IPv4":((rp->ai_family == AF_INET6)?"IPv6":((snprintf(buf, sizeof(buf), "%i", rp->ai_family) != 0)?buf:"")),
			rp->ai_socktype,
			rp->ai_protocol);

	/* getaddrinfo() returns a list of address structures.
	   Try each address until we successfully bind(2).
	   If socket(2) (or bind(2)) fails, we (close the socket
	   and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		debug(5, "Trying to bind to %s (%s)",
			get_ip_str((struct sockaddr *)rp->ai_addr,
			buf, sizeof(buf)),
			(rp->ai_family == AF_INET)?"IPv4":((rp->ai_family == AF_INET6)?"IPv6":((snprintf(buf, sizeof(buf), "%i", rp->ai_family) != 0)?buf:"")),
			rp->ai_socktype,
			rp->ai_protocol);
		
		//set reuseaddr	to avoid address in use (b/c if close_wait) when restarting
		ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

		//make this a dual-stack socket in windows
#if defined(_WIN32)
		ret = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
#endif

		if (ret != 0) debug_perror(0, "Could not set socket options: ");

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;                  /* Success */

		debug_perror(0, "Could not bind to %s.", get_ip_str((struct sockaddr *)rp->ai_addr, buf, sizeof(buf)));
		close(sfd);
	}

	if (rp == NULL) {               /* No address succeeded */
		debug_perror(0, "All addresses in use");
		exit(EXIT_FAILURE);
	} else debug(5, "Bind succeeded!");

	freeaddrinfo(result);           /* No longer needed */
	listen_socket = sfd;

	int flags = 0;
	flags = fcntl(listen_socket, F_GETFL, 0);
	fcntl(listen_socket, F_SETFL, flags | O_NDELAY);

	/* specify queue */
	ret = listen(listen_socket, SOMAXCONN);
	debug_assert(ret >= 0, "Could listen() listening socket");
}

/* open connection to cand */
cann_conn_t *cann_connect(char *server, char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;
	cann_conn_t *client;
	char buf[INET6_ADDRSTRLEN];

	// initialize client struct
	client = malloc(sizeof(cann_conn_t));
	if (client == NULL)
	{
		debug(0, "Could not allocate client buffer!\n");
		free(client);
		exit(EXIT_FAILURE);
	}
	client->state = CANN_LEN;
	client->missing_bytes = 0;
	client->error = 0;

	#ifdef USE_WINSOCK
		WSADATA wsaData;
		int err;

		err = WSAStartup(0x0202, &wsaData);
		if (err != 0) {
			/* Tell the user that we could not find a usable */
			/* Winsock DLL.                                  */
			printf("WSAStartup failed with error: %d\n", err);
			exit(EXIT_FAILURE);
		}
	#endif
	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV; // 
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo(server, port, &hints, &result);
	if (s != 0) {
		debug_perror(0, "getaddrinfo: %s\n", gai_strerror(s));
		free(client);
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	Try each address until we successfully connect(2).
	If socket(2) (or connect(2)) fails, we (close the socket
	and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		client->fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (client->fd == -1)
			continue;

		if (connect(client->fd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(client->fd);
	}
	if (rp == NULL) {               /* No address succeeded */
		debug_perror(0, "Could not connect\n");
		free(client);
		exit(EXIT_FAILURE);
	}
	debug(4, "Connected to %s.", get_ip_str((struct sockaddr *)rp->ai_addr, buf, sizeof(buf)));


	freeaddrinfo(result);           /* No longer needed */
	listen_socket = client->fd;
	#ifdef USE_WINSOCK
		u_long option = 1;
		ioctlsocket(client->fd, FIONBIO, &option);
	#else
		// set some options on socket
		fcntl(client->fd, F_SETFL, O_NONBLOCK);
	#endif

	int flag = 1;
	setsockopt(client->fd,
		   IPPROTO_TCP,     /* set option at TCP level */
		   TCP_NODELAY,     /* name of option */
		   (char *) &flag,  /* the cast is historical cruft */
		   sizeof(int));    /* length of option value */

	debug(9, "connected to server, fd=%d\r\n", client->fd);

	return client;
}

/* set bits in fd_set */
int cann_fdset(fd_set *set)
{
	cann_conn_t *client = cann_conns_head;
	int maxfd = listen_socket;

	// socket for new connections
	FD_SET(listen_socket, set);

	while (client) {
		if (!(client->error)) {
			FD_SET(client->fd, set);
			maxfd = max(maxfd, client->fd);
		}
		client = client->next;
	}

	return maxfd;
}

cann_conn_t *cann_accept(fd_set *set)
{
	cann_conn_t *client;

	// activity on listen_socket?
	if ( !FD_ISSET(listen_socket, set) )
		return NULL;

	FD_CLR(listen_socket, set);

	// accept connection
	int fd;
	struct sockaddr_storage remote;
	socklen_t len = sizeof(remote);

	fd = accept(listen_socket, (struct sockaddr*)&remote, &len);

	// set some options on socket
	fcntl(fd, F_SETFL, O_NONBLOCK);

	debug(3, "New Client\n");
	char buf1[200];
	if (getnameinfo ((struct sockaddr *) &remote, len,
		buf1, sizeof (buf1), NULL, 0, 0) != 0)
	strcpy (buf1, "???");
	char buf2[100];
	(void) getnameinfo ((struct sockaddr *) &remote, len,
		buf2, sizeof (buf2), NULL, 0, NI_NUMERICHOST);
	debug(3,"connection from %s (%s)\n", buf1, buf2);

	int flag = 1;
	setsockopt(fd,
		   IPPROTO_TCP,     /* set option at TCP level */
		   TCP_NODELAY,     /* name of option */
		   (char *) &flag,  /* the cast is historical cruft */
		   sizeof(int));    /* length of option value */

	// initialize client struct
	client = (cann_conn_t *)malloc(sizeof(cann_conn_t));
	if (client == NULL)
	{
		debug(0, "Could not allocate client buffer!\n");
		exit(EXIT_FAILURE);
	}
	client->next  = cann_conns_head;
	client->fd    = fd;
	client->missing_bytes = 0;
	client->state = CANN_LEN;
	client->error = 0;
	cann_conns_head = client;

	return client;
}

void cann_close_errors()
{
	cann_conn_t **client = &cann_conns_head;

	while (*client) {
		if ( (*client)->error) {
			cann_conn_t *del = (*client);

			*client = del->next;
			//client = &(del->next);

			debug(2, "===> Closing fd %d", del->fd);
			close(del->fd);
			free(del);

		} else {
			client = &((*client)->next);
		}
	}
}

void cann_close(cann_conn_t *conn)
{
	// close all connections?
	if (!conn) {
		debug(1, "trying to close socket");
		conn = cann_conns_head;
		while (conn) {
			cann_conn_t *oldconn = conn;
			debug(1, "close socket");
			shutdown(conn->fd, SHUT_RDWR);
			close(conn->fd);
			conn = conn->next;
			free(oldconn);
		}
	}
}


cann_conn_t *cann_activity(fd_set *set)
{
	cann_conn_t *client = cann_conns_head;

	while (client) {
		if (FD_ISSET(client->fd, set) && !client->error) {
			FD_CLR(client->fd, set);
			return client;
		}
		client = client->next;
	}

	return NULL;
}


/*****************************************************************************
 * Memory Management
 */

rs232can_msg *cann_buffer_get()
{
	rs232can_msg *cmsg = malloc(sizeof(rs232can_msg));
	debug_assert(cmsg == NULL, "cann_buffer_get malloc fail!\n");
	return cmsg;
}

void cann_free(rs232can_msg *rmsg)
{
	free(rmsg);
}


void cann_dumpconn()
{
	cann_conn_t *client = cann_conns_head;

	while (client) {
		debug(9, "CANN connection: fd=%d error=%d", client->fd, client->error);
		client = client->next;
	}
}

void cann_conn_free(cann_conn_t* conn)
{
	if (conn->next != NULL)
		cann_conn_free(conn->next);

	conn->next = NULL;
	if (conn->rcv_ptr != NULL)
		free(conn->rcv_ptr);

	free(conn);
}


/*****************************************************************************
 * rcv
 */

/* nonblocking read on netwok -- returns msg if complete msg arrived */
rs232can_msg *cann_get_nb(cann_conn_t *client)
{
	int ret;
	unsigned char val;

	// sanity
	if (client->error) {
		debug( 0, "cann_get_nb() with error %d on %d", client->error, client->fd); // if client con to server breaks we get an endless high cpu loop "cann_get_nb() with error 1 on 3"
		return NULL;
	}

	// XXX das alles geht auch einfacher XXX
	if (client->state == CANN_LEN) {
		ret = read(client->fd, &val, 1);

		if (ret == 0) goto eof;
		if (ret < 0) goto error;

		// check msg length
		if (val > sizeof(client->msg.data)) {
			debug( 2, "Protocol error on fd %d: message too big (size=%d)",
					client->fd, val );
			client->error = 1;
			return NULL;
		}

		debug(10, "Next packet on %d: length=%d", client->fd, val);
		client->msg.len        = val;
		client->missing_bytes  = val;
		client->rcv_ptr        = client->msg.data;
		client->state          = CANN_CMD;
	}

	if (client->state == CANN_CMD) {
		ret = read(client->fd, &(client->msg.cmd), 1);

		if (ret == 0) goto eof;
		if (ret < 0) goto error;

		debug(10, "Next packet on %d: cmd=%d", client->fd, client->msg.cmd);
		client->state = CANN_PAYLOAD;
	}

	if (client->missing_bytes > 0) {
		// read data
		ret = read(client->fd, client->rcv_ptr, client->missing_bytes);

		if (ret == 0) goto eof;
		if (ret < 0) goto error;

		client->missing_bytes -= ret;
		client->rcv_ptr       += ret;

		debug(10, "fd %d: recived %d bytes, %d missing",
			  client->fd, ret, client->missing_bytes);
	}

	// message complete?
	if (client->missing_bytes == 0) {
		rs232can_msg *rmsg = malloc(sizeof(rs232can_msg));

		if (rmsg == NULL)
		{
			debug(0, "Could not allocate rmsg buffer, exiting!\n");
			exit(EXIT_FAILURE);
		}

		memcpy(rmsg, &(client->msg), sizeof(rs232can_msg));
		client->state = CANN_LEN;
		return rmsg;
	}

	return NULL;

error:
	if ((errno == EAGAIN) || (errno==0))
		return NULL;

eof:
	debug_perror(5, "Error readig fd %d (ret==%d)", client->fd, ret);
	client->error = 1;
	return NULL;
}

rs232can_msg *cann_get(cann_conn_t *client)
{
	int ret;
	fd_set rset;
	rs232can_msg *rmsg;

	for (;;) {
		FD_ZERO(&rset);
		FD_SET(client->fd, &rset);

		ret = select(client->fd + 1, &rset, (fd_set *) NULL, (fd_set *) NULL, NULL);
		debug_assert(ret >= 0, "cann_get: select failed");

		rmsg = cann_get_nb(client);
		if (rmsg)
			return rmsg;
	}
}


/*****************************************************************************
 * transmit
 */

/* transmit and free message */
void cann_transmit(cann_conn_t *conn, rs232can_msg *msg)
{
	// sanity
	if (conn->error) {
		debug(5, "cann_transmit: not transmiting on errorous connection fd=%d", conn->fd);
		return;
	}

	debug(9, "Transmitting message fd=%d", conn->fd);

	/* copy this into another buffer, convert to cantcp and send it with a single write to avoid multiple tcp packets */
	unsigned char txbuf[sizeof(rs232can_msg)], swap;
	memcpy((void *) txbuf, (void *) msg, sizeof(rs232can_msg));
	swap = txbuf[0];
	txbuf[0] = txbuf[1];
	txbuf[1] = swap;
	if (send(conn->fd, txbuf, 2 + msg->len, MSG_NOSIGNAL) != msg->len + 2)
		goto error;

	return;
error:
	conn->error = 1;
	debug_perror(5, "Error sending fd %d", conn->fd);
	return;

}

