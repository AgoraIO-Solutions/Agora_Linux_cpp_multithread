/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Apr 12th, 2025
 * Module:	AOSL Generic Callable class C++ API file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2025 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)

#ifndef __AOSL_CALLABLE_CPP_H__
#define __AOSL_CALLABLE_CPP_H__


#include <stdlib.h>
#include <api/aosl_defs.h>
#include <api/aosl_types.h>
#include <api/aosl_mm.h>

#include <new>
#include <type_traits>
#include <utility>


template<typename T> class aosl_callable; /* main template, not defined */

/**
 * General callable class exported by AOSL.
 *
 * This class is more simple than std::function, and using this class will
 * save more code size.
 *
 * The objects of this class are movable but not copyable.
 *
 * @tparam __ret_type_ return type
 * @tparam ...__arg_types_ argument types
 *
 **/
template<typename __ret_type_, typename ...__arg_types_>
class aosl_callable<__ret_type_ (__arg_types_...)> {
private:
	using __g_c_fn_ = __ret_type_ (*) (__arg_types_...);

	void *__obj_lbuf [3]; /* 3 pointers size, no need to dynamic alloc for most cases */
	void *__obj_;
	union {
		struct {
			__ret_type_ (*__invoke_) (void *__obj_, __arg_types_...);
			void (*__move_) (void *dst__obj_, void *src__obj_);
			void (*__dtor_) (void *__obj_);
		};

		__g_c_fn_ __gf_;
	};

	template <typename __some_callable_t_, typename __ret_t_ = __ret_type_>
	static typename std::enable_if<!std::is_void<__ret_t_>::value, __ret_t_>::type __invoke_fobj (void *__obj_, __arg_types_... args)
	{
		__some_callable_t_ *f_obj_p = (__some_callable_t_ *)__obj_;
		return (*f_obj_p) (std::forward<__arg_types_> (args)...);
	}

	template <typename __some_callable_t_, typename __ret_t_ = __ret_type_>
	static typename std::enable_if<std::is_void<__ret_t_>::value, void>::type __invoke_fobj (void *__obj_, __arg_types_... args)
	{
		__some_callable_t_ *f_obj_p = (__some_callable_t_ *)__obj_;
		(*f_obj_p) (std::forward<__arg_types_> (args)...);
	}

	template <typename __some_callable_t_>
	static void __move_fobj (void *dst__obj_, void *src__obj_)
	{
		new (dst__obj_) __some_callable_t_ (std::move (*(__some_callable_t_ *)src__obj_));
	}

	template <typename __some_callable_t_>
	static void __obj_dtor (void *__obj_)
	{
		((__some_callable_t_ *)__obj_)->~__some_callable_t_ ();
	}
public:
	aosl_callable (__g_c_fn_ gf)
	{
		__obj_ = NULL;
		__gf_ = gf;
	}

	template <typename __some_callable_t_>
	aosl_callable (__some_callable_t_ &&f)
	{
		if (sizeof (__some_callable_t_) <= sizeof (__obj_lbuf)) {
			/* static buffer for small callable objects */
			__obj_ = __obj_lbuf;
		} else {
			/* dynamic memory for large callable objects */
			__obj_ = aosl_malloc (sizeof (__some_callable_t_));
			if (__obj_ == NULL)
				abort ();
		}

		new (__obj_) __some_callable_t_ (std::move (f));
		__invoke_ = __invoke_fobj<typename std::remove_reference<__some_callable_t_>::type>;
		__move_ = __move_fobj<typename std::remove_reference<__some_callable_t_>::type>;
		__dtor_ = __obj_dtor<typename std::remove_reference<__some_callable_t_>::type>;
	}

	~aosl_callable ()
	{
		if (__obj_ != NULL) {
			__dtor_ (__obj_);
			if (__obj_ != __obj_lbuf)
				aosl_free (__obj_);
		}
	}

	aosl_callable &operator = (__g_c_fn_ gf)
	{
		if (__obj_ != NULL)
			this->~aosl_callable ();

		__obj_ = NULL;
		__gf_ = gf;
		return *this;
	}

	template <typename __some_callable_t_>
	aosl_callable &operator = (__some_callable_t_ &&f)
	{
		if (__obj_ != NULL)
			this->~aosl_callable ();

		new (this) aosl_callable (std::move (f));
		return *this;
	}

	template <typename __ret_t_ = __ret_type_>
	typename std::enable_if<!std::is_void<__ret_t_>::value, __ret_t_>::type operator () (__arg_types_... args)
	{
		if (__obj_ != NULL)
			return __invoke_ (__obj_, std::forward<__arg_types_> (args)...);

		return __gf_ (std::forward<__arg_types_> (args)...);
	}

	template <typename __ret_t_ = __ret_type_>
	typename std::enable_if<std::is_void<__ret_t_>::value, void>::type operator () (__arg_types_... args)
	{
		if (__obj_ != NULL) {
			__invoke_ (__obj_, std::forward<__arg_types_> (args)...);
		} else {
			__gf_ (std::forward<__arg_types_> (args)...);
		}
	}

	aosl_callable (aosl_callable &&src)
	{
		if (this != &src) {
			if (src.__obj_ != NULL) {
				__invoke_ = src.__invoke_;
				__move_ = src.__move_;
				__dtor_ = src.__dtor_;
				if (src.__obj_ == src.__obj_lbuf) {
					__obj_ = __obj_lbuf;
					__move_ (__obj_, src.__obj_);
				} else {
					__obj_ = src.__obj_;
				}
				src.__obj_ = NULL;
			} else {
				__obj_ = NULL;
				__gf_ = src.__gf_;
				src.__gf_ = NULL;
			}
		}
	}

	aosl_callable &operator = (aosl_callable &&src)
	{
		if (this != &src) {
			this->~aosl_callable ();
			new (this) aosl_callable (std::move (src));
		}

		return *this;
	}

	/* movable but not copyable */
	aosl_callable (const aosl_callable &src) = delete;
	aosl_callable &operator = (const aosl_callable &src) = delete;
};


#endif /* __AOSL_CALLABLE_CPP_H__ */

#endif /* C++11 */