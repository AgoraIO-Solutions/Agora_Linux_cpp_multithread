/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 15th, 2023
 * Module:	AOSL async class for C++11 definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2023 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)

#ifndef __AOSL_ASYNC_CPP_H__
#define __AOSL_ASYNC_CPP_H__


#include <api/aosl_types.h>
#include <api/aosl_async.h>

#ifdef COMPILING_WITH_MPQ_H
#include <api/aosl_mpq.h>
#ifdef COMPILING_WITH_MPQP_H
#include <api/aosl_mpqp.h>
#endif
#endif

#include <api/aosl_ref.h>
#include <api/cpp/aosl_ref_class.h>

#include <memory>
#include <type_traits>
#if (__cplusplus >= 202002) || (defined (_MSVC_LANG) && _MSVC_LANG >= 202002)
#include <coroutine>

struct aosl_yld {
	bool await_ready () { return false; }

	void await_suspend (std::coroutine_handle<> h)
	{
		/**
		 * aosl task yield.
		 * This is very similar with operating system sched_yield, just move the current task
		 * to the tail of run queue.
		 **/
		aosl_ref_class::queue (aosl_mpq_this (), AOSL_REF_INVALID, "yield_resume", [=] { h.resume (); });
	}

	void await_resume () {}
};

struct aosl_co {
	struct promise_type {
		aosl_co get_return_object ()
		{
			return {};
		}

		std::suspend_never initial_suspend ()
		{
			return {};
		}

		std::suspend_never final_suspend () noexcept
		{
			return {};
		}

		aosl_yld yield_value (int i)
		{
			(void)i; /* unused for now. */
			return {};
		}

		void return_void ()
		{
		}

		void unhandled_exception ()
		{
		}
	};
};

class aosl_awt {
private:
	/**
	 * This is the common C++20 coroutine invoking protocol, 3 states totally:
	 * 1. <0: some error occurs, coroutine will not suspend;
	 * 2.  0: async operation issued successfully, coroutine will suspend;
	 * 3. >0: operation complete immediately, coroutine will not suspend;
	 * So, passing the aosl coroutine return value to this object, this is the
	 * only thing needed before using the aosl coroutine for C++20.
	 **/
	const int awt_state;

protected:
	/**
	 * Optional ref object to protect the coroutine executing safely, this ref
	 * object is passed to resume, so once it is destroyed, then the free_only
	 * of this object will be true. If you do not need this mechanism, just
	 * leave it unset.
	 **/
	const aosl_ref_t awt_ref;
	int awt_free_only;

public:
	aosl_awt (int state = 0, aosl_ref_t ref = AOSL_REF_INVALID): awt_state (state), awt_ref (ref)
	{
		awt_free_only = 0;
	}

	int state (void) const
	{
		return awt_state;
	}

	int free_only (void) const
	{
		return awt_free_only;
	}

	bool await_ready ()
	{
		if (awt_state == 0)
			return false;

		return true;
	}

	void await_suspend (std::coroutine_handle<aosl_co::promise_type> h)
	{
		aosl_define_stack (stack);
		aosl_ref_class::resume (stack, awt_ref, "suspend_resume", [=, this] (int free_only) {
			awt_free_only = free_only;
			h.resume ();
		});
	}

	void await_resume ()
	{
	}
};

class aosl_awt_isolated: public aosl_awt {
public:
	void await_suspend (std::coroutine_handle<aosl_co::promise_type> h)
	{
		aosl_ref_class::resume (AOSL_STACK_ISOLATED, awt_ref, "suspend_resume", [=, this] (int free_only) {
			awt_free_only = free_only;
			h.resume ();
		});
	}
};
#endif /* C++20 */


class aosl_async_class {
public:
	/* __local_lambda_t: void (void) */
	template <typename __local_lambda_t>
	/**
	 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
	 * because the buggy MSVC compiler version 14.25.28610 will report:
	 *  - error C2672: XXX: no matching overloaded function found
	 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
	 * So, we use the return type style SFINAE here instead.
	 * -- Lionfore Hao Apr 15th, 2025
	 **/
	static typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()())>::value, int>::type
	block (__local_lambda_t&& task)
	{
		return aosl_async_block (____block_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &task);
	}

	/* __local_lambda_t: void (void) */
	template <typename __local_lambda_t>
	/**
	 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
	 * because the buggy MSVC compiler version 14.25.28610 will report:
	 *  - error C2672: XXX: no matching overloaded function found
	 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
	 * So, we use the return type style SFINAE here instead.
	 * -- Lionfore Hao Apr 15th, 2025
	 **/
	static typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()())>::value, int>::type
	isolated (__local_lambda_t&& task)
	{
		return aosl_isolated_block (____block_f<typename std::remove_reference<__local_lambda_t>::type>, 1, &task);
	}

