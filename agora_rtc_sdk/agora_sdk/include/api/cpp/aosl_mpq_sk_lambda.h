/*************************************************************
 * Author:	Lionfore Hao
 * Date	 :	Nov 6th, 2018
 * Module:	AOSL mpq socket C++ 11 lambda implementation
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_MPQ_SK_LAMBDA_H__
#define __AOSL_MPQ_SK_LAMBDA_H__

#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_data.h>
#include <api/aosl_mpq_fd.h>
#include <api/aosl_mpq_net.h>
#include <api/cpp/aosl_mpq_fd_class.h>
#include <api/cpp/aosl_ref_class.h>
#include <api/cpp/aosl_data_class.h>

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <type_traits>
#include <utility>


class aosl_mpq_dgram_sk: public aosl_mpq_fd_class {
public:
	/**
	 * __dgram_data_lambda_t: void (void *data, size_t len, const aosl_sk_addr_t *addr)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __dgram_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_dgram_sk (aosl_fd_t sk, size_t max_pkt_size,
							__dgram_data_lambda_t&& data_cb,
							__event_lambda_t&& event_cb,
							aosl_mpq_t qid = aosl_mpq_this ())
						: aosl_mpq_fd_class (sk)
	{
		__dgram_data_lambda_t *on_data = new __dgram_data_lambda_t (std::move (data_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_data == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_dgram_socket (qid, sk, NULL, max_pkt_size, &__on_data<typename std::remove_reference<__dgram_data_lambda_t>::type>,
																			&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
					&__sk_dtor<typename std::remove_reference<__dgram_data_lambda_t>::type, typename std::remove_reference<__event_lambda_t>::type>,
																4, ref (), ref_magic (), on_data, on_event) < 0)
			abort ();
	}

	/**
	 * __dgram_data_lambda_t: void (void *data, size_t len, const aosl_sk_addr_t *addr)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __dgram_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_dgram_sk (aosl_fd_t sk, const char *name, size_t max_pkt_size,
							__dgram_data_lambda_t&& data_cb,
							__event_lambda_t&& event_cb,
							aosl_mpq_t qid = aosl_mpq_this ())
						: aosl_mpq_fd_class (sk)
	{
		__dgram_data_lambda_t *on_data = new __dgram_data_lambda_t (std::move (data_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_data == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_dgram_socket (qid, sk, name, max_pkt_size, &__on_data<typename std::remove_reference<__dgram_data_lambda_t>::type>,
																			&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
					&__sk_dtor<typename std::remove_reference<__dgram_data_lambda_t>::type, typename std::remove_reference<__event_lambda_t>::type>,
																4, ref (), ref_magic (), on_data, on_event) < 0)
			abort ();
	}

	/**
	 * __dgram_d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len, const aosl_sk_addr_t *addr)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __dgram_d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_dgram_sk (aosl_fd_t sk, size_t max_pkt_size,
							__dgram_d_data_lambda_t&& data_cb,
							__event_lambda_t&& event_cb,
							aosl_mpq_t qid = aosl_mpq_this ())
						: aosl_mpq_fd_class (sk)
	{
		__dgram_d_data_lambda_t *on_d_data = new __dgram_d_data_lambda_t (std::move (data_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_d_data == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_dgram_socket (qid, sk, NULL, max_pkt_size, &__on_d_data<typename std::remove_reference<__dgram_d_data_lambda_t>::type>,
																			&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
					&__sk_d_dtor<typename std::remove_reference<__dgram_d_data_lambda_t>::type, typename std::remove_reference<__event_lambda_t>::type>,
																4, ref (), ref_magic (), on_d_data, on_event) < 0)
			abort ();
	}

	/**
	 * __dgram_d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len, const aosl_sk_addr_t *addr)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __dgram_d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_dgram_sk (aosl_fd_t sk, const char *name, size_t max_pkt_size,
										__dgram_d_data_lambda_t&& data_cb,
										__event_lambda_t&& event_cb,
										aosl_mpq_t qid = aosl_mpq_this ())
									: aosl_mpq_fd_class (sk)
	{
		__dgram_d_data_lambda_t *on_d_data = new __dgram_d_data_lambda_t (std::move (data_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_d_data == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_dgram_socket (qid, sk, name, max_pkt_size, &__on_d_data<typename std::remove_reference<__dgram_d_data_lambda_t>::type>,
																			&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
					&__sk_d_dtor<typename std::remove_reference<__dgram_d_data_lambda_t>::type, typename std::remove_reference<__event_lambda_t>::type>,
																4, ref (), ref_magic (), on_d_data, on_event) < 0)
			abort ();
	}

	virtual ~aosl_mpq_dgram_sk ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
	}

private:
	/**
	 * __dgram_data_lambda_t: void (void *data, size_t len, const aosl_sk_addr_t *addr)
	 **/
	template <typename __dgram_data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0>
	static void __on_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [], const aosl_sk_addr_t *addr)
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__dgram_data_lambda_t *on_data = (__dgram_data_lambda_t *)argv [2];
			(*on_data) (data, len, addr);
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __dgram_d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len, const aosl_sk_addr_t *addr)
	 **/
	template <typename __dgram_d_data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0>
	static void __on_d_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [], const aosl_sk_addr_t *addr)
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__dgram_d_data_lambda_t *on_d_data = (__dgram_d_data_lambda_t *)argv [2];
			aosl_data_buf d_buf;
			d_buf.attach ((aosl_data_t)argv [argc]);
			(*on_d_data) (d_buf, (uintptr_t)((char *)data - (char *)d_buf), len, addr);
			d_buf.detach ();
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __on_event (aosl_fd_t sk, int event, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__event_lambda_t *on_event = (__event_lambda_t *)argv [3];
			(*on_event) (sk, event);
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __dgram_data_lambda_t: void (void *data, size_t len, const aosl_sk_addr_t *addr)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __dgram_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __sk_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__dgram_data_lambda_t *on_data = (__dgram_data_lambda_t *)argv [2];
		__event_lambda_t *on_event = (__event_lambda_t *)argv [3];
		delete on_data;
		delete on_event;
	}

	/**
	 * __dgram_d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len, const aosl_sk_addr_t *addr)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __dgram_d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__dgram_d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>(),
												std::declval<const aosl_sk_addr_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __sk_d_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__dgram_d_data_lambda_t *on_d_data = (__dgram_d_data_lambda_t *)argv [2];
		__event_lambda_t *on_event = (__event_lambda_t *)argv [3];
		delete on_d_data;
		delete on_event;
	}

private:
	aosl_mpq_dgram_sk () = delete;
	aosl_mpq_dgram_sk (const aosl_mpq_dgram_sk &) = delete;
	aosl_mpq_dgram_sk (aosl_mpq_dgram_sk &&) = delete;
	aosl_mpq_dgram_sk &operator = (const aosl_mpq_dgram_sk &) = delete;
	aosl_mpq_dgram_sk &operator = (aosl_mpq_dgram_sk &&) = delete;
};

class aosl_mpq_stream_sk: public aosl_mpq_fd_class {
public:
	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __data_lambda_t: void (void *data, size_t len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_stream_sk (aosl_fd_t sk, const char *name, int enable, size_t max_pkt_size,
												__chk_pkt_lambda_t&& chk_pkt_cb,
													__data_lambda_t&& data_cb,
													__event_lambda_t&& event_cb,
													aosl_mpq_t qid = aosl_mpq_this ())
									: aosl_mpq_fd_class (sk)
	{
		__chk_pkt_lambda_t *on_chk_pkt = new __chk_pkt_lambda_t (std::move (chk_pkt_cb));
		__data_lambda_t *on_data = new __data_lambda_t (std::move (data_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_chk_pkt == NULL || on_data == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_stream_socket (qid, sk, name, enable, max_pkt_size,
							&__on_chk_pkt<typename std::remove_reference<__chk_pkt_lambda_t>::type>,
								&__on_data<typename std::remove_reference<__data_lambda_t>::type>,
							&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
							&__sk_dtor<typename std::remove_reference<__chk_pkt_lambda_t>::type,
									typename std::remove_reference<__data_lambda_t>::type,
									typename std::remove_reference<__event_lambda_t>::type>,
										5, ref (), ref_magic (), on_chk_pkt, on_data, on_event) < 0)
			abort ();
	}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_stream_sk (aosl_fd_t sk, const char *name, int enable, size_t max_pkt_size,
												__chk_pkt_lambda_t&& chk_pkt_cb,
													__d_data_lambda_t&& data_cb,
													__event_lambda_t&& event_cb,
													aosl_mpq_t qid = aosl_mpq_this ())
									: aosl_mpq_fd_class (sk)
	{
		__chk_pkt_lambda_t *on_chk_pkt = new __chk_pkt_lambda_t (std::move (chk_pkt_cb));
		__d_data_lambda_t *on_d_data = new __d_data_lambda_t (std::move (data_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_chk_pkt == NULL || on_d_data == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_stream_socket (qid, sk, name, enable, max_pkt_size,
											&__on_chk_pkt<typename std::remove_reference<__chk_pkt_lambda_t>::type>,
											&__on_d_data<typename std::remove_reference<__d_data_lambda_t>::type>,
												&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
										&__sk_d_dtor<typename std::remove_reference<__chk_pkt_lambda_t>::type,
													typename std::remove_reference<__d_data_lambda_t>::type,
													typename std::remove_reference<__event_lambda_t>::type>,
												5, ref (), ref_magic (), on_chk_pkt, on_d_data, on_event) < 0)
			abort ();
	}

	virtual ~aosl_mpq_stream_sk ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
	}

private:
	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 **/
	template <typename __chk_pkt_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0>
	static ssize_t __on_chk_pkt (const void *data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		ssize_t ret = -1;

		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__chk_pkt_lambda_t *on_chk_pkt = (__chk_pkt_lambda_t *)argv [2];
			ret = (*on_chk_pkt) (data, len);
		}, (aosl_ref_magic_t)argv [1]);

		return ret;
	}

	/**
	 * __data_lambda_t: void (void *data, size_t len)
	 **/
	template <typename __data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0>
	static void __on_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__data_lambda_t *on_data = (__data_lambda_t *)argv [3];
			(*on_data) (data, len);
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 **/
	template <typename __d_data_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0>
	static void __on_d_data (void *data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__d_data_lambda_t *on_d_data = (__d_data_lambda_t *)argv [3];
			aosl_data_buf d_buf;
			d_buf.attach ((aosl_data_t)argv [argc]);
			(*on_d_data) (d_buf, (uintptr_t)((char *)data - (char *)d_buf), len);
			d_buf.detach ();
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __on_event (aosl_fd_t sk, int event, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__event_lambda_t *on_event = (__event_lambda_t *)argv [4];
			(*on_event) (sk, event);
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __data_lambda_t: void (void *data, size_t len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __sk_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__chk_pkt_lambda_t *on_chk_pkt = (__chk_pkt_lambda_t *)argv [2];
		__data_lambda_t *on_data = (__data_lambda_t *)argv [3];
		__event_lambda_t *on_event = (__event_lambda_t *)argv [4];
		delete on_chk_pkt;
		delete on_data;
		delete on_event;
	}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __sk_d_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__chk_pkt_lambda_t *on_chk_pkt = (__chk_pkt_lambda_t *)argv [2];
		__d_data_lambda_t *on_d_data = (__d_data_lambda_t *)argv [3];
		__event_lambda_t *on_event = (__event_lambda_t *)argv [4];
		delete on_chk_pkt;
		delete on_d_data;
		delete on_event;
	}

private:
	aosl_mpq_stream_sk () = delete;
	aosl_mpq_stream_sk (const aosl_mpq_stream_sk &) = delete;
	aosl_mpq_stream_sk (aosl_mpq_stream_sk &&) = delete;
	aosl_mpq_stream_sk &operator = (const aosl_mpq_stream_sk &) = delete;
	aosl_mpq_stream_sk &operator = (aosl_mpq_stream_sk &&) = delete;
};

