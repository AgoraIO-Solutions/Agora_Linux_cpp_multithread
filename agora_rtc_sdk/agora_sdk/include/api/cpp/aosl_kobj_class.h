/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Dec 8th, 2022
 * Module:	AOSL value object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2022 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_KOBJ_CPP_H__
#define __AOSL_KOBJ_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_kobj.h>

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <memory>
#include <type_traits>
#include <utility>
#endif


class aosl_kobj_class {
private:
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	aosl_kobj_class () = delete;
	aosl_kobj_class (const aosl_kobj_class &) = delete;
	aosl_kobj_class (aosl_kobj_class &&) = delete;
	aosl_kobj_class &operator = (const aosl_kobj_class &) = delete;
	aosl_kobj_class &operator = (aosl_kobj_class &&) = delete;
	~aosl_kobj_class () = delete;
#else
	aosl_kobj_class ();
	aosl_kobj_class (const aosl_kobj_class &);
	aosl_kobj_class &operator = (const aosl_kobj_class &);
	~aosl_kobj_class ();
#endif

public:
	static void *notify_register (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, aosl_kobj_notify_t f, aosl_obj_dtor_t dtor, uintptr_t argc, ...)
	{
		va_list args;
		void *err;

		va_start (args, argc);
		err = aosl_kobj_notify_register_args (dfd, name, touch, qid, ref, f_name, f, dtor, argc, args);
		va_end (args);
		return err;
	}

	static void *notify_register_args (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, aosl_kobj_notify_t f, aosl_obj_dtor_t dtor, uintptr_t argc, va_list args)
	{
		return aosl_kobj_notify_register_args (dfd, name, touch, qid, ref, f_name, f, dtor, argc, args);
	}

	static void *notify_register_argv (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, aosl_kobj_notify_t f, aosl_obj_dtor_t dtor, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_kobj_notify_register_argv (dfd, name, touch, qid, ref, f_name, f, dtor, argc, argv);
	}

	static int notify_unregister (aosl_kfd_t dfd, const char *name, aosl_mpq_t qid, aosl_ref_t ref, void *handle)
	{
		return aosl_kobj_notify_unregister (dfd, name, handle);
	}

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
public:
	/* __notify_lambda_t: void (aosl_refobj_t robj, const char *dirpath, const char *name, void *notif_data) */
	template <typename __notify_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__notify_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<const char *>(),
			std::declval<void *>()))>::value, int>::type = 0>
	static void *notify_register (aosl_kfd_t dfd, const char *name, int touch, aosl_mpq_t qid, aosl_ref_t ref, const char *f_name, __notify_lambda_t &&task)
	{
		__notify_lambda_t *task_obj = new __notify_lambda_t (std::move (task));
		void *err = notify_register (dfd, name, touch, qid, ref, f_name, ____notify_f<typename std::remove_reference<__notify_lambda_t>::type>,
														____notify_dtor<typename std::remove_reference<__notify_lambda_t>::type>, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

private:
	/* __notify_lambda_t: void (aosl_refobj_t robj, const char *dirpath, const char *name, void *notif_data) */
	template <typename __notify_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__notify_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<const char *>(),
			std::declval<void *>()))>::value, int>::type = 0>
	static void ____notify_f (aosl_refobj_t robj, const char *dirpath, const char *name, void *notif_data, uintptr_t argc, uintptr_t argv [])
	{
		__notify_lambda_t *task_obj = reinterpret_cast<__notify_lambda_t *>(argv [0]);
		(*task_obj) (robj, dirpath, name, notif_data);
	}

	/* __notify_lambda_t: void (aosl_refobj_t robj, const char *dirpath, const char *name, void *notif_data) */
	template <typename __notify_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__notify_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<const char *>(),
			std::declval<void *>()))>::value, int>::type = 0>
	static void ____notify_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__notify_lambda_t *task_obj = reinterpret_cast<__notify_lambda_t *>(argv [0]);
		delete task_obj;
	}
#endif /* C++11 */
};


#endif /* __AOSL_KOBJ_CPP_H__ */