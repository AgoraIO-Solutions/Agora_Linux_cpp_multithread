/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 22nd, 2024
 * Module:	AOSL packet splitter API header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_PKTSPLTR_H__
#define __AOSL_PKTSPLTR_H__

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_data.h>
#include <api/aosl_ref.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * The data packet check callback function is provided by the user for determining
 * whether the data has contained a complet user level packet, as following:
 * Parameters:
 *    data: the data buffer holding the packet;
 *     len: the data length in bytes in the buffer;
 *     ...: va_arg(args, void *) is the opaque arg passed in create function,
 *          ignore it if needless;
 * Return value:
 *      <0: indicate some error occurs;
 *       0: no error, but the whole packet is not ready so far;
 *      >0: the whole packet length
 **/
typedef ssize_t (*aosl_co_chk_pkt_t) (const void *data, size_t len, ...);

/**
 * The byte stream read coroutine function.
 * Parameters:
 *      arg: the opaque arg for the real read function;
 *    d_ret: the data object for holding return value, the
 *           length must be sizeof (ssize_t);
 *    d_buf: the buffer for holding the reading data;
 *    d_off: the buffer offset for holding the reading data;
 *    d_len: the buffer length from the offset the read operation can use,
 *           (uintptr_t)-1 for using all left space of the buf from offset;
 *  d_extra: the data object for holding the user defined extra info, and
 *           set to NULL if no extra info needed. If it is not NULL, the
 *           first field of this extra info must be 'uintptr_t extra_size',
 *           and init the extra_size with the total size of extra info in
 *           bytes;
 * Return value:
 *      <0: failed with aosl_errno set;
 *       0: async read issued successfully;
 **/
typedef int (*aosl_co_read_t) (void *arg, aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off, uintptr_t d_len, aosl_data_t d_extra);

/**
 * Reset the specified async result object to non signaled state.
 * Parameters:
 *      chk_pkt_f: check whether the received data build up a complete packet now;
 *         read_f: callback function for co read more data to build up a packet;
 *        d_extra: the data object for holding the user defined extra info, and
 *                 set to NULL if no extra info needed. If it is not NULL, the
 *                 first field of this extra info must be 'uintptr_t extra_size',
 *                 and init the extra_size with the total size of extra info in
 *                 bytes;
 *   max_pkt_size: the maximum packet size in bytes;
 *            arg: opaque arg which will be passed to chk_pkt_f as ... and read_f;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 **/
extern __aosl_api__ aosl_ref_t aosl_co_pktspltr_create (aosl_co_chk_pkt_t chk_pkt_f, aosl_co_read_t read_f, aosl_data_t d_extra, size_t max_pkt_size, void *arg);

typedef struct {
    aosl_data_t d_buf; /* buf object if !NULL */
    intptr_t d_offerr; /* >=0: off; <0: err */
    uintptr_t d_len; /* packet len */
} aosl_co_pkt_t;

/**
 * Reset the specified async result object to non signaled state.
 * Parameters:
 *    spltr: the packet splitter ref object;
 *    d_pkt: the data object with content of aosl_co_pkt_t;
 * Return value:
 *        <0: error occured, and aosl_errno indicates which error;
 *       >=0: successful;
 **/
extern __aosl_api__ int aosl_co_pktspltr_read (aosl_ref_t spltr, aosl_data_t d_pkt);


#ifdef __cplusplus
}
#endif

#endif /* __AOSL_PKTSPLTR_H__ */