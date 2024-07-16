#pragma once

// Struct/function definitions copied from headers in bionic/libc/dns

#include <resolv.h>
#include <net/if.h>

/* Hard-coded defines */
#define MAXNS			4	/* max # name servers we'll track */
#define MAXDNSRCH		6	/* max # domains in search path */
#define MAXDNSRCHPATH		256	/* max length of domain search paths */
#define MAXNSSAMPLES		64	/* max # samples to store per server */
#define	MAXRESOLVSORT		10	/* number of net to sort on */
#define	MAXALIASES	35
#define	MAXADDRS	35

// These are function pointers
typedef void* res_send_rhook;
typedef void* res_send_qhook;

struct res_static {
  char* h_addr_ptrs[MAXADDRS + 1];
  char* host_aliases[MAXALIASES];
  char hostbuf[8 * 1024];
  u_int32_t host_addr[16 / sizeof(u_int32_t)]; /* IPv4 or IPv6 */
  FILE* hostf;
  int stayopen;
  const char* servent_ptr;
  struct servent servent;
  struct hostent host;
};

union res_sockaddr_union {
	struct sockaddr_in	sin;
#ifdef IN6ADDR_ANY_INIT
	struct sockaddr_in6	sin6;
#endif
#ifdef ISC_ALIGN64
	int64_t			__align64;	/* 64bit alignment */
#else
	int32_t			__align32;	/* 32bit alignment */
#endif
	char			__space[128];   /* max size */
};

struct __res_state_ext {
	union res_sockaddr_union nsaddrs[MAXNS];
	struct sort_list {
		int     af;
		union {
			struct in_addr  ina;
			struct in6_addr in6a;
		} addr, mask;
	} sort_list[MAXRESOLVSORT];
	char nsuffix[64];
	char nsuffix2[64];
};

struct __res_state {
	unsigned	netid;			/* NetId: cache key and socket mark */
	int	retrans;	 	/* retransmission time interval */
	int	retry;			/* number of times to retransmit */
#ifdef sun
	u_int	options;		/* option flags - see below. */
#else
	u_long	options;		/* option flags - see below. */
#endif
	int	nscount;		/* number of name servers */
	struct sockaddr_in
		nsaddr_list[MAXNS];	/* address of name server */
#define	nsaddr	nsaddr_list[0]		/* for backward compatibility */
	u_short	id;			/* current message id */
	char	*dnsrch[MAXDNSRCH+1];	/* components of domain to search */
	char	defdname[256];		/* default domain (deprecated) */
#ifdef sun
	u_int	pfcode;			/* RES_PRF_ flags - see below. */
#else
	u_long	pfcode;			/* RES_PRF_ flags - see below. */
#endif
	unsigned ndots:4;		/* threshold for initial abs. query */
	unsigned nsort:4;		/* number of elements in sort_list[] */
	char	unused[3];
	struct {
		struct in_addr	addr;
		uint32_t	mask;
	} sort_list[MAXRESOLVSORT];
#ifdef __OLD_RES_STATE
	char lookups[4];
#else
	res_send_qhook qhook;		/* query hook */
	res_send_rhook rhook;		/* response hook */
	int	res_h_errno;		/* last one set for this context */
	unsigned _mark;			/* If non-0 SET_MARK to _mark on all request sockets */
	int	_vcsock;		/* PRIVATE: for res_send VC i/o */
	u_int	_flags;			/* PRIVATE: see below */
	u_int	_pad;			/* make _u 64 bit aligned */
	union {
		/* On an 32-bit arch this means 512b total. */
		char	pad[72 - 4*sizeof (int) - 2*sizeof (void *)];
		struct {
			uint16_t		nscount;
			uint16_t		nstimes[MAXNS];	/* ms. */
			int			nssocks[MAXNS];
			struct __res_state_ext *ext;	/* extention for IPv6 */
		} _ext;
	} _u;
#endif
        struct res_static   rstatic[1];
};

extern struct __res_state *__res_get_state(void);
#define _res (*__res_get_state())
