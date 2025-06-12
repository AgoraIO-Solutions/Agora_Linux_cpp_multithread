/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 19th, 2018
 * Module:	AOSL task object for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_TASK_CLASS_H__
#define __AOSL_TASK_CLASS_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mpq.h>
#include <api/aosl_ref.h>
#include <api/aosl_task.h>
#include <api/aosl_async.h>
#include <api/cpp/aosl_ref_class.h>

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <memory>
#include <type_traits>
#include <utility>
#endif


class aosl_task_class: public aosl_ref_class {
public:
	class aosl_ref_t_oop: public aosl_ref_class::aosl_ref_t_oop {
	private:
		/**
		 * We do not allow create any object of this class directly,
		 * only the pointer of this class is significant.
		 **/
		aosl_ref_t_oop ();

	public:
		static aosl_ref_t_oop *create (aosl_task_type_t type, void *arg = NULL, aosl_ref_dtor_t dtor = NULL,
									bool serial = true, aosl_mpq_t st_op_q = aosl_mpq_main (), bool resume = true,
									aosl_task_res_wait_t wait_f = NULL)
		{
			return (aosl_ref_t_oop *)aosl_task_create (arg, dtor, type, (int)(serial == true), st_op_q, (int)(resume == true), wait_f);
		}

		static aosl_ref_t_oop *from_aosl_ref_t (aosl_ref_t ref)
		{
			return (aosl_ref_t_oop *)ref;
		}

		aosl_task_type_t get_type ()
		{
			return aosl_task_get_type (ref ());
		}

		int exec (const char *f_name, aosl_task_func_t f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_task_exec_args (ref (), f_name, f, argc, args);
			va_end (args);

			return err;
		}

		int exec_args (const char *f_name, aosl_task_func_t f, uintptr_t argc, va_list args)
		{
			return aosl_task_exec_args (ref (), f_name, f, argc, args);
		}

		int exec_argv (const char *f_name, aosl_task_func_t f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_task_exec_argv (ref (), f_name, f, argc, argv);
		}

		int exclusive_exec (aosl_task_func_t exclusive_f, uintptr_t argc, ...)
		{
			va_list args;
			int err;

			va_start (args, argc);
			err = aosl_task_exclusive_exec_args (ref (), exclusive_f, argc, args);
			va_end (args);

			return err;
		}

		int exclusive_exec_args (aosl_task_func_t exclusive_f, uintptr_t argc, va_list args)
		{
			return aosl_task_exclusive_exec_args (ref (), exclusive_f, argc, args);
		}

		int exclusive_exec_argv (aosl_task_func_t exclusive_f, uintptr_t argc, uintptr_t argv [])
		{
			return aosl_task_exclusive_exec_argv (ref (), exclusive_f, argc, argv);
		}

		int waiting_ops_count ()
		{
			return aosl_task_waiting_ops_count (ref ());
		}

		int remove_waiting_ops_head ()
		{
			return aosl_task_remove_waiting_ops_head (ref ());
		}

		int async_done ()
		{
			return aosl_task_async_done (ref ());
		}

		int async_done (uintptr_t opaque)
		{
			return aosl_task_async_done_opaque (ref (), opaque);
		}

		/* C++11 lambda encapsulations */
	#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	public:
		/* __task_exec_lambda_t: void (aosl_task_act_t action, uintptr_t opaque, aosl_refobj_t tobj, uintptr_t &res) */
		template <typename __task_exec_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_lambda_t>()(
												std::declval<aosl_task_act_t>(),
												std::declval<uintptr_t>(),
												std::declval<aosl_refobj_t>(),
												std::declval<uintptr_t &>()
											))>::value, int>::type = 0>
		int exec (const char *f_name, __task_exec_lambda_t&& task)
		{
			__task_exec_lambda_t *exec_f = new __task_exec_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (f_name, ____task_exec_f<typename std::remove_reference<__task_exec_lambda_t>::type>, 2, 0, exec_f);
			if (err < 0)
				delete exec_f;

			return err;
		}