private:
	/* __local_lambda_t: void (void) */
	template <typename __local_lambda_t>
	/**
	 * Do not use the template parameter with default value style SFINAE for 0 argument lambda case,
	 * because the buggy MSVC compiler version 14.25.28610 will report:
	 *  - error C2672: XXX: no matching overloaded function found
	 *  - error C2783: XXX(YYY): could not deduce template argument for '__formal'
	 * So, we use the return type style SFINAE here instead.
	 * -- Lionfore Hao Apr 15th, 2025
	 **/
	static typename std::enable_if<std::is_void<decltype(std::declval<__local_lambda_t>()())>::value, void>::type
	____block_f (uintptr_t argc, uintptr_t argv [])
	{
		(*(__local_lambda_t *)argv [0]) ();
	}

private:
	aosl_async_class () = delete;
	aosl_async_class (const aosl_async_class &) = delete;
	aosl_async_class (aosl_async_class &&) = delete;
	aosl_async_class &operator = (const aosl_async_class &) = delete;
	aosl_async_class &operator = (aosl_async_class &&) = delete;
	~aosl_async_class () = delete;
};

/**
 * Helper async macro for single line code, example:
 * __aosl_async__ (some_func ());
 **/
#define __aosl_async__(_x_y__) { aosl_async_class::block ([&] { _x_y__; }); }

/**
 * Helper async macros for multiple lines code, example:
 *
 * __aosl_async_begin__
 *	// Add your own multiple lines code here:
 *	aosl_define_stack (stack);
 *
 *	task->async ("<task lambda name>", [=] {
 *		......
 *	});
 *
 *	aosl_ref_class::resume (stack, AOSL_REF_INVALID, "<resume lambda name>", [=](int free_only) {
 *		......
 *	});
 * __aosl_async_end__
 *
 **/
#define __aosl_async_begin__ { aosl_async_class::block ([&] {
#define __aosl_async_end__ }); }

/**
 * Helper async macros for implementing a function with return value, example:
 *
 * __aosl_async_func_def__ (<return type>, <ret var>, func_name (func args))
 * {
 *     <func body>
 * }
 * __aosl_async_func_end__ (<ret var>)
 *
 **/
#define __aosl_async_func_def__(_ret_type_, _ret_var_, _fn_and_args_) \
_ret_type_ _fn_and_args_ \
{ \
	_ret_type_ _ret_var_; \
	aosl_async_class::block ([&]

#define __aosl_async_func_end__(_ret_var_) \
	); \
	return _ret_var_; \
}

/**
 * Helper async macros for implementing a void function, example:
 *
 * __aosl_async_func_def_void__ (func_name (func args))
 * {
 *     <func body>
 * }
 * __aosl_async_func_end_void__ ()
 *
 **/
#define __aosl_async_func_def_void__(_fn_) \
void _fn_ \
{ \
	aosl_async_class::block ([&]

#define __aosl_async_func_end_void__() \
	); \
}

#ifdef __AOSL_MPQ_H__
/**
 * Schedule the coroutine task specifed by func on the mpq specified by qid.
 * Parameters:
 *     qid: the mpq id;
 *     ref: the optional ref id for protecting the coroutine, AOSL_REF_INVALID for none;
 *    func: cotask function with parameters;
 * Return Value:
 *       0: successful;
 *      <0: failed with aosl_errno set;
 **/
#define aosl_cotask_sched_on(qid, ref, func) aosl_ref_class::queue (qid, ref, #func, [=] { func; })

/**
 * Loop the coroutine task asynchronously.
 * Parameters:
 *     ref: the optional ref id for protecting the coroutine, AOSL_REF_INVALID for none;
 *    func: cotask function with parameters;
 * Return Value:
 *       0: successful;
 *      <0: failed with aosl_errno set;
 * Remarks:
 *     This functional macro can only be used in a cotask resume block.
 **/
#define aosl_cotask_loop(ref, func) aosl_cotask_sched_on (aosl_mpq_this (), ref, func)

#ifdef __AOSL_MPQP_H__
/**
 * Schedule the coroutine task specifed by func on an mpq in general pool and consider
 * the load balance.
 * Parameters:
 *     ref: the optional ref id for protecting the coroutine, AOSL_REF_INVALID for none;
 *    func: cotask function with parameters;
 * Return Value:
 *       0: successful;
 *      <0: failed with aosl_errno set;
 **/
#define aosl_cotask_sched(ref, func) aosl_ref_class::queue (aosl_genp (), ref, #func, [=] { func; })
#endif /* __AOSL_MPQP_H__ */
#endif /* __AOSL_MPQ_H__ */


#endif /* __AOSL_ASYNC_CPP_H__ */

#endif /* C++11 */