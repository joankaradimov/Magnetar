#include "patching/FailStubPatch.h"
#include "patching/FunctionPatch.h"
#include "patching/CallSitePatch.h"
#include "patching/MemoryPatch.h"
#include "patching/NopPatch.h"

#define CONCAT_IMPL3(x, y, z) x ## _ ## y ## _ ## z
#define CONCAT3(x, y, z) CONCAT_IMPL3(x, y, z)

#define CONCAT_IMPL4(a, b, c, d) a ## _ ## b ## _ ## c ## _ ## d
#define CONCAT4(a, b, c, d) CONCAT_IMPL4(a, b, c, d)

#define FUNCTION_PATCH(destination, replacement) \
        FunctionPatch CONCAT3(function_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, replacement)

#define FAIL_STUB_PATCH(destination) \
        FailStubPatch CONCAT4(fail_stub_patch, destination, __BASE_FILE__, __LINE__)(__FILE__, __LINE__, destination)

#define CALL_SITE_PATCH(destination, function) \
        CallSitePatch CONCAT3(call_site_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, function)

#define NOP_PATCH(destination, block_length) \
        NopPatch CONCAT3(nop_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, block_length)

#define MEMORY_PATCH(destination, data) \
        MemoryPatch CONCAT3(memory_patch, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, data)