		/* __task_exec_2args_lambda_t: void (aosl_task_act_t action, uintptr_t opaque) */
		template <typename __task_exec_2args_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_2args_lambda_t>()(
													std::declval<aosl_task_act_t>(),
													std::declval<uintptr_t>()
												))>::value, int>::type = 0>
		int exec (const char *f_name, __task_exec_2args_lambda_t&& task)
		{
			__task_exec_2args_lambda_t *exec_f = new __task_exec_2args_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (f_name, ____task_exec_2args_f<typename std::remove_reference<__task_exec_2args_lambda_t>::type>, 2, 0, exec_f);
			if (err < 0)
				delete exec_f;

			return err;
		}

		/* __task_exec_1arg_lambda_t: void (aosl_task_act_t action) */
		template <typename __task_exec_1arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_1arg_lambda_t>()(
												std::declval<aosl_task_act_t>()
											))>::value, int>::type = 0>
		int exec (const char *f_name, __task_exec_1arg_lambda_t&& task)
		{
			__task_exec_1arg_lambda_t *exec_f = new __task_exec_1arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (f_name, ____task_exec_1arg_f<typename std::remove_reference<__task_exec_1arg_lambda_t>::type>, 2, 0, exec_f);
			if (err < 0)
				delete exec_f;

			return err;
		}

		/* __task_exec_1arg_lambda_t: void (aosl_task_act_t action) */
		template <typename __task_exec_1arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_1arg_lambda_t>()(
												std::declval<aosl_task_act_t>()
											))>::value, int>::type = 0>
		int exclusive_exec (__task_exec_1arg_lambda_t&& task)
		{
			__task_exec_1arg_lambda_t *exc_exec_f = new __task_exec_1arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exclusive_exec (____exclusive_exec_f<typename std::remove_reference<__task_exec_1arg_lambda_t>::type>, 1, exc_exec_f);
			if (err < 0)
				delete exc_exec_f;

			return err;
		}

	private:
		/* __task_exec_lambda_t: void (aosl_task_act_t action, uintptr_t opaque, aosl_refobj_t tobj, uintptr_t &res) */
		template <typename __task_exec_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_lambda_t>()(
												std::declval<aosl_task_act_t>(),
												std::declval<uintptr_t>(),
												std::declval<aosl_refobj_t>(),
												std::declval<uintptr_t &>()
											))>::value, int>::type = 0>
		static void ____task_exec_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_exec_lambda_t *exec_f = reinterpret_cast<__task_exec_lambda_t *>(argv [1]);
			(*exec_f) (action, opaque, tobj, argv [0]);
			if (action != aosl_task_act_exec) {
				/**
				 * Do not free the task object for the task object 'run' case,
				 * because the task object is still used later.
				 * -- Lionfore Hao Nov 19th, 2018
				 **/
				delete exec_f;
			}
		}

		/**
		 * Windows SDK 14.35.32215 uses __stdcall, so do not implement these lambdas
		 * via same callback function with full args, using an separate callback func
		 * instead.
		 **/
		/* __task_exec_2args_lambda_t: void (aosl_task_act_t action, uintptr_t opaque) */
		template <typename __task_exec_2args_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_2args_lambda_t>()(
												std::declval<aosl_task_act_t>(),
												std::declval<uintptr_t>()
											))>::value, int>::type = 0>
		static void ____task_exec_2args_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_exec_2args_lambda_t *exec_f = reinterpret_cast<__task_exec_2args_lambda_t *>(argv [1]);
			(*exec_f) (action, opaque);
			if (action != aosl_task_act_exec) {
				/**
				 * Do not free the task object for the task object 'run' case,
				 * because the task object is still used later.
				 * -- Lionfore Hao Nov 19th, 2018
				 **/
				delete exec_f;
			}
		}

		/* __task_exec_1arg_lambda_t: void (aosl_task_act_t action) */
		template <typename __task_exec_1arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_1arg_lambda_t>()(
												std::declval<aosl_task_act_t>()
											))>::value, int>::type = 0>
		static void ____task_exec_1arg_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_exec_1arg_lambda_t *exec_f = reinterpret_cast<__task_exec_1arg_lambda_t *>(argv [1]);
			(*exec_f) (action);
			if (action != aosl_task_act_exec) {
				/**
				 * Do not free the task object for the task object 'run' case,
				 * because the task object is still used later.
				 * -- Lionfore Hao Nov 19th, 2018
				 **/
				delete exec_f;
			}
		}

		/* __task_exec_1arg_lambda_t: void (aosl_task_act_t action) */
		template <typename __task_exec_1arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_1arg_lambda_t>()(
												std::declval<aosl_task_act_t>()
											))>::value, int>::type = 0>
		static void ____exclusive_exec_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_exec_1arg_lambda_t *exc_exec_f = reinterpret_cast<__task_exec_1arg_lambda_t *>(argv [0]);
			(*exc_exec_f) (action);
			delete exc_exec_f;
		}

	public:
		/* __task_async_lambda_t: void (uintptr_t opaque, aosl_refobj_t tobj, uintptr_t &res) */
		template <typename __task_async_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_async_lambda_t>()(
												std::declval<uintptr_t>(),
												std::declval<aosl_refobj_t>(),
												std::declval<uintptr_t &>()
											))>::value, int>::type = 0>
		int async (const char *f_name, __task_async_lambda_t&& task)
		{
			__task_async_lambda_t *async_f = new __task_async_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (f_name, ____task_async_f<typename std::remove_reference<__task_async_lambda_t>::type>, 2, 0, async_f);
			if (err < 0)
				delete async_f;

			return err;
		}

		/* __task_async_1arg_lambda_t: void (uintptr_t opaque) */
		template <typename __task_async_1arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_async_1arg_lambda_t>()(
												std::declval<uintptr_t>()
											))>::value, int>::type = 0>
		int async (const char *f_name, __task_async_1arg_lambda_t&& task)
		{
			__task_async_1arg_lambda_t *async_f = new __task_async_1arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (f_name, ____task_async_1arg_f<typename std::remove_reference<__task_async_1arg_lambda_t>::type>, 2, 0, async_f);
			if (err < 0)
				delete async_f;

			return err;
		}

		/* __task_async_0arg_lambda_t: void (void) */
		template <typename __task_async_0arg_lambda_t>
		/**
		 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
		 * because the buggy MSVC compiler version 14.25.28610 will report:
		 *  - error C2672: XXX: no matching overloaded function found
		 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
		 * So, we use the return type style SFINAE here instead.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		typename std::enable_if<std::is_void<decltype(std::declval<__task_async_0arg_lambda_t>()())>::value, int>::type
		async (const char *f_name, __task_async_0arg_lambda_t&& task)
		{
			__task_async_0arg_lambda_t *async_f = new __task_async_0arg_lambda_t (std::move (task));
			int err = aosl_ref_t_oop::exec (f_name, ____task_async_0arg_f<typename std::remove_reference<__task_async_0arg_lambda_t>::type>, 2, 0, async_f);
			if (err < 0)
				delete async_f;

			return err;
		}

	private:
		/* __task_async_lambda_t: void (uintptr_t opaque, aosl_refobj_t tobj, uintptr_t &res) */
		template <typename __task_async_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_async_lambda_t>()(
												std::declval<uintptr_t>(),
												std::declval<aosl_refobj_t>(),
												std::declval<uintptr_t &>()
											))>::value, int>::type = 0>
		static void ____task_async_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_async_lambda_t *async_f = reinterpret_cast<__task_async_lambda_t *>(argv [1]);
			if (action == aosl_task_act_exec) {
				(*async_f) (opaque, tobj, argv [0]);
			} else {
				delete async_f;
			}
		}

		/**
		 * Windows SDK 14.35.32215 uses __stdcall, so do not implement these lambdas
		 * via same callback function with full args, using an separate callback func
		 * instead.
		 **/
		/* __task_async_1arg_lambda_t: void (uintptr_t opaque) */
		template <typename __task_async_1arg_lambda_t,
			typename std::enable_if<std::is_void<decltype(std::declval<__task_async_1arg_lambda_t>()(
												std::declval<uintptr_t>()
											))>::value, int>::type = 0>
		static void ____task_async_1arg_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_async_1arg_lambda_t *async_f = reinterpret_cast<__task_async_1arg_lambda_t *>(argv [1]);
			if (action == aosl_task_act_exec) {
				(*async_f) (opaque);
			} else {
				delete async_f;
			}
		}

		/* __task_async_0arg_lambda_t: void (void) */
		template <typename __task_async_0arg_lambda_t>
		/**
		 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
		 * because the buggy MSVC compiler version 14.25.28610 will report:
		 *  - error C2672: XXX: no matching overloaded function found
		 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
		 * So, we use the return type style SFINAE here instead.
		 * -- Lionfore Hao Apr 15th, 2025
		 **/
		static typename std::enable_if<std::is_void<decltype(std::declval<__task_async_0arg_lambda_t>()())>::value, void>::type
		____task_async_0arg_f (aosl_refobj_t tobj, aosl_task_act_t action, uintptr_t opaque, uintptr_t argc, uintptr_t argv [])
		{
			__task_async_0arg_lambda_t *async_f = reinterpret_cast<__task_async_0arg_lambda_t *>(argv [1]);
			if (action == aosl_task_act_exec) {
				(*async_f) ();
			} else {
				delete async_f;
			}
		}
	#endif /* C++11 */
	};

