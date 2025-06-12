/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 24th, 2024
 * Module:	AOSL Generic FSM C++ API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#if (__cplusplus >= 201103) || (defined (_MSC_VER) && _MSC_VER >= 1800)

#ifndef __AOSL_GFSM_CPP_H__
#define __AOSL_GFSM_CPP_H__


#include <stdlib.h>
#include <api/aosl_defs.h>
#include <api/aosl_types.h>
#include <api/aosl_gfsm.h>

#include <api/cpp/aosl_data_class.h>
#include <api/cpp/aosl_callable_class.h>

#include <new>
#include <type_traits>
#include <utility>
#include <initializer_list>


typedef aosl_callable<int (uint32_t, aosl_data_t)> aosl_gfsm_act_lambda_f;

class aosl_gfsm_class {
private:
	aosl_ref_t gfsm;

public:
	struct evtact {
		int32_t event;
		aosl_gfsm_act_cost_t act_cost;
		int32_t st_next_s; /* state next if success */
		int32_t st_next_f; /* state next if failure */
		aosl_gfsm_act_lambda_f act_f;
	};

	struct state {
		int32_t state;
		uint32_t acts_count;
		evtact *evtacts;
	};

	aosl_gfsm_class ()
	{
		gfsm = AOSL_REF_INVALID;
	}

	aosl_gfsm_class (const char *name, std::initializer_list<state> &&st_table, int32_t st_init)
	{
		gfsm = aosl_gfsm_create (name, this, (aosl_gfsm_state_t *)st_table.begin (), st_table.size (), st_init, sizeof (evtact) - sizeof (aosl_gfsm_evtact_t),
																						____gfsm_evtact_ctor, ____gfsm_evtact_dtor, ____gfsm_evtact_invoke);
		if (aosl_ref_invalid (gfsm))
			abort ();
	}

	int create (const char *name, std::initializer_list<state> &&st_table, int32_t st_init)
	{
		if (!aosl_ref_invalid (gfsm))
			aosl_ref_destroy (gfsm, 1);

		gfsm = aosl_gfsm_create (name, this, (aosl_gfsm_state_t *)st_table.begin (), st_table.size (), st_init, sizeof (evtact) - sizeof (aosl_gfsm_evtact_t),
																						____gfsm_evtact_ctor, ____gfsm_evtact_dtor, ____gfsm_evtact_invoke);
		if (aosl_ref_invalid (gfsm))
			return -1;

		return 0;
	}

	int trigger (uint32_t event, aosl_data_t data = NULL, bool resume = true)
	{
		return aosl_gfsm_trigger (gfsm, event, data, (int)resume);
	}

	void destroy ()
	{
		if (!aosl_ref_invalid (gfsm)) {
			aosl_ref_destroy (gfsm, 1);
			gfsm = AOSL_REF_INVALID;
		}
	}

	virtual ~aosl_gfsm_class ()
	{
		if (!aosl_ref_invalid (gfsm))
			aosl_ref_destroy (gfsm, 1);
	}

private:
	/* the customized event action constructor */
	static int ____gfsm_evtact_ctor (void *dst, void *src)
	{
		aosl_gfsm_act_lambda_f *l_src = (aosl_gfsm_act_lambda_f *)src;
		/* construct the fsm internal object according to the src object */
		new (dst) aosl_gfsm_act_lambda_f (std::move (*l_src));
		return 0;
	}

	/* the customized event action destructor */
	static void ____gfsm_evtact_dtor (void *actarg)
	{
		aosl_gfsm_act_lambda_f *lambda_p = (aosl_gfsm_act_lambda_f *)actarg;
		/* destruct the fsm internal object, and its' memory will be freed inside fsm code. */
		lambda_p->~aosl_gfsm_act_lambda_f ();
	}

	/* the customized event action invoker */
	static int ____gfsm_evtact_invoke (void *actarg, void *fsmarg, uint32_t event, aosl_data_t data)
	{
		aosl_gfsm_act_lambda_f *lambda_f = (aosl_gfsm_act_lambda_f *)actarg;
		return (*lambda_f) (event, data);
	}

private:
	aosl_gfsm_class (const aosl_gfsm_class &) = delete;
	aosl_gfsm_class (aosl_gfsm_class &&) = delete;
	aosl_gfsm_class &operator = (const aosl_gfsm_class &) = delete;
	aosl_gfsm_class &operator = (aosl_gfsm_class &&) = delete;
};


#endif /* __AOSL_GFSM_CPP_H__ */

#endif /* C++11 */