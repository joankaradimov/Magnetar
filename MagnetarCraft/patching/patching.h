#include "patching/FailStubPatch.h"
#include "patching/FunctionPatch.h"
#include "patching/CallSitePatch.h"
#include "patching/MemoryPatch.h"
#include "patching/NopPatch.h"

#define CONCAT_IMPL(x, y, z) x ## y ## z
#define CONCAT(x, y, z) CONCAT_IMPL(x, y, z)

#define FUNCTION_PATCH(destination, replacement) \
        FunctionPatch CONCAT(function_patch_, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, replacement)

#define FAIL_STUB_PATCH(destination) \
        FailStubPatch CONCAT(fail_stub_patch_, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination)

#define CALL_SITE_PATCH(destination, function) \
        CallSitePatch CONCAT(call_site_patch_, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, function)

#define NOP_PATCH(destination, block_length) \
        NopPatch CONCAT(nop_patch_, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, block_length)

#define MEMORY_PATCH(destination, data) \
        MemoryPatch CONCAT(memory_patch_, __LINE__, __COUNTER__)(__FILE__, __LINE__, destination, data)