public:
	aosl_task_class (aosl_task_type_t type, bool serial = false, aosl_mpq_t st_op_q = aosl_mpq_main (), bool resume = true, aosl_task_res_wait_t wait_f = NULL)
				: aosl_ref_class (aosl_ref_t_oop::create (type, this, __dtor, serial, st_op_q, resume, wait_f))
	{
		if (aosl_ref_invalid (ref_oop ()))
			abort ();
	}

	aosl_task_class (): aosl_ref_class (AOSL_REF_INVALID)
	{
	}

	aosl_ref_t_oop *ref_oop () const
	{
		return (aosl_ref_t_oop *)aosl_ref_class::ref_oop ();
	}

	aosl_task_type_t get_type ()
	{
		return ref_oop ()->get_type ();
	}

	int exec (const char *f_name, aosl_task_func_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ref_oop ()->exec_args (f_name, f, argc, args);
		va_end (args);

		return err;
	}

	int exec_args (const char *f_name, aosl_task_func_t f, uintptr_t argc, va_list args)
	{
		return ref_oop ()->exec_args (f_name, f, argc, args);
	}

	int exec_argv (const char *f_name, aosl_task_func_t f, uintptr_t argc, uintptr_t argv [])
	{
		return ref_oop ()->exec_argv (f_name, f, argc, argv);
	}

	int exclusive_exec (aosl_task_func_t exclusive_f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = ref_oop ()->exclusive_exec_args (exclusive_f, argc, args);
		va_end (args);

		return err;
	}

	int exclusive_exec_args (aosl_task_func_t exclusive_f, uintptr_t argc, va_list args)
	{
		return ref_oop ()->exclusive_exec_args (exclusive_f, argc, args);
	}

	int exclusive_exec_argv (aosl_task_func_t exclusive_f, uintptr_t argc, uintptr_t argv [])
	{
		return ref_oop ()->exclusive_exec_argv (exclusive_f, argc, argv);
	}

	int waiting_ops_count ()
	{
		return ref_oop ()->waiting_ops_count ();
	}

	int remove_waiting_ops_head ()
	{
		return ref_oop ()->remove_waiting_ops_head ();
	}

	int async_done ()
	{
		return ref_oop ()->async_done ();
	}

	int async_done (uintptr_t opaque)
	{
		return ref_oop ()->async_done (opaque);
	}

