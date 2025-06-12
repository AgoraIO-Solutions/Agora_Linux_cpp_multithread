/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Nov 24th, 2024
 * Module:	AOSL Generic FSM API definition file
 *
 *
 * This is a part of the Advanced Operating System Layer.
 * Copyright (C) 2018 ~ 2024 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AOSL_GFSM_H__
#define __AOSL_GFSM_H__


#include <api/aosl_types.h>
#include <api/aosl_defs.h>
#include <api/aosl_ref.h>
#include <api/aosl_data.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * The gfsm event action callback function.
 * Parameters:
 *  fsmarg: the argument passed when creating the gfsm;
 *   event: the event passed by trigger function, useless for most cases;
 *    data: the data object passed by trigger function;
 * Return value:
 *      <0: indicate some error occurs;
 *     >=0: successful;
 **/
typedef int (*aosl_gfsm_act_f) (void *fsmarg, uint32_t event, aosl_data_t data);

/* the fsm event action cost type */
typedef enum {
	aosl_gfsm_act_cost_n = 0, /* no cost */
	aosl_gfsm_act_cost_s = 1, /* small cost */
	aosl_gfsm_act_cost_l = 2, /* large cost */
} aosl_gfsm_act_cost_t;

/**
 * The fsm event action common struct, you can customize
 * this struct according to the requirement, but please
 * be informed that all the members before act_f must
 * keep identical with this struct.
 **/
typedef struct {
	uint32_t event;
	aosl_gfsm_act_cost_t act_cost;
	int32_t st_next_s; /* state next if success */
	int32_t st_next_f; /* state next if failure */
	aosl_gfsm_act_f act_f;
} aosl_gfsm_evtact_t;

/* the fsm state definition struct */
typedef struct {
	int32_t state;
	uint32_t acts_count;
	aosl_gfsm_evtact_t *evtacts;
} aosl_gfsm_state_t;

/**
 * The gfsm customized event action constructor callback function.
 * Parameters:
 *     dst: the destination address internal for constructing the customized event action object,
 *          it is &aosl_gfsm_evtact_t.act_f of the fsm internal object;
 *     src: the source address for constructing the customized event action object, it has similar
 *          semantics in the customized event action structure with &aosl_gfsm_evtact_t.act_f;
 * Return value:
 *      <0: indicate some error occurs;
 *     >=0: successful;
 * Remarks:
 *     Construct the customized event action object according to the source in this callback
 *     function, such as invoking the C++ constructor etc for C++ API.
 **/
typedef int (*aosl_gfsm_evtact_ctor_t) (void *dst, void *src);

/**
 * The gfsm customized event action destructor callback function.
 * Parameters:
 *     actarg: the address of fsm internal customized event action object,
 *             it is &aosl_gfsm_evtact_t.act_f of the fsm internal object;
 * Return value:
 *     N/A.
 * Remarks:
 *     Destruct the customized event action object in this callback function,
 *     such as invoking the C++ destructor etc for C++ API.
 **/
typedef void (*aosl_gfsm_evtact_dtor_t) (void *actarg);

/**
 * The gfsm customized event action invoker callback function.
 * Parameters:
 *     actarg: the address of fsm internal customized event action object,
 *             it is &aosl_gfsm_evtact_t.act_f of the fsm internal object;
 *     fsmarg: the argument passed when creating the gfsm;
 *      event: the event passed by trigger function, useless for most cases;
 *       data: the data object passed by trigger function;
 * Return value:
 *      <0: indicate some error occurs;
 *     >=0: successful;
 **/
typedef int (*aosl_gfsm_evtact_invoke_t) (void *actarg, void *fsmarg, uint32_t event, aosl_data_t data);

/**
 * Create a generic finite state machine object.
 * Parameter:
 *          name: the generic fsm object name;
 *        states: the fsm state definition table;
 *      st_count: the fsm state count of definition table;
 *       st_init: the initial state of the fsm;
 *   evtact_diff: the actual fsm event action data structure size diff value in bytes with sizeof (aosl_gfsm_evtact_t),
 *                so please pass in 0 for the cases of just using aosl_gfsm_evtact_t;
 *           ...: the potential event action constructor, destructor and invoking method if evtact_diff > 0;
 * Return value:
 *   non-NULL: create success, returns the generic fsm object;
 *       NULL: failed with aosl_errno set;
 * Remarks:
 *     The state specified by st_init must exist in the state definition table, otherwise this function would fail.
 *     If you want to use self defined event action rather than the common aosl_gfsm_evtact_t structure, please be
 *     informed the following rules:
 *     1. the first 4 members must keep identical with aosl_gfsm_evtact_t structure;
 *     2. must provide constructor, destructor and invoking function passed in through the ... parameters, and use
 *        NULL for no need these functions;
 **/
extern __aosl_api__ aosl_ref_t aosl_gfsm_create (const char *name, void *arg, aosl_gfsm_state_t *states, size_t st_count, int32_t st_init, size_t evtact_diff,
									.../*aosl_gfsm_evtact_ctor_t evtact_ctor, aosl_gfsm_evtact_dtor_t evtact_dtor, aosl_gfsm_evtact_invoke_t evtact_invoke*/);

/**
 * Trigger the generic fsm with specified event + data.
 * Parameter:
 *       gf: the generic fsm object;
 *    event: the event;
 *     data: the event data, passing NULL if no data needed;
 *   resume: boolean value for specifying whether support coroutine resume operation;
 * Return value:
 *       <0: failed with aosl_errno set, the following resume will be triggered immediately;
 *        0: success with pending action, the following resume will be trigered when done;
 *       >0: success without pending action, the following resume will be triggered immediately;
 * Remarks:
 *     You'd better use an following resume code only when the return value is 0, and do what
 *     you want to do directly rather than use resume code to avoid too deep nested call.
 **/
extern __aosl_api__ int aosl_gfsm_trigger (aosl_ref_t gf, uint32_t event, aosl_data_t data, int resume);

/**
 * Clear the unexecuted events in the queue of the specified generic fsm.
 * Parameter:
 *       gf: the generic fsm object;
 * Return value:
 *       <0: failed with aosl_errno set;
 *        0: success, all the queued pending event will be triggered resume with free only;
 * Remarks:
 *     This function can not stop the current executing event, and only clear the events in
 *     the queue which are not scheduled now.
 **/
extern __aosl_api__ int aosl_gfsm_clear (aosl_ref_t gf);


#ifdef __cplusplus
}
#endif



#endif /* __AOSL_GFSM_H__ */