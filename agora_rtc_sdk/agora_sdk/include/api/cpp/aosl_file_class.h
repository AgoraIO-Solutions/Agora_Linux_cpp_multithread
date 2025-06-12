/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 19th, 2020
 * Module:	AOSL file operations for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2020 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_FILE_CPP_H__
#define __AOSL_FILE_CPP_H__


#include <stdlib.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_file.h>
#include <api/aosl_errno.h>

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
#include <memory>
#include <type_traits>
#include <utility>
#endif

class aosl_file_class {
private:
	aosl_fd_t fd;

public:
	aosl_file_class ()
	{
		fd = AOSL_INVALID_FD;
	}

	aosl_fd_t detach ()
	{
		if (!aosl_fd_invalid (fd)) {
			aosl_fd_t _fd = fd;
			fd = AOSL_INVALID_FD;
			return _fd;
		}

		return AOSL_INVALID_FD;
	}

	int open (const char *file, int oflag, ...)
	{
		int mode = 0;

		if (!aosl_fd_invalid (fd)) {
			aosl_file_close (fd);
			fd = AOSL_INVALID_FD;
		}

		if ((oflag & O_CREAT) != 0) {
			va_list arg;
			va_start (arg, oflag);
			mode = va_arg (arg, int);
			va_end (arg);
		}

		fd = aosl_file_open (file, oflag, mode);
		if (aosl_fd_invalid (fd))
			return -1;

		return 0;
	}

	ssize_t size ()
	{
		return aosl_file_size (fd);
	}

	ssize_t read (void *buf, size_t count)
	{
		return aosl_file_read (fd, buf, count);
	}

	ssize_t write (const void *buf, size_t count)
	{
		return aosl_file_write (fd, buf, count);
	}

	off_t lseek (off_t offset, int whence)
	{
		return aosl_file_lseek (fd, offset, whence);
	}

	int aread (size_t count, aosl_file_aread_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_file_aread_args (fd, count, f, argc, args);
		va_end (args);

		return err;
	}

	int aread_args (size_t count, aosl_file_aread_t f, uintptr_t argc, va_list args)
	{
		return aosl_file_aread_args (fd, count, f, argc, args);
	}

	int aread_argv (size_t count, aosl_file_aread_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_file_aread_argv (fd, count, f, argc, argv);
	}

	int awrite (const void *buf, size_t count, aosl_file_awrite_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_file_awrite_args (fd, buf, count, f, argc, args);
		va_end (args);

		return err;
	}

	int awrite_args (const void *buf, size_t count, aosl_file_awrite_t f, uintptr_t argc, va_list args)
	{
		return aosl_file_awrite_args (fd, buf, count, f, argc, args);
	}

	int awrite_argv (const void *buf, size_t count, aosl_file_awrite_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_file_awrite_argv (fd, buf, count, f, argc, argv);
	}

	int alseek (off_t offset, int whence, aosl_file_alseek_t f, uintptr_t argc, ...)
	{
		va_list args;
		int err;

		va_start (args, argc);
		err = aosl_file_alseek_args (fd, offset, whence, f, argc, args);
		va_end (args);

		return err;
	}

	int alseek_args (off_t offset, int whence, aosl_file_alseek_t f, uintptr_t argc, va_list args)
	{
		return aosl_file_alseek_args (fd, offset, whence, f, argc, args);
	}

	int alseek_argv (off_t offset, int whence, aosl_file_alseek_t f, uintptr_t argc, uintptr_t argv [])
	{
		return aosl_file_alseek_argv (fd, offset, whence, f, argc, argv);
	}