protected:
	/* We do not allow delete this object directly. */
	virtual ~aosl_task_class ()
	{
	}

private:
	static void __dtor (void *arg)
	{
		aosl_task_class *__this = (aosl_task_class *)arg;
		::delete __this;
	}

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
public:
	/* __task_exec_lambda_t: void (aosl_task_act_t action, uintptr_t opaque, aosl_refobj_t tobj, uintptr_t &res) */
	template <typename __task_exec_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_lambda_t>()(
											std::declval<aosl_task_act_t>(),
											std::declval<uintptr_t>(),
											std::declval<aosl_refobj_t>(),
											std::declval<uintptr_t &>()
										))>::value, int>::type = 0>
	int exec (const char *f_name, __task_exec_lambda_t&& task)
	{
		return ref_oop ()->exec (f_name, std::move (task));
	}

	/* __task_exec_2args_lambda_t: void (aosl_task_act_t action, uintptr_t opaque) */
	template <typename __task_exec_2args_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_2args_lambda_t>()(
												std::declval<aosl_task_act_t>(),
												std::declval<uintptr_t>()
											))>::value, int>::type = 0>
	int exec (const char *f_name, __task_exec_2args_lambda_t&& task)
	{
		return ref_oop ()->exec (f_name, std::move (task));
	}

	/* __task_exec_1arg_lambda_t: void (aosl_task_act_t action) */
	template <typename __task_exec_1arg_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_1arg_lambda_t>()(
												std::declval<aosl_task_act_t>()
											))>::value, int>::type = 0>
	int exec (const char *f_name, __task_exec_1arg_lambda_t&& task)
	{
		return ref_oop ()->exec (f_name, std::move (task));
	}

	/* __task_exec_1arg_lambda_t: void (aosl_task_act_t action) */
	template <typename __task_exec_1arg_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__task_exec_1arg_lambda_t>()(
											std::declval<aosl_task_act_t>()
										))>::value, int>::type = 0>
	int exclusive_exec (__task_exec_1arg_lambda_t&& task)
	{
		return ref_oop ()->exclusive_exec (std::move (task));
	}

	/* __task_async_lambda_t: void (uintptr_t opaque, aosl_refobj_t tobj, uintptr_t &res) */
	template <typename __task_async_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__task_async_lambda_t>()(
												std::declval<uintptr_t>(),
												std::declval<aosl_refobj_t>(),
												std::declval<uintptr_t &>()
											))>::value, int>::type = 0>
	int async (const char *f_name, __task_async_lambda_t&& task)
	{
		return ref_oop ()->async (f_name, std::move (task));
	}

	/* __task_async_1arg_lambda_t: void (uintptr_t opaque) */
	template <typename __task_async_1arg_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__task_async_1arg_lambda_t>()(
												std::declval<uintptr_t>()
											))>::value, int>::type = 0>
	int async (const char *f_name, __task_async_1arg_lambda_t&& task)
	{
		return ref_oop ()->async (f_name, std::move (task));
	}

	/* __task_async_0arg_lambda_t: void (void) */
	template <typename __task_async_0arg_lambda_t>
	/**
	 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
	 * because the buggy MSVC compiler version 14.25.28610 will report:
	 *  - error C2672: XXX: no matching overloaded function found
	 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
	 * So, we use the return type style SFINAE here instead.
	 * -- Lionfore Hao Apr 15th, 2025
	 **/
	typename std::enable_if<std::is_void<decltype(std::declval<__task_async_0arg_lambda_t>()())>::value, int>::type
	async (const char *f_name, __task_async_0arg_lambda_t&& task)
	{
		return ref_oop ()->async (f_name, std::move (task));
	}
#endif /* C++11 */

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
private:
	aosl_task_class (const aosl_task_class &) = delete;
	aosl_task_class (aosl_task_class &&) = delete;
	aosl_task_class &operator = (const aosl_task_class &) = delete;
	aosl_task_class &operator = (aosl_task_class &&) = delete;
#else
private:
	aosl_task_class (const aosl_task_class &);
	aosl_task_class &operator = (const aosl_task_class &);
#endif /* C++11 */
};


typedef aosl_ref_unique_ptr<aosl_task_class> aosl_task_class_unique_ptr;



#endif /* __AOSL_TASK_CLASS_H__ */