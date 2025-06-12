/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Aug 14th, 2024
 * Module:	AOSL iovec/iob relatives for C++ definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_IOVEC_CLASS_H__
#define __AOSL_IOVEC_CLASS_H__


#include <stdlib.h>
#include <string.h>

#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_mm.h>
#include <api/aosl_data.h>
#include <api/aosl_iovec.h>


class aosl_iovec_class {
private:
	aosl_biov_t __iovecs [8];
	aosl_biov_t *_iov;
	int _iovcnt;
	bool _attach;

private:
	void __assign_iov (const aosl_biovs_t *iovs, int iovscnt, bool move)
	{
		int total_iovcnt = 0;
		const aosl_biovs_t *iovs_e;
		int i;
		int idx;

		if (iovscnt < 0 || iovscnt > AOSL_IOBUF_MAX)
			abort ();

		for (i = 0; i < iovscnt; i++) {
			iovs_e = &iovs [i];
			if (iovs_e->d_iovcnt > UIO_MAXIOV)
				abort ();

			total_iovcnt += iovs_e->d_iovcnt;
		}

		_iov = __iovecs;
		if (total_iovcnt > (int)(sizeof __iovecs / sizeof __iovecs [0])) {
			_iov = (aosl_biov_t *)aosl_malloc (sizeof (aosl_biov_t) * total_iovcnt);
			if (_iov == NULL)
				abort ();
		}

		idx = 0;
		for (i = 0; i < iovscnt; i++) {
			uintptr_t j;
			iovs_e = &iovs [i];
			for (j = 0; j < iovs_e->d_iovcnt; j++) {
				const aosl_biov_t *src = &iovs_e->d_iov [j];
				aosl_biov_t *dst = &_iov [idx];
				dst->d_buf = src->d_buf;
				if (dst->d_buf != NULL && !move)
					aosl_data_get (dst->d_buf);
				dst->d_off = src->d_off;
				dst->d_len = src->d_len;
				idx++;
			}
		}

		_iovcnt = total_iovcnt;
	}

	void __add_iov (const aosl_biovs_t *iovs, int iovscnt, bool move, bool add_head)
	{
		int add_iovcnt = 0;
		const aosl_biovs_t *iovs_e;
		int i;
		int idx;

		if (iovscnt < 0 || iovscnt > AOSL_IOBUF_MAX)
			abort ();

		for (i = 0; i < iovscnt; i++) {
			iovs_e = &iovs [i];
			if (iovs_e->d_iovcnt > UIO_MAXIOV)
				abort ();

			add_iovcnt += iovs_e->d_iovcnt;
		}

		if (_attach) {
			/* for attach case, we must 'get' the data object after copy */
			for (i = 0; i < _iovcnt; i++) {
				aosl_data_t d_buf = _iov [i].d_buf;
				if (d_buf != NULL)
					aosl_data_get (d_buf);
			}
		}

		idx = add_head ? add_iovcnt : 0;
		if (_iovcnt + add_iovcnt > (int)(sizeof __iovecs / sizeof __iovecs [0])) {
			aosl_biov_t *new_iov = (aosl_biov_t *)aosl_malloc (sizeof (aosl_biov_t) * (_iovcnt + add_iovcnt));
			if (new_iov == NULL)
				abort ();

			memcpy (&new_iov [idx], &_iov [0], sizeof (aosl_biov_t) * _iovcnt);
			if (!_attach && _iov != __iovecs)
				aosl_free (_iov);
			_iov = new_iov;
		} else {
			if (_attach) {
				memcpy (&__iovecs [idx], &_iov [0], sizeof (aosl_biov_t) * _iovcnt);
				_iov = __iovecs;
			} else if (add_head)
				memmove (&_iov [add_iovcnt], &_iov [0], sizeof (aosl_biov_t) * _iovcnt);
		}

		_attach = false;
		idx = add_head ? 0 : _iovcnt;
		for (i = 0; i < iovscnt; i++) {
			uintptr_t j;
			iovs_e = &iovs [i];
			for (j = 0; j < iovs_e->d_iovcnt; j++) {
				const aosl_biov_t *src = &iovs_e->d_iov [j];
				aosl_biov_t *dst = &_iov [idx];
				dst->d_buf = src->d_buf;
				if (dst->d_buf != NULL && !move)
					aosl_data_get (dst->d_buf);
				dst->d_off = src->d_off;
				dst->d_len = src->d_len;
				idx++;
			}
		}

		_iovcnt += add_iovcnt;
	}

