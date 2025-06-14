/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Dec 13th, 2020
 * Module:	AOSL so module interface definition header file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_SO_H__
#define __AOSL_SO_H__

#include <api/aosl_defs.h>
#include <api/aosl_log.h>
#include <api/aosl_errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AOSL_SO_DEFAULT_VER (0)

extern __aosl_api__ int aosl_so_register (const char *name, const void *mod, uintptr_t version);
extern __aosl_api__ int aosl_so_register_group (const char *name, const void *mod, uint32_t version, uint32_t gid);

#if defined (__GNUC__)
#define AOSL_SO_REGISTER_VER_GROUP(name, version, gid) \
	static void __attribute__  ((constructor, used)) _##name##_so_reg_ctor (void) \
	{ \
		if (aosl_so_register_group (#name, (const void *)_##name##_so_reg_ctor, version, gid) < 0) \
			aosl_log (AOSL_LOG_WARNING, "so lib %s register failed, err %d.", #name, aosl_errno); \
	}
#elif defined (_MSC_VER)
#pragma section (".CRT$XIG", long, read)

#define AOSL_SO_REGISTER_VER_GROUP(name, version, gid) \
	static int _##name##_so_reg_ctor (void) \
	{ \
		if (aosl_so_register_group (#name, (const void *)_##name##_so_reg_ctor, version, gid) < 0) \
			aosl_log (AOSL_LOG_WARNING, "so lib %s register failed, err %d.", #name, aosl_errno); \
		return 0; \
	} \
	__declspec(allocate(".CRT$XIG")) int (*_##name##_so_module_ctor_f) (void) = _##name##_so_reg_ctor;
#else
#error Unsupported Toolchain!
#endif

#define AOSL_SO_REGISTER_VER(name, version) \
	AOSL_SO_REGISTER_VER_GROUP (name, version, 1/* the first real gid */)

#define AOSL_SO_REGISTER_GROUP(name, gid) \
	AOSL_SO_REGISTER_VER_GROUP (name, AOSL_SO_DEFAULT_VER, gid)

#define AOSL_SO_REGISTER(name) \
	AOSL_SO_REGISTER_VER_GROUP(name, AOSL_SO_DEFAULT_VER, 1/* the first real gid */)


#ifdef __cplusplus
}
#endif

#endif /* __AOSL_SO_H__ */