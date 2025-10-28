//
//  AssertAPI.h
//  KanViz
//
//  Created by Ashish . on 28/10/25.
//

#pragma once

#include "Debug/Logger.hpp"

#define IK_EXPAND_VARGS(x) x
#define ASSERT(condition) assert(condition);

#define IK_ASSERT_NO_MESSAGE(condition) { \
if(!(condition)) { \
IK_LOG_ERROR("Assert", "Assertion Failed"); \
ASSERT(condition); \
} \
}

#define IK_ASSERT_MESSAGE(condition, ...) { \
if(!(condition)) { \
IK_LOG_ERROR("Assert", "Assertion Failed: {0}", __VA_ARGS__); \
ASSERT(condition); \
} \
} \

#define IK_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro

#define IK_GET_ASSERT_MACRO(...) \
IK_EXPAND_VARGS( \
IK_ASSERT_RESOLVE( \
__VA_ARGS__, \
IK_ASSERT_MESSAGE, \
IK_ASSERT_NO_MESSAGE \
) \
)

#define IK_ABORT(condition) IK_ASSERT_NO_MESSAGE(condition);

#ifdef IK_ENABLE_ASSERT

#define IK_ASSERT(...) \
IK_EXPAND_VARGS(\
IK_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__)\
)

#else

#define IK_ASSERT(...)

#endif

#define IK_FORCE_ASSERT(...) \
IK_EXPAND_VARGS(\
IK_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__)\
)