class aosl_mpq_connect_stream_sk: public aosl_mpq_stream_sk {
public:
	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __data_lambda_t: void (void *data, size_t len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_connect_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							__chk_pkt_lambda_t&& chk_pkt_cb,
								__data_lambda_t&& data_cb,
								__event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, NULL, 0/* !enable */,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __data_lambda_t: void (void *data, size_t len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_connect_stream_sk (aosl_fd_t sk, const char *name, size_t max_pkt_size,
											__chk_pkt_lambda_t&& chk_pkt_cb,
												__data_lambda_t&& data_cb,
												__event_lambda_t&& event_cb,
												aosl_mpq_t qid = aosl_mpq_this ())
												: aosl_mpq_stream_sk (sk, name, 0/* !enable */,
												max_pkt_size,
												std::move (chk_pkt_cb),
												std::move (data_cb),
												std::move (event_cb), qid) {}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_connect_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							__chk_pkt_lambda_t&& chk_pkt_cb,
								__d_data_lambda_t&& data_cb,
								__event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, NULL, 0/* !enable */,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_connect_stream_sk (aosl_fd_t sk, const char *name, size_t max_pkt_size,
											__chk_pkt_lambda_t&& chk_pkt_cb,
												__d_data_lambda_t&& data_cb,
												__event_lambda_t&& event_cb,
												aosl_mpq_t qid = aosl_mpq_this ())
												: aosl_mpq_stream_sk (sk, name, 0/* !enable */,
												max_pkt_size,
												std::move (chk_pkt_cb),
												std::move (data_cb),
												std::move (event_cb), qid) {}

	int connect (const struct sockaddr *dest_addr, socklen_t addrlen, int timeo)
	{
		return aosl_mpq_connect (get_fd (), dest_addr, addrlen, timeo);
	}

private:
	aosl_mpq_connect_stream_sk () = delete;
	aosl_mpq_connect_stream_sk (const aosl_mpq_connect_stream_sk &) = delete;
	aosl_mpq_connect_stream_sk (aosl_mpq_connect_stream_sk &&) = delete;
	aosl_mpq_connect_stream_sk &operator = (const aosl_mpq_connect_stream_sk &) = delete;
	aosl_mpq_connect_stream_sk &operator = (aosl_mpq_connect_stream_sk &&) = delete;
};

class aosl_mpq_accepted_stream_sk: public aosl_mpq_stream_sk {
public:
	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __data_lambda_t: void (void *data, size_t len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_accepted_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							__chk_pkt_lambda_t&& chk_pkt_cb,
								__data_lambda_t&& data_cb,
								__event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, NULL, 1/*enable*/,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __data_lambda_t: void (void *data, size_t len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__data_lambda_t>()(
												std::declval<void *>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_accepted_stream_sk (aosl_fd_t sk, const char *name, size_t max_pkt_size,
											__chk_pkt_lambda_t&& chk_pkt_cb,
												__data_lambda_t&& data_cb,
												__event_lambda_t&& event_cb,
												aosl_mpq_t qid = aosl_mpq_this ())
												: aosl_mpq_stream_sk (sk, name, 1/*enable*/,
												max_pkt_size,
												std::move (chk_pkt_cb),
												std::move (data_cb),
												std::move (event_cb), qid) {}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_accepted_stream_sk (aosl_fd_t sk, size_t max_pkt_size,
							__chk_pkt_lambda_t&& chk_pkt_cb,
								__d_data_lambda_t&& data_cb,
								__event_lambda_t&& event_cb,
								aosl_mpq_t qid = aosl_mpq_this ())
								: aosl_mpq_stream_sk (sk, NULL, 1/*enable*/,
								  max_pkt_size,
								  std::move (chk_pkt_cb),
								  std::move (data_cb),
								  std::move (event_cb), qid) {}

	/**
	 * __chk_pkt_lambda_t: ssize_t (const void *data, size_t len)
	 * __d_data_lambda_t: void (const aosl_data_buf &d_data, uintptr_t d_off, size_t d_len)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __chk_pkt_lambda_t, typename __d_data_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_same<decltype(std::declval<__chk_pkt_lambda_t>()(
												std::declval<const void *>(),
												std::declval<size_t>()
											)), ssize_t>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__d_data_lambda_t>()(
												std::declval<const aosl_data_buf &>(),
												std::declval<uintptr_t>(),
												std::declval<size_t>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_accepted_stream_sk (aosl_fd_t sk, const char *name, size_t max_pkt_size,
											__chk_pkt_lambda_t&& chk_pkt_cb,
												__d_data_lambda_t&& data_cb,
												__event_lambda_t&& event_cb,
												aosl_mpq_t qid = aosl_mpq_this ())
												: aosl_mpq_stream_sk (sk, name, 1/*enable*/,
												max_pkt_size,
												std::move (chk_pkt_cb),
												std::move (data_cb),
												std::move (event_cb), qid) {}

private:
	aosl_mpq_accepted_stream_sk () = delete;
	aosl_mpq_accepted_stream_sk (const aosl_mpq_accepted_stream_sk &) = delete;
	aosl_mpq_accepted_stream_sk (aosl_mpq_accepted_stream_sk &&) = delete;
	aosl_mpq_accepted_stream_sk &operator = (const aosl_mpq_accepted_stream_sk &) = delete;
	aosl_mpq_accepted_stream_sk &operator = (aosl_mpq_accepted_stream_sk &&) = delete;
};

class aosl_mpq_listen_sk: public aosl_mpq_fd_class {
public:
	/**
	 * __accept_lambda_t: void (const aosl_accept_data_t *accept_data)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __accept_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__accept_lambda_t>()(
												std::declval<const aosl_accept_data_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_listen_sk (aosl_fd_t sk, int backlog,
		__accept_lambda_t&& accept_cb,
			__event_lambda_t&& event_cb,
			aosl_mpq_t qid = aosl_mpq_this ())
					: aosl_mpq_fd_class (sk)
	{
		__accept_lambda_t *on_accept = new __accept_lambda_t (std::move (accept_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_accept == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_listen_socket (qid, sk, NULL, backlog, &__on_accept<typename std::remove_reference<__accept_lambda_t>::type>,
																		&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
				&__sk_dtor<typename std::remove_reference<__accept_lambda_t>::type, typename std::remove_reference<__event_lambda_t>::type>,
																		4, ref (), ref_magic (), on_accept, on_event) < 0)
			abort ();
	}

	/**
	 * __accept_lambda_t: void (const aosl_accept_data_t *accept_data)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __accept_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__accept_lambda_t>()(
												std::declval<const aosl_accept_data_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	aosl_mpq_listen_sk (aosl_fd_t sk, const char *name, int backlog,
		__accept_lambda_t&& accept_cb,
			__event_lambda_t&& event_cb,
			aosl_mpq_t qid = aosl_mpq_this ())
					: aosl_mpq_fd_class (sk)
	{
		__accept_lambda_t *on_accept = new __accept_lambda_t (std::move (accept_cb));
		__event_lambda_t *on_event = new __event_lambda_t (std::move (event_cb));
		if (on_accept == NULL || on_event == NULL)
			abort ();

		if (aosl_mpq_add_named_listen_socket (qid, sk, name, backlog, &__on_accept<typename std::remove_reference<__accept_lambda_t>::type>,
																		&__on_event<typename std::remove_reference<__event_lambda_t>::type>,
				&__sk_dtor<typename std::remove_reference<__accept_lambda_t>::type, typename std::remove_reference<__event_lambda_t>::type>,
																		4, ref (), ref_magic (), on_accept, on_event) < 0)
			abort ();
	}

	virtual ~aosl_mpq_listen_sk ()
	{
		/**
		 * Please make sure this is the first action in the destructor of
		 * each final derived class of this class and its' derivatives.
		 **/
		ref_destroy ();
	}

private:
	/**
	 * __accept_lambda_t: void (const aosl_accept_data_t *accept_data)
	 **/
	template <typename __accept_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__accept_lambda_t>()(
												std::declval<const aosl_accept_data_t *>()
											))>::value, int>::type = 0>
	static void __on_accept (aosl_accept_data_t *accept_data, size_t len, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__accept_lambda_t *on_accept = (__accept_lambda_t *)argv [2];
			(*on_accept) (accept_data);
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __on_event (aosl_fd_t sk, int event, uintptr_t argc, uintptr_t argv [])
	{
		aosl_ref_class::hold ((aosl_ref_t)argv [0], [&] (void *arg) {
			__event_lambda_t *on_event = (__event_lambda_t *)argv [3];
			(*on_event) (sk, event);
		}, (aosl_ref_magic_t)argv [1]);
	}

	/**
	 * __accept_lambda_t: void (const aosl_accept_data_t *accept_data)
	 * __event_lambda_t: void (aosl_fd_t sk, int event)
	 **/
	template <typename __accept_lambda_t, typename __event_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__accept_lambda_t>()(
												std::declval<const aosl_accept_data_t *>()
											))>::value, int>::type = 0,
		typename std::enable_if<std::is_void<decltype(std::declval<__event_lambda_t>()(
												std::declval<aosl_fd_t>(),
												std::declval<int>()
											))>::value, int>::type = 0>
	static void __sk_dtor (uintptr_t argc, uintptr_t argv [])
	{
		__accept_lambda_t *on_accept = (__accept_lambda_t *)argv [2];
		__event_lambda_t *on_event = (__event_lambda_t *)argv [3];
		delete on_accept;
		delete on_event;
	}

private:
	aosl_mpq_listen_sk () = delete;
	aosl_mpq_listen_sk (const aosl_mpq_listen_sk &) = delete;
	aosl_mpq_listen_sk (aosl_mpq_listen_sk &&) = delete;
	aosl_mpq_listen_sk &operator = (const aosl_mpq_listen_sk &) = delete;
	aosl_mpq_listen_sk &operator = (aosl_mpq_listen_sk &&) = delete;
};
#endif /* C++11 */

class aosl_co_sk: public aosl_mpq_fd_class {
public:
	aosl_co_sk (int domain, int type, int protocol): aosl_mpq_fd_class (aosl_socket (domain, type, protocol))
	{
	}

	aosl_co_sk (aosl_fd_t sk): aosl_mpq_fd_class (sk)
	{
	}

public:
	int listen (int backlog)
	{
		return aosl_listen (get_fd (), backlog);
	}

	int co_accept (aosl_data_t d_accept)
	{
		return aosl_co_accept (get_fd (), d_accept);
	}

	int co_recv (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off = 0, uintptr_t d_len = -1ul, aosl_data_t d_extra = NULL)
	{
		return aosl_co_recv (get_fd (), d_ret, d_buf, d_off, d_len, d_extra);
	}

	int co_recvv (aosl_data_t d_ret, const aosl_biov_t *d_iov, int d_iovcnt, aosl_data_t d_extra = NULL)
	{
		return aosl_co_recvv (get_fd (), d_ret, d_iov, d_iovcnt, d_extra);
	}

	int co_mrecv (aosl_data_t d_ret, const aosl_iob_t *iob, int iobcnt)
	{
		return aosl_co_mrecv (get_fd (), d_ret, iob, iobcnt);
	}

	int co_connect (aosl_data_t d_ret, const struct sockaddr *dest_addr, socklen_t addrlen, int timeo)
	{
		return aosl_co_connect (get_fd (), d_ret, dest_addr, addrlen, timeo);
	}

	int co_send (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off = 0, uintptr_t d_len = -1ul, aosl_data_t d_extra = NULL)
	{
		return aosl_co_send (get_fd (), d_ret, d_buf, d_off, d_len, d_extra);
	}

	int co_sendv (aosl_data_t d_ret, const aosl_biov_t *d_iov, int d_iovcnt, aosl_data_t d_extra = NULL)
	{
		return aosl_co_sendv (get_fd (), d_ret, d_iov, d_iovcnt, d_extra);
	}

	int co_msend (aosl_data_t d_ret, const aosl_iob_t *iob, int iobcnt)
	{
		return aosl_co_msend (get_fd (), d_ret, iob, iobcnt);
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
private:
	aosl_co_sk () = delete;
	aosl_co_sk (const aosl_co_sk &) = delete;
	aosl_co_sk (aosl_co_sk &&) = delete;
	aosl_co_sk &operator = (const aosl_co_sk &) = delete;
	aosl_co_sk &operator = (aosl_co_sk &&) = delete;
#else
private:
	aosl_co_sk ();
	aosl_co_sk (const aosl_co_sk &);
	aosl_co_sk &operator = (const aosl_co_sk &);
#endif /* C++11 */
};


#endif /* __AOSL_MPQ_SK_LAMBDA_H__ */