#ifndef _MENCUS_ASSERT_HPP_
#define _MENCUS_ASSERT_HPP_

#include <assert.h>
#include "GlobalBuildDefines.hpp"

#ifndef MENCUS_ASSERT_MODE
#error "Mencus assert mode is undefined. Try to rerun cmake build script."
#else

#if MENCUS_ASSERT_MODE == 0
#define MENCUS_ASSERT(cond) 
#elif MENCUS_ASSERT_MODE == 1
#define MENCUS_ASSERT(cond) assert(cond)
#endif


#endif

#endif