	/* C++11 lambda encapsulations */
#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
public:
	/* __aread_lambda_t: void (bool free_only, aosl_fd_t fd, ssize_t ret, void *buf) */
	template <typename __aread_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__aread_lambda_t>()(
			std::declval<bool>(),
			std::declval<aosl_fd_t>(),
			std::declval<ssize_t>(),
			std::declval<void *>()))>::value, int>::type = 0>
	int aread (size_t count, __aread_lambda_t &&lambda)
	{
		if (lambda != nullptr) {
			__aread_lambda_t *lambda_p = new __aread_lambda_t (std::move (lambda));
			int err = aread (count, ____aread_f<typename std::remove_reference<__aread_lambda_t>::type>, 1, lambda_p);
			if (err < 0)
				delete lambda_p;

			return err;
		}

		aosl_errno = EINVAL;
		return -1;
	}

	/* __awrite_lambda_t: void (bool free_only, aosl_fd_t fd, ssize_t ret) */
	template <typename __awrite_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__awrite_lambda_t>()(
			std::declval<bool>(),
			std::declval<aosl_fd_t>(),
			std::declval<ssize_t>()))>::value, int>::type = 0>
	int awrite (const void *buf, size_t count, __awrite_lambda_t &&lambda = nullptr)
	{
		int err;

		if (lambda == nullptr) {
			err = awrite (buf, count, NULL, 0);
		} else {
			__awrite_lambda_t *lambda_p = new __awrite_lambda_t (std::move (lambda));
			err = awrite (buf, count, ____awrite_f<typename std::remove_reference<__awrite_lambda_t>::type>, 1, lambda_p);
			if (err < 0)
				delete lambda_p;
		}

		return err;
	}

	/* __alseek_lambda_t: void (bool free_only, aosl_fd_t fd, off_t ret) */
	template <typename __alseek_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__alseek_lambda_t>()(
			std::declval<bool>(),
			std::declval<aosl_fd_t>(),
			std::declval<off_t>()))>::value, int>::type = 0>
	int alseek (off_t offset, int whence, __alseek_lambda_t &&lambda = nullptr)
	{
		int err;

		if (lambda == nullptr) {
			err = alseek (offset, whence, NULL, 0);
		} else {
			__alseek_lambda_t *lambda_p = new __alseek_lambda_t (std::move (lambda));
			err = alseek (offset, whence, ____alseek_f<typename std::remove_reference<__alseek_lambda_t>::type>, 1, lambda_p);
			if (err < 0)
				delete lambda_p;
		}

		return err;
	}

private:
	/* __aread_lambda_t: void (bool free_only, aosl_fd_t fd, ssize_t ret, void *buf) */
	template <typename __aread_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__aread_lambda_t>()(
			std::declval<bool>(),
			std::declval<aosl_fd_t>(),
			std::declval<ssize_t>(),
			std::declval<void *>()))>::value, int>::type = 0>
	static void ____aread_f (int free_only, aosl_fd_t fd, ssize_t ret, void *buf, uintptr_t argc, uintptr_t argv [])
	{
		__aread_lambda_t *lambda_f = (__aread_lambda_t *)argv [0];
		(*lambda_f) ((bool)(free_only != 0), fd, ret, buf);
		delete lambda_f;
	}

	/* __awrite_lambda_t: void (bool free_only, aosl_fd_t fd, ssize_t ret) */
	template <typename __awrite_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__awrite_lambda_t>()(
			std::declval<bool>(),
			std::declval<aosl_fd_t>(),
			std::declval<ssize_t>()))>::value, int>::type = 0>
	static void ____awrite_f (int free_only, aosl_fd_t fd, ssize_t ret, uintptr_t argc, uintptr_t argv [])
	{
		__awrite_lambda_t *lambda_f = (__awrite_lambda_t *)argv [0];
		(*lambda_f) ((bool)(free_only != 0), fd, ret);
		delete lambda_f;
	}

	/* __alseek_lambda_t: void (bool free_only, aosl_fd_t fd, off_t ret) */
	template <typename __alseek_lambda_t,
		typename std::enable_if<std::is_void<decltype(std::declval<__alseek_lambda_t>()(
			std::declval<bool>(),
			std::declval<aosl_fd_t>(),
			std::declval<off_t>()))>::value, int>::type = 0>
	static void ____alseek_f (int free_only, aosl_fd_t fd, off_t ret, uintptr_t argc, uintptr_t argv [])
	{
		__alseek_lambda_t *lambda_f = (__alseek_lambda_t *)argv [0];
		(*lambda_f) ((bool)(free_only != 0), fd, ret);
		delete lambda_f;
	}
#endif /* C++11 */

public:
	int co_read (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off = 0, uintptr_t d_len = -1ul)
	{
		return aosl_co_file_read (fd, d_ret, d_buf, d_off, d_len);
	}

	int co_write (aosl_data_t d_ret, aosl_data_t d_buf, uintptr_t d_off = 0, uintptr_t d_len = -1ul)
	{
		return aosl_co_file_write (fd, d_ret, d_buf, d_off, d_len);
	}

	int co_file_lseek (aosl_data_t d_ret, off_t offset, int whence)
	{
		return aosl_co_file_lseek (fd, d_ret, offset, whence);
	}

	int close ()
	{
		if (!aosl_fd_invalid (fd)) {
			int err;
			err = aosl_file_close (fd);
			fd = AOSL_INVALID_FD;
			return err;
		}

		return -1;
	}

	virtual ~aosl_file_class ()
	{
		if (!aosl_fd_invalid (fd))
			aosl_file_close (fd);
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
private:
	aosl_file_class (const aosl_file_class &) = delete;
	aosl_file_class (aosl_file_class &&) = delete;
	aosl_file_class &operator = (const aosl_file_class &) = delete;
	aosl_file_class &operator = (aosl_file_class &&) = delete;
#else
private:
	aosl_file_class (const aosl_file_class &);
	aosl_file_class &operator = (const aosl_file_class &);
#endif /* C++11 */
};


#endif /* __AOSL_FILE_CPP_H__ */