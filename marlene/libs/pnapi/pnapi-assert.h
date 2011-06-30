// -*- C++ -*-

/*!
 * \file    pnapi-assert.h
 *
 * \brief   assertion makros
 * 
 *          Keep the customer tools' namespace clean - DO NOT include this file in other headers.
 *
 * \since   2010-05-29
 *
 * \date    $Date: 2010-06-04 00:59:09 +0200 (Fri, 04 Jun 2010) $
 *
 * \version $Revision: 5801 $
 * 
 * This header introduces two kinds of assertion macros.
 * 1.: PNAPI_ASSERT(expr) should be used like cassert, to ensure internal behaviour.
 *     But instead of terminating the whole program, this macro will cause an exception.
 * 2.: C assertions can be disabled by defining the symbol NDEBUG, so does PNAPI_ASSERT.
 *     But some errors are caused by the user (i.e. defining a node name twice or deleting
 *     nonexisting nodes). These kind of errors should still be reportet to the customer tool
 *     by throwing an exception, so PNAPI_ASSERT_USER(expr, msg) can not be disabled by defining
 *     NDEBUG. When "expr" evaluates to false, an exception containing the message "msg"
 *     will be thrown.
 * 
 * Both makros can be disabled by defining the symbol PNAPI_USE_C_ASSERTS. They will behave like
 * assert(expr) then.  
 */

#ifndef PNAPI_ASSERT_H
#define PNAPI_ASSERT_H

#include "config.h"
#include "exception.h"


/*!
 * \brief macros allowing PNAPI_ASSERT_USER's last parameters to be optional
 * 
 * Thanks to Laurent Deniau
 * (See http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5?lnk=st&q=&rnum=1#346fc464319b1ee5) 
 */
#define PNAPI_PP_NARG(...) PNAPI_PP_NARG_(__VA_ARGS__, PNAPI_PP_RSEQ_N())
#define PNAPI_PP_NARG_(...) PNAPI_PP_ARG_N(__VA_ARGS__) // this line is needed to enforce expansion of PNAPI_PP_RSEQ_N
#define PNAPI_PP_ARG_N(_1, _2, _3, N, ...) N
#define PNAPI_PP_RSEQ_N() 3, 2, 1, 0

#define PNAPI_ASSERT_USER__(...) PNAPI_ASSERT_USER___(__VA_ARGS__) // expand PNAPI_PP_NARG (see below)
#define PNAPI_ASSERT_USER___(x, ...) PNAPI_ASSERT_USER_ ## x(__VA_ARGS__)  

#define PNAPI_ASSERT_USER_1(expr) ((expr) ? static_cast<void>(0) : (throw pnapi::exception::UserCausedError(pnapi::exception::UserCausedError::UE_NONE, #expr)))
#define PNAPI_ASSERT_USER_2(expr, msg) ((expr) ? static_cast<void>(0) : (throw pnapi::exception::UserCausedError(pnapi::exception::UserCausedError::UE_NONE, msg)))
#define PNAPI_ASSERT_USER_3(expr, msg, type) ((expr) ? static_cast<void>(0) : (throw pnapi::exception::UserCausedError(type, msg)))

/********************\
 * assertion makros *
\********************/

#ifdef PNAPI_USE_C_ASSERTS
#define PNAPI_ASSERT(expr) assert(expr)
#define PNAPI_ASSERT_USER(expr, ...) assert(expr)
#else

#define PNAPI_ASSERT_USER(...) PNAPI_ASSERT_USER__(PNAPI_PP_NARG(__VA_ARGS__), __VA_ARGS__)

#ifdef NDEBUG
#define PNAPI_ASSERT(expr) assert(expr)
#else
#define PNAPI_ASSERT(expr) ((expr) ? static_cast<void>(0) : (throw pnapi::exception::AssertionFailedError( __FILE__ , __LINE__ , #expr ))) 
#endif /* NDEBUG */

#endif /* PNAPI_USE_C_ASSERTS */

#endif /*PNAPIASSERT_H_*/
