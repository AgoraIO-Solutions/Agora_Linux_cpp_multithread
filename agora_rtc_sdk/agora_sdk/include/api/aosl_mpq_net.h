/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 22nd, 2018
 * Module:	Socket helper utils header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_NET_H__
#define __AOSL_MPQ_NET_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_mpq_fd.h>
#include <api/aosl_socket.h>
#include <api/aosl_data.h>

#ifdef _MSC_VER
/* C4200: nonstandard extension used: zero sized array */
#pragma warning (disable: 4200)
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if !defined (__linux__) && !defined (__MACH__) && !defined (__kliteos__)
typedef uint16_t sa_family_t;
#endif

#define AOSL_IPHDR_LEN 20

#define AOSL_UDPHDR_LEN 8
#define AOSL_TCPHDR_LEN 20

#define AOSL_IP_UDP_HDR_LEN (AOSL_IPHDR_LEN + AOSL_UDPHDR_LEN)
#define AOSL_IP_TCP_HDR_LEN (AOSL_IPHDR_LEN + AOSL_TCPHDR_LEN)


typedef union {
	struct sockaddr sa;
	struct sockaddr_in in;
	aosl_sockaddr_in6_t in6;
#if defined (__linux__) || defined (__MACH__) || defined (__kliteos2__)
	struct sockaddr_un un;
#endif
	aosl_sockaddr_storage_t __storage;
} aosl_sk_addr_t;

typedef struct {
	aosl_sk_addr_t addr;
	socklen_t addrlen;
} aosl_sk_al_t;

typedef struct {
	aosl_fd_t newsk;
	aosl_sk_al_t al;
} aosl_accept_data_t;

/* socket flags addr info */
typedef struct {
	int flags;
	aosl_sk_al_t al;
} aosl_sk_fal_t;

typedef struct {
	size_t len;
	uintptr_t buf [0];
} aosl_sk_ctrl_t;

/**
 * The protocol here is as following:
 * 1. the extra size has uintptr_t type;
 * 2. the extra size must be the first member of the type;
 * 3. the extra size must be set to the size of total struct;
 **/
typedef struct {
	uintptr_t extra_size;
	int flags;
} aosl_sk_ext_f_t;

typedef struct {
	uintptr_t extra_size;
	aosl_sk_fal_t fal;
} aosl_sk_ext_fal_t;

typedef struct {
	uintptr_t extra_size;
	aosl_sk_fal_t fal;
	aosl_sk_ctrl_t ctrl;
} aosl_sk_ext_falc_t;

/**
 * The listen state socket readable callback function type
 * Parameters:
 *    data: the data buffer holding the packet
 *     len: the data length in bytes in the buffer
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 *    addr: the socket address data received from
 * Return value:
 *    None.
 **/
typedef void (*aosl_sk_accepted_t) (aosl_accept_data_t *accept_data, size_t len, uintptr_t argc, uintptr_t argv []);

/**
 * The dgram socket received data callback function type
 * Parameters:
 *    data: the data buffer holding the packet
 *     len: the data length in bytes in the buffer
 *    argc: the args count when adding the fd
 *    argv: the args vector when adding the fd
 *    addr: the socket address data received from
 * Return value:
 *    None.
 **/
typedef void (*aosl_dgram_sk_data_t) (void *data, size_t len, uintptr_t argc, uintptr_t argv [], const aosl_sk_addr_t *addr);

