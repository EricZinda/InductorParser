#pragma once

#define StaticFailFastAssertDesc(expression, description) failfast_uncaught_exception_handler(expression, description, __FILE__, __LINE__);
#define StaticFailFastAssert(expression) failfast_uncaught_exception_handler(expression, __FILE__, __LINE__);

#define FailFastAssertDesc(expression, description) failfastThis_uncaught_exception_handler((void *) this, expression, description, __FILE__, __LINE__);
#define FailFastAssert(expression) failfastThis_uncaught_exception_handler((void *) this, expression, __FILE__, __LINE__);

#if defined(_DEBUG) || DEBUG == 1
#define FXDebugAssert(expression) FailFastAssert(expression);
#else
#define FXDebugAssert(expression)
#endif

#include <exception>

// Name using _uncaught_exception_handler on all of this so that HockeyApp ignores this part of the stack trace when grouping traces
void TreatFailFastAsException(bool value);
void failfast_uncaught_exception_handler(bool expression, const char *file, long line);
void failfast_uncaught_exception_handler(bool expression, const char *description, const char *file, long line);
void failfastThis_uncaught_exception_handler(void *thisPtr, bool expression, const char *description, const char *file, long line);
void failfastThis_uncaught_exception_handler(void *thisPtr, bool expression, const char *file, long line);

void uncaught_exception_handler (void);
// Done so this is executed when globals are initialized
extern std::terminate_handler previousHandler;
