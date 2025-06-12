/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 7th, 2024
 * Module:	AOSL iovec relative API header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_IOVEC_H__
#define __AOSL_IOVEC_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_data.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	aosl_data_t d_buf;
	uintptr_t d_off;
	uintptr_t d_len;
} aosl_biov_t;

typedef struct {
	const aosl_biov_t *d_iov;
	uintptr_t d_iovcnt;
} aosl_biovs_t;

typedef struct {
	ssize_t ret_val;
	aosl_miov_t *iov;
	uintptr_t iovcnt;
	uintptr_t *extra_size;
#ifdef _WIN32
	OVERLAPPED *ovlp;
#endif
} aosl_iom_t;

typedef struct {
	aosl_data_t d_ret; /* content: ssize_t */
	aosl_biov_t *d_iov;
	uintptr_t d_iovcnt;
	aosl_data_t d_extra;
} aosl_iob_t;


#define AOSL_IOBUF_MAX 32


#ifdef __cplusplus
}
#endif


#endif /* __AOSL_IOVEC_H__ */