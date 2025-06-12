/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	May 6th, 2024
 * Module:	AOSL software bus relative API C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_SBUS_CPP_H__
#define __AOSL_SBUS_CPP_H__


/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>
#include <api/aosl_data.h>
#include <api/aosl_sbus.h>
#include <api/aosl_errno.h>
#include <api/cpp/aosl_data_class.h>

#include <memory>
#include <type_traits>
#include <utility>

class aosl_sbus_class_base {
private:
	aosl_sbus_class_base () = delete;
	aosl_sbus_class_base (const aosl_sbus_class_base &) = delete;
	aosl_sbus_class_base (aosl_sbus_class_base &&) = delete;
	aosl_sbus_class_base &operator = (const aosl_sbus_class_base &) = delete;
	aosl_sbus_class_base &operator = (aosl_sbus_class_base &&) = delete;
	~aosl_sbus_class_base () = delete;

public:
	/* __sbus_data_lambda_t: void (aosl_refobj_t robj, const char *name, aosl_data_t data) */
	template <typename __sbus_data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_data_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<aosl_data_t>()))>::value>::type = 0>
	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, __sbus_data_lambda_t&& task)
	{
		__sbus_data_lambda_t *task_obj = new __sbus_data_lambda_t (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_data<typename std::remove_reference<__sbus_data_lambda_t>::type>,
														____action_dtor<typename std::remove_reference<__sbus_data_lambda_t>::type>, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	/* __sbus_data_2_args_lambda_t: void (const char *name, aosl_data_t data) */
	template <typename __sbus_data_2_args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_data_2_args_lambda_t>()(
			std::declval<const char *>(),
			std::declval<aosl_data_t>()))>::value, int>::type = 0>
	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, __sbus_data_2_args_lambda_t&& task)
	{
		__sbus_data_2_args_lambda_t *task_obj = new __sbus_data_2_args_lambda_t (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_data_2args<typename std::remove_reference<__sbus_data_2_args_lambda_t>::type>,
														____action_dtor_2args<typename std::remove_reference<__sbus_data_2_args_lambda_t>::type>, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	static int action_del (const char *name, void *h_action)
	{
		return aosl_sbus_action_del (name, h_action);
	}

	static int send (const char *name, aosl_data_class &data)
	{
		return aosl_sbus_send (name, data);
	}

private:
	/* __sbus_data_lambda_t: void (aosl_refobj_t robj, const char *name, aosl_data_t data) */
	template <typename __sbus_data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_data_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<aosl_data_t>()))>::value, int>::type = 0>
	static void ____sbus_data (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		__sbus_data_lambda_t *task_obj = reinterpret_cast<__sbus_data_lambda_t *>(argv [0]);
		(*task_obj) (robj, name, data);
	}

	/* __sbus_data_lambda_t: void (aosl_refobj_t robj, const char *name, aosl_data_t data) */
	template <typename __sbus_data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_data_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<aosl_data_t>()))>::value, int>::type = 0>
	static void ____action_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__sbus_data_lambda_t *task_obj = reinterpret_cast<__sbus_data_lambda_t *>(argv [0]);
		delete task_obj;
	}

	/* __sbus_data_2_args_lambda_t: void (const char *name, aosl_data_t data) */
	template <typename __sbus_data_2_args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_data_2_args_lambda_t>()(
			std::declval<const char *>(),
			std::declval<aosl_data_t>()))>::value, int>::type = 0>
	static void ____sbus_data_2args (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		if (!aosl_is_free_only (robj)) {
			__sbus_data_2_args_lambda_t *task_obj = reinterpret_cast<__sbus_data_2_args_lambda_t *>(argv [0]);
			(*task_obj) (name, data);
		}
	}

	/* __sbus_data_2_args_lambda_t: void (const char *name, aosl_data_t data) */
	template <typename __sbus_data_2_args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_data_2_args_lambda_t>()(
			std::declval<const char *>(),
			std::declval<aosl_data_t>()))>::value, int>::type = 0>
	static void ____action_dtor_2args (uintptr_t argc, uintptr_t argv [])
	{
		__sbus_data_2_args_lambda_t *task_obj = reinterpret_cast<__sbus_data_2_args_lambda_t *>(argv [0]);
		delete task_obj;
	}
};

/**
 * Template class for accessing a software bus with the data var format
 * specified by type T.
 **/