extern __aosl_api__ aosl_fd_t aosl_socket (int domain, int type, int protocol);
extern __aosl_api__ int aosl_bind (aosl_fd_t sockfd, const struct sockaddr *addr, socklen_t addrlen);
extern __aosl_api__ int aosl_getsockname (aosl_fd_t sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern __aosl_api__ int aosl_getpeername (aosl_fd_t sockfd, struct sockaddr *addr, socklen_t *addrlen);
extern __aosl_api__ int aosl_getsockopt (aosl_fd_t sockfd, int level, int optname, void *optval, socklen_t *optlen);
extern __aosl_api__ int aosl_setsockopt (aosl_fd_t sockfd, int level, int optname, const void *optval, socklen_t optlen);


extern __aosl_api__ int aosl_mpq_add_dgram_socket (aosl_mpq_t qid, aosl_fd_t fd, size_t max_pkt_size,
							aosl_dgram_sk_data_t data_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_add_named_dgram_socket (aosl_mpq_t qid, aosl_fd_t fd, const char *name, size_t max_pkt_size,
							aosl_dgram_sk_data_t data_f, aosl_fd_event_t event_f, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_add_stream_socket (aosl_mpq_t qid, aosl_fd_t fd, int enable, size_t max_pkt_size,
			aosl_check_packet_t chk_pkt_f, aosl_fd_data_t data_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_add_named_stream_socket (aosl_mpq_t qid, aosl_fd_t fd, const char *name, int enable, size_t max_pkt_size,
				aosl_check_packet_t chk_pkt_f, aosl_fd_data_t data_f, aosl_fd_event_t event_f, aosl_obj_dtor_t dtor, uintptr_t argc, ...);


extern __aosl_api__ int aosl_mpq_add_listen_socket (aosl_mpq_t qid, aosl_fd_t fd, int backlog,
				aosl_sk_accepted_t accepted_f, aosl_fd_event_t event_f, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_add_named_listen_socket (aosl_mpq_t qid, aosl_fd_t fd, const char *name, int backlog,
					aosl_sk_accepted_t accepted_f, aosl_fd_event_t event_f, aosl_obj_dtor_t dtor, uintptr_t argc, ...);

extern __aosl_api__ int aosl_mpq_connect (aosl_fd_t fd, const struct sockaddr *dest_addr, socklen_t addrlen, int timeo);
extern __aosl_api__ int aosl_mpq_listen (aosl_fd_t fd, int backlog);

/**
 * The socket send function with common buffer parameter.
 * Parameters:
 *      fd: the socket fd;
 *     buf: the buffer for holding the data for writing;
 *     len: the sending buffer length;
 *   flags: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_send (aosl_fd_t sockfd, const void *buf, size_t len, int flags);

/**
 * The socket scatter-gather send function with common buffer array parameter.
 * Parameters:
 *      fd: the socket fd;
 *     iov: iov points to an array of iovec structures;
 *  iovcnt: the iov array elements count;
 *   flags: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_sendv (aosl_fd_t fd, const aosl_miov_t *iov, int iovcnt, int flags);

/**
 * The socket sendto function with common buffer parameter.
 * Parameters:
 *        fd: the socket fd;
 *       buf: the buffer for holding the data for writing;
 *       len: the sending buffer length;
 *     flags: the flags same as sendto syscall;
 * dest_addr: the destination address;
 *   addrlen: the destination address length;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_sendto (aosl_fd_t sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

/**
 * The socket scatter-gather sendto function with common buffer array parameter.
 * Parameters:
 *        fd: the socket fd;
 *    iovcnt: the iov array elements count;
 *     flags: the flags same as send syscall;
 *     flags: the flags same as sendto syscall;
 * dest_addr: the destination address;
 *   addrlen: the destination address length;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_sendtov (aosl_fd_t sockfd, const aosl_miov_t *iov, int iovcnt, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

/**
 * The socket message send function with data object parameter.
 * Parameters:
 *        fd: the socket fd;
 *     d_buf: the buffer for holding the data for writing;
 *     d_off: the buffer offset for holding the sending data;
 *     d_len: the buffer length from the offset the send operation can use,
 *            (uintptr_t)-1 for using all left space of the buf from offset;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_f_t)
 *               2. == sizeof (aosl_sk_ext_fal_t)
 *               3. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as send/sendto/sendmsg syscalls, must be set
 *            correctly before the call.
 *            d_extra could be set NULL, then no further information provided,
 *            so this case just like send (..., <flags is 0>);
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_data_send (aosl_fd_t fd, aosl_data_t d_buf, uintptr_t d_off, uintptr_t d_len, aosl_data_t d_extra);

/**
 * The socket scatter-gather message send function with data object array parameter.
 * Parameters:
 *        fd: the socket fd;
 *     d_iov: d_iov points to an array of aosl_biov_t structures for sending data;
 *  d_iovcnt: the d_iov array elements count;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_f_t)
 *               2. == sizeof (aosl_sk_ext_fal_t)
 *               3. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as send/sendto/sendmsg syscalls, must be set
 *            correctly before the call.
 *            d_extra could be set NULL, then no further information provided,
 *            so this case just like send (..., <flags is 0>);
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_data_sendv (aosl_fd_t fd, const aosl_biov_t *d_iov, int d_iovcnt, aosl_data_t d_extra);

/**
 * The socket multiple message send function with multiple message memory array parameter.
 * Parameters:
 *        fd: the socket fd;
 *       iom: io memory array for holding the data;
 *    iomcnt: the io memory array elements count;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ int aosl_msend (aosl_fd_t fd, const aosl_iom_t *iom, int iomcnt);

/**
 * The socket multiple message send function with multiple data object memory array parameter.
 * Parameters:
 *        fd: the socket fd;
 *       iob: io memory data object array for holding the data;
 *    iobcnt: the io memory data object array elements count;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ int aosl_data_msend (aosl_fd_t fd, const aosl_iob_t *iob, int iobcnt);

extern __aosl_api__ int aosl_ip_sk_bind_port_only (aosl_fd_t sk, sa_family_t af, unsigned short port);


typedef struct {
	aosl_fd_t v4;
	aosl_fd_t v6;
} aosl_ip_sk_t;

static __inline__ void aosl_ip_sk_init (aosl_ip_sk_t *sk)
{
	sk->v4 = AOSL_INVALID_FD;
	sk->v6 = AOSL_INVALID_FD;
}

extern __aosl_api__ int aosl_ip_sk_create (aosl_ip_sk_t *sk, int type, int protocol);

typedef struct {
	struct sockaddr_in v4;
	aosl_sockaddr_in6_t v6;
} aosl_ip_addr_t;

extern __aosl_api__ void aosl_ip_addr_init (aosl_ip_addr_t *addr);
extern __aosl_api__ int aosl_ip_sk_bind (const aosl_ip_sk_t *sk, const aosl_ip_addr_t *addr);

extern __aosl_api__ int aosl_mpq_ip_sk_connect (const aosl_ip_sk_t *sk, const struct sockaddr *dest_addr, int timeo);

/**
 * The IPv4/IPv6 sockets sendto function with common buffer parameter.
 * Parameters:
 *        sk: the IPv4/IPv6 socket fds;
 *       buf: the buffer for holding the data for writing;
 *       len: the sending buffer length;
 *     flags: the flags same as sendto syscall;
 * dest_addr: the destination address;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_ip_sk_sendto (const aosl_ip_sk_t *sk, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr);

/**
 * The IPv4/IPv6 sockets sendto function with data object parameter.
 * Parameters:
 *        sk: the IPv4/IPv6 socket fds;
 *     d_buf: the buffer for holding the data for writing;
 *     d_off: the buffer offset for holding the sending data;
 *     d_len: the buffer length from the offset the send operation can use,
 *            (uintptr_t)-1 for using all left space of the buf from offset;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_fal_t)
 *               2. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as send/sendto/sendmsg syscalls, must be set
 *            correctly before the call.
 *            d_extra could not be NULL;
 * Return value:
 *      <0: failed with aosl_errno set;
 *     >=0: successfully;
 **/
extern __aosl_api__ ssize_t aosl_ip_sk_data_sendto (const aosl_ip_sk_t *sk, aosl_data_t d_buf, uintptr_t d_off, uintptr_t d_len, aosl_data_t d_extra);

extern __aosl_api__ void aosl_ip_sk_close (aosl_ip_sk_t *sk);

extern __aosl_api__ int aosl_ipv6_addr_v4_mapped (const aosl_in6_addr_t *a6);
extern __aosl_api__ int aosl_ipv6_addr_nat64 (const aosl_in6_addr_t *a6);
extern __aosl_api__ int aosl_ipv6_addr_v4_compatible (const aosl_in6_addr_t *a6);

extern __aosl_api__ int aosl_ipv6_sk_addr_from_ipv4 (struct sockaddr *sk_addr_v6, const struct sockaddr *sk_addr_v4);
extern __aosl_api__ int aosl_ipv6_sk_addr_to_ipv4 (struct sockaddr *sk_addr_v6, const struct sockaddr *sk_addr_v4);


extern __aosl_api__ int aosl_ip_sk_addr_init_with_port (aosl_sk_addr_t *sk_addr, sa_family_t af, unsigned short port);

/* Structure for describing a resolved sock address information */
typedef struct {
	sa_family_t sk_af;
	int sk_type;
	int sk_prot;
	aosl_sk_addr_t sk_addr;
	socklen_t sk_addr_len;
} aosl_sk_addrinfo_t;


extern __aosl_api__ int aosl_sk_addr_ip_equal (const struct sockaddr *addr1, const struct sockaddr *addr2);
extern __aosl_api__ socklen_t aosl_sk_addr_len (const struct sockaddr *addr);

extern __aosl_api__ const char *aosl_inet_addr_str (int af, const void *addr, char *addr_buf, size_t buf_len);
extern __aosl_api__ const char *aosl_ip_sk_addr_str (const aosl_sk_addr_t *addr, char *addr_buf, size_t buf_len);
extern __aosl_api__ unsigned short aosl_ip_sk_addr_port (const aosl_sk_addr_t *addr);

extern __aosl_api__ socklen_t aosl_inet_addr_from_string (void *addr, const char *str_addr);
extern __aosl_api__ socklen_t aosl_ip_sk_addr_from_string (aosl_sk_addr_t *sk_addr, const char *str_addr, uint16_t port);

extern __aosl_api__ int aosl_net_get_ipv6_prefix (aosl_in6_addr_t *prefix);
extern __aosl_api__ void aosl_net_set_ipv6_prefix (const aosl_in6_addr_t *a6);


/**
 * Resolve host name asynchronously relative functions, these functions would queue back a function call
 * specified by f to the mpq object specified by q with the specified args.
 **/
extern __aosl_api__ int aosl_resolve_host_async (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_resolve_host_asyncv (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_tcp_resolve_host_async (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_tcp_resolve_host_asyncv (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);
extern __aosl_api__ int aosl_udp_resolve_host_async (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, ...);
extern __aosl_api__ int aosl_udp_resolve_host_asyncv (aosl_ref_t ref, const char *hostname, unsigned short port, aosl_sk_addrinfo_t *addrs, size_t addr_count, aosl_mpq_t q, aosl_mpq_func_argv_t f, uintptr_t argc, va_list args);

/**
 * The DNS resolving function which supports coroutine.
 * Parameters:
 *      d_count: the data object for holding the resolved addresses
 *               count, the length must equal to sizeof (size_t);
 *   d_hostname: the data object for holding the resolving hostname;
 *      d_addrs: the data object for holding the resolved addresses,
 *               this is an array of aosl_sk_addrinfo_t in fact;
 * Return value:
 *       0: async DNS resolve issued successfully;
 *      <0: failed with aosl_errno set;
 **/
extern __aosl_api__ int aosl_co_resolve_host (aosl_data_t d_count, aosl_data_t d_hostname, aosl_data_t d_addrs);
extern __aosl_api__ int aosl_co_resolve_tcp_host (aosl_data_t d_count, aosl_data_t d_hostname, aosl_data_t d_addrs);
extern __aosl_api__ int aosl_co_resolve_udp_host (aosl_data_t d_count, aosl_data_t d_hostname, aosl_data_t d_addrs);

/**
 * This is just a wrapper implementation of listen syscall for
 * cross platforms supporting.
 **/
extern __aosl_api__ int aosl_listen (aosl_fd_t sk, int backlog);

/**
 * The socket accept function which supports coroutine.
 * Parameters:
 *        fd: the socket fd;
 *  d_accept: the data object for holding the newsk & remote address,
 *            the len must equal to sizeof (aosl_accept_data_t);
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async accept issued successfully;
 **/
extern __aosl_api__ int aosl_co_accept (aosl_fd_t fd, aosl_data_t d_accept);

/**
 * The socket recv function which supports coroutine.
 * Parameters:
 *       fd: the socket fd;
 *    d_ret: the data object for holding return value, it can be NULL, but if
 *           non NULL, the length must equal to sizeof (ssize_t);
 *    d_buf: the buffer for holding the receiving data;
 *    d_off: the buffer offset for holding the receiving data;
 *    d_len: the buffer length from the offset the receive operation can use,
 *           (uintptr_t)-1 for using all left space of the buf from offset;
 *  d_extra: the data object for holding the flags, remote address, and ctrl
 *           etc, the length could be one of followings:
 *              1. == sizeof (aosl_sk_ext_f_t)
 *              2. == sizeof (aosl_sk_ext_fal_t)
 *              3. > sizeof (aosl_sk_ext_falc_t)
 *           the flags is same as recv/recvfrom/recvmsg syscalls, must be set
 *           correctly before the call, it is an in/out arg.
 *           d_extra could be set NULL, then no further information provided,
 *           and no any information(addr/ctrl etc) output too.
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async receive operation issued successfully, resume later;
 *      >0: receive success immediately, no async op issued, resume at once;
 **/
extern __aosl_api__ int aosl_co_recv (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off, uintptr_t d_len, aosl_data_t d_extra);

/**
 * The socket scatter-gather recv function which supports coroutine.
 * Parameters:
 *        fd: the socket fd;
 *     d_ret: the data object for holding return value, it can be NULL, but if
 *            non NULL, the length must equal to sizeof (ssize_t);
 *     d_iov: d_iov points to an array of aosl_biov_t structures for sending data;
 *  d_iovcnt: the d_iov array elements count;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_f_t)
 *               2. == sizeof (aosl_sk_ext_fal_t)
 *               3. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as recv/recvfrom/recvmsg syscalls, must be set
 *            correctly before the call, it is an in/out arg.
 *            d_extra could be set NULL, then no further information provided,
 *            and no any information(addr/ctrl etc) output too.
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async receive operation issued successfully, resume later;
 *      >0: receive success immediately, no async op issued, resume at once;
 **/
extern __aosl_api__ int aosl_co_recvv (aosl_fd_t fd, aosl_data_t d_ret, const aosl_biov_t *d_iov, int d_iovcnt, aosl_data_t d_extra);

/**
 * The socket multiple message receive function with data object array parameter
 * which supports coroutine.
 * Parameters:
 *        fd: the socket fd;
 *     d_ret: the data object for holding return value, it can be NULL, but if
 *            non NULL, the length must equal to sizeof (ssize_t);
 *       iob: io memory data object array for holding the data;
 *    iobcnt: the io memory data object array elements count;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async receive operation issued successfully, resume later;
 *      >0: receive success immediately, no async op issued, return value
 *          is the multiple receive message count, resume at once;
 **/
extern __aosl_api__ int aosl_co_mrecv (aosl_fd_t fd, aosl_data_t d_ret, const aosl_iob_t *iob, int iobcnt);

/**
 * The socket connect function which supports coroutine.
 * Parameters:
 *           fd: the socket fd;
 *        d_ret: the data object for holding the async connect
 *               done return value, it can be NULL, but if set
 *               the length must equal to sizeof (int);
 *    dest_addr: the target address;
 *      addrlen: the target address size;
 *        timeo: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async connect issued successfully, but in progress, resume later;
 *      >0: connect complete immediately, resume at once;
 **/
extern __aosl_api__ int aosl_co_connect (aosl_fd_t fd, aosl_data_t d_ret, const struct sockaddr *dest_addr, socklen_t addrlen, int timeo);

/**
 * The IPv4/IPv6 socket connect function which supports coroutine.
 * Parameters:
 *           sk: the IPv4/IPv6 sockets;
 *        d_ret: the data object for holding the async connect
 *               done return value, it can be NULL, but if set
 *               the length must equal to sizeof (int);
 *    dest_addr: the target address;
 *        timeo: the flags same as send syscall;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async connect issued successfully;
 **/
extern __aosl_api__ int aosl_co_ip_sk_connect (const aosl_ip_sk_t *sk, aosl_data_t d_ret, const struct sockaddr *dest_addr, int timeo);

/**
 * The socket send function which supports coroutine.
 * Parameters:
 *        fd: the socket fd;
 *     d_ret: the data object for holding the async send
 *            done return value, it can be NULL, but if
 *            set the length must equal to sizeof (ssize_t);
 *     d_buf: the buffer for holding the data for writing;
 *     d_off: the buffer offset for holding the sending data;
 *     d_len: the buffer length from the offset the send operation can use,
 *            (uintptr_t)-1 for using all left space of the buf from offset;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_f_t)
 *               2. == sizeof (aosl_sk_ext_fal_t)
 *               3. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as send/sendto/sendmsg syscalls, must be set
 *            correctly before the call.
 *            d_extra could be set NULL, then no further information provided,
 *            so this case just like send (..., <flags is 0>);
 * Return value:
 *      <0: failed with aosl_errno set, resume at once;
 *       0: async send operation issued successfully, resume later;
 *      >0: send success immediately, no async op issued, resume at once;
 **/
extern __aosl_api__ int aosl_co_send (aosl_fd_t fd, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off, uintptr_t d_len, aosl_data_t d_extra);

/**
 * The socket scatter-gather send function which supports coroutine.
 * Parameters:
 *        fd: the socket fd;
 *     d_ret: the data object for holding the async send
 *            done return value, it can be NULL, but if
 *            set the length must equal to sizeof (ssize_t);
 *     d_iov: d_iov points to an array of aosl_biov_t structures for sending data;
 *  d_iovcnt: the d_iov array elements count;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_f_t)
 *               2. == sizeof (aosl_sk_ext_fal_t)
 *               3. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as send/sendto/sendmsg syscalls, must be set
 *            correctly before the call.
 *            d_extra could be set NULL, then no further information provided,
 *            so this case just like send (..., <flags is 0>);
 * Return value:
 *      <0: failed with aosl_errno set, resume at once;
 *       0: async send operation issued successfully, resume later;
 *      >0: send success immediately, no async op issued, resume at once;
 **/
extern __aosl_api__ int aosl_co_sendv (aosl_fd_t fd, aosl_data_t d_ret, const aosl_biov_t *d_iov, int d_iovcnt, aosl_data_t d_extra);

/**
 * The socket multiple message send function with multiple data object memory array parameter
 * which supports coroutine.
 * Parameters:
 *        fd: the socket fd;
 *     d_ret: the data object for holding the async send
 *            done return value, it can be NULL, but if
 *            set the length must equal to sizeof (int);
 *       iob: io memory data object array for holding the data;
 *    iobcnt: the io memory data object array elements count;
 * Return value:
 *      <0: failed with aosl_errno set, resume at once;
 *       0: async send operation issued successfully, resume later;
 *      >0: send success immediately, no async op issued, resume at once;
 **/
extern __aosl_api__ int aosl_co_msend (aosl_fd_t fd, aosl_data_t d_ret, const aosl_iob_t *iob, int iobcnt);

/**
 * The IPv4/IPv6 socket sendto function which supports coroutine.
 * Parameters:
 *        sk: the IPv4/IPv6 sockets;
 *     d_ret: the data object for holding the async send
 *            done return value, it can be NULL, but if
 *            set the length must equal to sizeof (ssize_t);
 *     d_buf: the buffer for holding the data for writing;
 *     d_off: the buffer offset for holding the sending data;
 *     d_len: the buffer length from the offset the send operation can use,
 *            (uintptr_t)-1 for using all left space of the buf from offset;
 *   d_extra: the data object for holding the flags, remote address, and ctrl
 *            etc, the length could be one of followings:
 *               1. == sizeof (aosl_sk_ext_fal_t)
 *               2. > sizeof (aosl_sk_ext_falc_t)
 *            the flags is same as send/sendto/sendmsg syscalls, must be set
 *            correctly before the call.
 *            d_extra could not be NULL;
 * Return value:
 *      <0: failed with aosl_errno set, resume at once;
 *       0: async send operation issued successfully, resume later;
 *      >0: send success immediately, no async op issued, resume at once;
 **/
extern __aosl_api__ int aosl_co_ip_sk_sendto (const aosl_ip_sk_t *sk, aosl_data_t d_ret,
				aosl_data_t d_buf, uintptr_t d_off, uintptr_t d_len, aosl_data_t d_extra);



#ifdef __cplusplus
}
#endif


#endif /* __AOSL_MPQ_NET_H__ */