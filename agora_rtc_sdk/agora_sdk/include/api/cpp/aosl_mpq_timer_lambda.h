/*************************************************************
 * Author:	Lionfore Hao
 * Date	 :	Nov 6th, 2018
 * Module:	AOSL mpq timer C++ 11 lambda implementation
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_TIMER_LAMBDA_H__
#define __AOSL_MPQ_TIMER_LAMBDA_H__

#include <stdlib.h>
#include <api/aosl_types.h>
#include <api/aosl_time.h>
#include <api/aosl_mpq_timer.h>

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <type_traits>
#include <utility>

class aosl_mpq_timer_base {
public:
	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	aosl_mpq_timer_base (const char *t_name, __timer_lambda_t&& timer_cb, uintptr_t interval, aosl_ts_t expire_ms,
									aosl_mpq_t q = AOSL_MPQ_INVALID, bool start = true)
	{
		if (aosl_mpq_invalid (q)) {
			q = aosl_mpq_this ();
			if (aosl_mpq_invalid (q))
				abort ();
		}

		__timer_lambda_t *on_timer = new __timer_lambda_t (std::move (timer_cb));
		if (on_timer == NULL)
			abort ();

		if (expire_ms != 0) {
			timer = aosl_mpq_set_named_oneshot_timer (q, t_name, expire_ms, __on_timer<typename std::remove_reference<__timer_lambda_t>::type>,
															__timer_dtor<typename std::remove_reference<__timer_lambda_t>::type>, 1, on_timer);
		} else {
			if (interval == AOSL_INVALID_TIMER_INTERVAL) {
				timer = aosl_mpq_create_named_oneshot_timer (q, t_name, __on_timer<typename std::remove_reference<__timer_lambda_t>::type>,
														__timer_dtor<typename std::remove_reference<__timer_lambda_t>::type>, 1, on_timer);
			} else {
				if (start) {
					timer = aosl_mpq_set_named_timer (q, t_name, interval, __on_timer<typename std::remove_reference<__timer_lambda_t>::type>,
															__timer_dtor<typename std::remove_reference<__timer_lambda_t>::type>, 1, on_timer);
				} else {
					timer = aosl_mpq_create_named_timer (q, t_name, interval, __on_timer<typename std::remove_reference<__timer_lambda_t>::type>,
															__timer_dtor<typename std::remove_reference<__timer_lambda_t>::type>, 1, on_timer);
				}
			}
		}

		if (aosl_mpq_timer_invalid (timer))
			abort ();
	}

	~aosl_mpq_timer_base ()
	{
		if (aosl_mpq_kill_timer (timer) < 0)
			abort ();
	}

	aosl_timer_t timer_id (void)
	{
		return timer;
	}

	int interval (uintptr_t *interval_p)
	{
		return aosl_mpq_timer_interval (timer, interval_p);
	}

	int active (bool *active_p)
	{
		int active;
		int err = aosl_mpq_timer_active (timer, &active);
		if (err == 0 && active_p != NULL)
			*active_p = (bool)(active != 0);

		return err;
	}

	void cancel ()
	{
		aosl_mpq_cancel_timer (timer);
	}

private:
	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	static void __on_timer (aosl_timer_t timer, const aosl_ts_t *now_p, uintptr_t argc, uintptr_t argv [])
	{
		__timer_lambda_t *on_timer = (__timer_lambda_t *)argv [0];
		(*on_timer) (*now_p);
	}

	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	static void __timer_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__timer_lambda_t *on_timer = (__timer_lambda_t *)argv [0];
		delete on_timer;
	}

	aosl_timer_t timer;

private:
	aosl_mpq_timer_base () = delete;
	aosl_mpq_timer_base (const aosl_mpq_timer_base &) = delete;
	aosl_mpq_timer_base (aosl_mpq_timer_base &&) = delete;
	aosl_mpq_timer_base &operator = (const aosl_mpq_timer_base &) = delete;
	aosl_mpq_timer_base &operator = (aosl_mpq_timer_base &&) = delete;
};


class aosl_mpq_timer: public aosl_mpq_timer_base {
public:
	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	aosl_mpq_timer (__timer_lambda_t&& timer_cb, uintptr_t interval, aosl_mpq_t q = AOSL_MPQ_INVALID, bool start = true)
				: aosl_mpq_timer_base (NULL, std::move (timer_cb), interval, 0, q, start) {}

	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	aosl_mpq_timer (const char *t_name, __timer_lambda_t&& timer_cb, uintptr_t interval, aosl_mpq_t q = AOSL_MPQ_INVALID, bool start = true)
				: aosl_mpq_timer_base (t_name, std::move (timer_cb), interval, 0, q, start) {}

	int resched (uintptr_t interval = AOSL_INVALID_TIMER_INTERVAL)
	{
		return aosl_mpq_resched_timer (timer_id (), interval);
	}

private:
	aosl_mpq_timer () = delete;
	aosl_mpq_timer (const aosl_mpq_timer &) = delete;
	aosl_mpq_timer (aosl_mpq_timer &&) = delete;
	aosl_mpq_timer &operator = (const aosl_mpq_timer &) = delete;
	aosl_mpq_timer &operator = (aosl_mpq_timer &&) = delete;
};

class aosl_mpq_oneshot_timer: public aosl_mpq_timer_base {
public:
	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	aosl_mpq_oneshot_timer (__timer_lambda_t&& timer_cb, aosl_ts_t expire_ms = 0, aosl_mpq_t q = AOSL_MPQ_INVALID)
				: aosl_mpq_timer_base (NULL, std::move (timer_cb), AOSL_INVALID_TIMER_INTERVAL, expire_ms, q) {}

	/* __timer_lambda_t: void (const aosl_ts_t &now) */
	template <typename __timer_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__timer_lambda_t>()(
			std::declval<const aosl_ts_t &>()))>::value, int>::type = 0>
	aosl_mpq_oneshot_timer (const char *t_name, __timer_lambda_t&& timer_cb, aosl_ts_t expire_ms = 0, aosl_mpq_t q = AOSL_MPQ_INVALID)
				: aosl_mpq_timer_base (t_name, std::move (timer_cb), AOSL_INVALID_TIMER_INTERVAL, expire_ms, q) {}

	int sched (aosl_ts_t expire_ms)
	{
		return aosl_mpq_resched_oneshot_timer (timer_id (), expire_ms);
	}

private:
	aosl_mpq_oneshot_timer () = delete;
	aosl_mpq_oneshot_timer (const aosl_mpq_oneshot_timer &) = delete;
	aosl_mpq_oneshot_timer (aosl_mpq_oneshot_timer &&) = delete;
	aosl_mpq_oneshot_timer &operator = (const aosl_mpq_oneshot_timer &) = delete;
	aosl_mpq_oneshot_timer &operator = (aosl_mpq_oneshot_timer &&) = delete;
};
#endif /* C++11 */


#endif /* __AOSL_MPQ_TIMER_LAMBDA_H__ */