template<typename T>
class aosl_sbus_class: public aosl_sbus_class_base {
private:
	aosl_sbus_class () = delete;
	aosl_sbus_class (const aosl_sbus_class &) = delete;
	aosl_sbus_class (aosl_sbus_class &&) = delete;
	aosl_sbus_class &operator = (const aosl_sbus_class &) = delete;
	aosl_sbus_class &operator = (aosl_sbus_class &&) = delete;
	~aosl_sbus_class () = delete;

public:
	/* __sbus_var_lambda_t: void (aosl_refobj_t robj, const char *name, aosl_data_var<T> &var) */
	template <typename __sbus_var_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_var_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<aosl_data_var<T> &>()))>::value, int>::type = 0>
	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, __sbus_var_lambda_t&& task)
	{
		__sbus_var_lambda_t *task_obj = new __sbus_var_lambda_t (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_var<typename std::remove_reference<__sbus_var_lambda_t>::type>,
														____action_var_dtor<typename std::remove_reference<__sbus_var_lambda_t>::type>, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	/* __sbus_var_2args_lambda_t: void (const char *name, aosl_data_var<T> &var) */
	template <typename __sbus_var_2args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_var_2args_lambda_t>()(
			std::declval<const char *>(),
			std::declval<aosl_data_var<T> &>()))>::value, int>::type = 0>
	static void *action_add (const char *name, aosl_mpq_t qid, aosl_ref_t ref, uintptr_t max, const char *f_name, __sbus_var_2args_lambda_t&& task)
	{
		__sbus_var_2args_lambda_t *task_obj = new __sbus_var_2args_lambda_t (std::move (task));
		void *err = aosl_sbus_action_add (name, qid, ref, max, f_name, ____sbus_var_2args<typename std::remove_reference<__sbus_var_2args_lambda_t>::type>,
														____action_var_dtor_2args<typename std::remove_reference<__sbus_var_2args_lambda_t>::type>, 1, task_obj);
		if (err == NULL)
			delete task_obj;

		return err;
	}

	static int send (const char *name, const aosl_data_var<T> &var)
	{
		return aosl_sbus_send (name, var);
	}

	static int send (const char *name, aosl_data_t data)
	{
		if (aosl_data_len (data) == sizeof (T))
			return aosl_sbus_send (name, data);

		aosl_errno = EINVAL;
		return -1;
	}

private:
	/* __sbus_var_lambda_t: void (aosl_refobj_t robj, const char *name, aosl_data_var<T> &var) */
	template <typename __sbus_var_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_var_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<aosl_data_var<T> &>()))>::value, int>::type = 0>
	static void ____sbus_var (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		__sbus_var_lambda_t *task_obj = reinterpret_cast<__sbus_var_lambda_t *>(argv [0]);
		aosl_data_var<T> var ((aosl_data_arg<aosl_data_t> (data)));
		(*task_obj) (robj, name, var);
	}

	/* __sbus_var_lambda_t: void (aosl_refobj_t robj, const char *name, aosl_data_var<T> &var) */
	template <typename __sbus_var_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_var_lambda_t>()(
			std::declval<aosl_refobj_t>(),
			std::declval<const char *>(),
			std::declval<aosl_data_var<T> &>()))>::value, int>::type = 0>
	static void ____action_var_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__sbus_var_lambda_t *task_obj = reinterpret_cast<__sbus_var_lambda_t *>(argv [0]);
		delete task_obj;
	}

	/* __sbus_var_2args_lambda_t: void (const char *name, aosl_data_var<T> &var) */
	template <typename __sbus_var_2args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_var_2args_lambda_t>()(
			std::declval<const char *>(),
			std::declval<aosl_data_var<T> &>()))>::value, int>::type = 0>
	static void ____sbus_var_2args (aosl_refobj_t robj, const char *name, aosl_data_t data, uintptr_t argc, uintptr_t argv [])
	{
		if (!aosl_is_free_only (robj)) {
			__sbus_var_2args_lambda_t *task_obj = reinterpret_cast<__sbus_var_2args_lambda_t *>(argv [0]);
			aosl_data_var<T> var ((aosl_data_arg<aosl_data_t> (data)));
			(*task_obj) (name, var);
		}
	}

	/* __sbus_var_2args_lambda_t: void (const char *name, aosl_data_var<T> &var) */
	template <typename __sbus_var_2args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__sbus_var_2args_lambda_t>()(
			std::declval<const char *>(),
			std::declval<aosl_data_var<T> &>()))>::value, int>::type = 0>
	static void ____action_var_dtor_2args (uintptr_t argc, uintptr_t argv [])
	{
		__sbus_var_2args_lambda_t *task_obj = reinterpret_cast<__sbus_var_2args_lambda_t *>(argv [0]);
		delete task_obj;
	}
};
#endif /* C++11 */


#endif /* __AOSL_SBUS_CPP_H__ */