	void __release_iov ()
	{
		int i;
		for (i = 0; i < _iovcnt; i++) {
			aosl_biov_t *iov_e = &_iov [i];
			if (iov_e->d_buf != NULL)
				aosl_data_put (iov_e->d_buf);
		}

		if (_iov != __iovecs)
			aosl_free (_iov);
	}

public:
	void reset ()
	{
		_iov = __iovecs;
		_iovcnt = 0;
		_attach = false;
	}

	aosl_iovec_class ()
	{
		reset ();
	}

	void assign_iov (const aosl_biovs_t *iovs, int iovscnt, bool move)
	{
		__assign_iov (iovs, iovscnt, move);
	}

	void copy_iov (const aosl_biovs_t *iovs, int iovscnt)
	{
		__assign_iov (iovs, iovscnt, false);
	}

	void move_iov (const aosl_biovs_t *iovs, int iovscnt)
	{
		__assign_iov (iovs, iovscnt, true);
	}

	aosl_iovec_class (const aosl_biov_t *iov, int iovcnt)
	{
		aosl_biovs_t iovs = {iov, (uintptr_t)iovcnt};
		copy_iov (&iovs, 1);
		_attach = false;
	}

	aosl_iovec_class (const aosl_biovs_t &iovs)
	{
		copy_iov (&iovs, 1);
		_attach = false;
	}

	~aosl_iovec_class ()
	{
		if (!_attach)
			__release_iov ();
	}

	operator aosl_biov_t *() const
	{
		return _iov;
	}

	operator int () const
	{
		return _iovcnt;
	}

	operator aosl_biovs_t () const
	{
		aosl_biovs_t iovs;
		iovs.d_iov = _iov;
		iovs.d_iovcnt = (uintptr_t)_iovcnt;
		return iovs;
	}

	void attach (aosl_biov_t *iov, int iovcnt)
	{
		if (!_attach)
			__release_iov ();
		_iov = iov;
		_iovcnt = iovcnt;
		_attach = true;
	}

	void attach (const aosl_biovs_t &iovs)
	{
		if (!_attach)
			__release_iov ();
		_iov = (aosl_biov_t *)iovs.d_iov;
		_iovcnt = iovs.d_iovcnt;
		_attach = true;
	}

	void detach ()
	{
		reset ();
	}

	aosl_iovec_class (const aosl_iovec_class &src)
	{
		if (this != &src) {
			aosl_biovs_t iovs = {src._iov, (uintptr_t)src._iovcnt};
			copy_iov (&iovs, 1);
		}

		_attach = false;
	}

	aosl_iovec_class &operator = (const aosl_iovec_class &src)
	{
		if (this != &src) {
			aosl_biovs_t iovs = {src._iov, (uintptr_t)src._iovcnt};
			copy_iov (&iovs, 1);
		}

		_attach = false;
		return *this;
	}

	aosl_iovec_class &operator += (const aosl_iovec_class &src)
	{
		if (this != &src) {
			aosl_biovs_t iovs = {src._iov, (uintptr_t)src._iovcnt};
			__add_iov (&iovs, 1, false, false);
		}

		return *this;
	}

	/* append iovecs after current */
	aosl_iovec_class &operator += (const aosl_biovs_t &iovs)
	{
		__add_iov (&iovs, 1, false, false);
		return *this;
	}

