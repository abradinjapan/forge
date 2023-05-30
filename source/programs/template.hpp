#ifndef ANVIL__code
#define ANVIL__code

/* Include */
// anvil
#include "essentials.h"

/* Offsets */
// offset type
typedef enum TEST__ot {
    // offsets
    TEST__ot__package_start,

    // count
    TEST__ot__COUNT,
} TEST__ot;

// offsets
typedef struct TEST__offsets {
    ANVIL__offset offsets[TEST__ot__COUNT];
} TEST__offsets;

/* Build Package */
void TEST__code__package(ANVIL__workspace* workspace, TEST__offsets* test_offsets, ESS__offsets* essential_offsets) {
    // write functions
    // TODO

    return;
}

#endif
