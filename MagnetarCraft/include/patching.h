#include "patching/FailStubPatch.h"
#include "patching/FunctionPatch.h"
#include "patching/CallSitePatch.h"
#include "patching/MemoryPatch.h"
#include "patching/NopPatch.h"

#define CONCAT_IMPL3(x, y, z) x ## _ ## y ## _ ## z
#define CONCAT3(x, y, z) CONCAT_IMPL3(x, y, z)

#define CONCAT_IMPL4(a, b, c, d) a ## _ ## b ## _ ## c ## _ ## d
#define CONCAT4(a, b, c, d) CONCAT_IMPL4(a, b, c, d)

#define FUNCTION_PATCH(destination, replacement, tag, __VA_ARGS__) \
        FunctionPatch CONCAT3(function_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, replacement, {tag, __VA_ARGS__})

#define FAIL_STUB_PATCH(destination, tag, __VA_ARGS__) \
        FailStubPatch CONCAT4(fail_stub_patch, destination, __BASE_FILE__, __LINE__)(__FILE__, __LINE__, destination, {tag, __VA_ARGS__})

#define CALL_SITE_PATCH(destination, function, tag, __VA_ARGS__) \
        CallSitePatch CONCAT3(call_site_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, function, {tag, __VA_ARGS__})

#define NOP_PATCH(destination, block_length, tag, __VA_ARGS__) \
        NopPatch CONCAT3(nop_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, block_length, {tag, __VA_ARGS__})

#define MEMORY_PATCH(destination, data, tag, __VA_ARGS__) \
        MemoryPatch CONCAT3(memory_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, data, {tag, __VA_ARGS__})