	/* prepend iovecs before current */
	void prepend (const aosl_biovs_t &iovs)
	{
		__add_iov (&iovs, 1, false, true);
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	aosl_iovec_class (aosl_iovec_class &&src)
	{
		if (this != &src) {
			if (src._attach || src._iov == src.__iovecs) {
				aosl_biovs_t iovs = {src._iov, (uintptr_t)src._iovcnt};
				move_iov (&iovs, 1);
			} else {
				_iov = src._iov;
				_iovcnt = src._iovcnt;
			}
			src.reset ();
		}
	}

	aosl_iovec_class &operator = (aosl_iovec_class &&src)
	{
		if (this != &src) {
			if (src._attach || src._iov == src.__iovecs) {
				aosl_biovs_t iovs = {src._iov, (uintptr_t)src._iovcnt};
				move_iov (&iovs, 1);
			} else {
				_iov = src._iov;
				_iovcnt = src._iovcnt;
			}
			src.reset ();
		}

		return *this;
	}

	aosl_iovec_class &operator += (aosl_iovec_class &&src)
	{
		if (this != &src) {
			aosl_biovs_t iovs = {src._iov, (uintptr_t)src._iovcnt};
			__add_iov (&iovs, 1, true, false);
			src.reset ();
		}

		return *this;
	}
#endif /* C++11 */
};

static inline aosl_iovec_class operator + (const aosl_biovs_t &iovs1, const aosl_biovs_t &iovs2)
{
	aosl_iovec_class ret;
	aosl_biovs_t iovs [2] = {
		{iovs1.d_iov, iovs1.d_iovcnt},
		{iovs2.d_iov, iovs2.d_iovcnt},
	};
	ret.copy_iov (iovs, 2);
	return ret;
}

static inline aosl_iovec_class operator + (const aosl_iovec_class &iovcls, const aosl_biovs_t &iovs)
{
	aosl_iovec_class ret;
	aosl_biovs_t iovcls_iovs [2] = {
		{(aosl_biov_t *)iovcls, (uintptr_t)iovcls},
		{iovs.d_iov, iovs.d_iovcnt},
	};
	ret.copy_iov (iovcls_iovs, 2);
	return ret;
}

static inline aosl_iovec_class operator + (const aosl_biovs_t &iovs, const aosl_iovec_class &iovcls)
{
	aosl_iovec_class ret;
	aosl_biovs_t iovcls_iovs [2] = {
		{iovs.d_iov, iovs.d_iovcnt},
		{(aosl_biov_t *)iovcls, (uintptr_t)iovcls},
	};
	ret.copy_iov (iovcls_iovs, 2);
	return ret;
}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
static inline aosl_iovec_class operator + (aosl_iovec_class &&iovcls, const aosl_biovs_t &iovs)
{
	aosl_iovec_class ret;
	aosl_biovs_t iovcls_iovs [2] = {
		{(aosl_biov_t *)iovcls, (uintptr_t)iovcls},
		{iovs.d_iov, iovs.d_iovcnt},
	};
	ret.move_iov (iovcls_iovs, 2);
	iovcls.reset ();
	return ret;
}

static inline aosl_iovec_class operator + (const aosl_biovs_t &iovs, aosl_iovec_class &&iovcls)
{
	aosl_iovec_class ret;
	aosl_biovs_t iovcls_iovs [2] = {
		{iovs.d_iov, iovs.d_iovcnt},
		{(aosl_biov_t *)iovcls, (uintptr_t)iovcls},
	};
	ret.move_iov (iovcls_iovs, 2);
	iovcls.reset ();
	return ret;
}
#endif

class aosl_iob_class {
private:
	aosl_iob_t __iobs [4];
	aosl_iovec_class __iovclses [4];
	aosl_iob_t *_iob;
	aosl_iovec_class *_iovcls;
	int _iobcnt;
	bool _attach;

private:
	void __assign_iob (const aosl_iob_t *iob, int iobcnt, bool move)
	{
		int i;

		if (iobcnt < 0 || iobcnt > AOSL_IOBUF_MAX)
			abort ();

		_iob = __iobs;
		_iovcls = __iovclses;
		if (iobcnt > (int)(sizeof __iobs / sizeof __iobs [0])) {
			_iob = (aosl_iob_t *)aosl_malloc (sizeof (aosl_iob_t) * iobcnt);
			if (_iob == NULL)
				abort ();

			_iovcls = new aosl_iovec_class [iobcnt];
			if (_iovcls == NULL)
				abort ();
		}

		for (i = 0; i < iobcnt; i++) {
			aosl_iob_t *dst = &_iob [i];
			const aosl_iob_t *src = &iob [i];
			aosl_biovs_t iovs = {src->d_iov, src->d_iovcnt};
			aosl_iovec_class *dstiovcls = &_iovcls [i];

			dst->d_ret = src->d_ret;
			if (dst->d_ret != NULL && !move)
				aosl_data_get (dst->d_ret);

			dstiovcls->assign_iov (&iovs, 1, move);
			dst->d_iov = (aosl_biov_t *)dstiovcls;
			dst->d_iovcnt = src->d_iovcnt;

			dst->d_extra = src->d_extra;
			if (dst->d_extra != NULL && !move)
				aosl_data_get (dst->d_extra);
		}

		_iobcnt = iobcnt;
	}

	void __release_iob ()
	{
		int i;
		for (i = 0; i < _iobcnt; i++) {
			aosl_iob_t *iob_e = &_iob [i];
			if (iob_e->d_ret != NULL)
				aosl_data_put (iob_e->d_ret);
			if (iob_e->d_extra != NULL)
				aosl_data_put (iob_e->d_extra);
		}

		if (_iob != __iobs) {
			aosl_free (_iob);
			delete [] _iovcls;
		}
	}

public:
	void reset ()
	{
		_iob = __iobs;
		_iovcls = __iovclses;
		_iobcnt = 0;
		_attach = false;
	}

	aosl_iob_class ()
	{
		reset ();
	}

	void copy_iob (const aosl_iob_t *iob, int iobcnt)
	{
		__assign_iob (iob, iobcnt, false);
	}

	void move_iob (const aosl_iob_t *iob, int iobcnt)
	{
		__assign_iob (iob, iobcnt, true);
	}

	aosl_iob_class (aosl_iob_t *iob, int iobcnt)
	{
		copy_iob (iob, iobcnt);
		_attach = false;
	}

	~aosl_iob_class ()
	{
		if (!_attach)
			__release_iob ();
	}

	operator aosl_iob_t *() const
	{
		return _iob;
	}

	operator int () const
	{
		return _iobcnt;
	}

	void attach (aosl_iob_t *iob, int iobcnt)
	{
		if (!_attach)
			__release_iob ();
		_iob = iob;
		_iobcnt = iobcnt;
		_attach = true;
	}

	void detach ()
	{
		reset ();
	}

	aosl_iob_class (const aosl_iob_class &src)
	{
		if (this != &src)
			copy_iob (src._iob, src._iobcnt);

		_attach = false;
	}

	aosl_iob_class &operator = (const aosl_iob_class &src)
	{
		if (this != &src)
			copy_iob (src._iob, src._iobcnt);

		_attach = false;
		return *this;
	}

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)
	aosl_iob_class (aosl_iob_class &&src)
	{
		if (this != &src) {
			if (src._attach || src._iob == src.__iobs) {
				int i;
				move_iob (src._iob, src._iobcnt);
				for (i = 0; i < src._iobcnt; i++)
					src._iovcls [i].reset ();
			} else {
				_iob = src._iob;
				_iovcls = src._iovcls;
				_iobcnt = src._iobcnt;
			}
			src.reset ();
		}
	}

	aosl_iob_class &operator = (aosl_iob_class &&src)
	{
		if (this != &src) {
			if (src._attach || src._iob == src.__iobs) {
				int i;
				move_iob (src._iob, src._iobcnt);
				for (i = 0; i < src._iobcnt; i++)
					src._iovcls [i].reset ();
			} else {
				_iob = src._iob;
				_iovcls = src._iovcls;
				_iobcnt = src._iobcnt;
			}
			src.reset ();
		}

		return *this;
	}
#endif /* C++11 */
};


#endif /* __AOSL_IOVEC_CLASS_H__ */