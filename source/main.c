// alloy
#include "./programs/test/test_anything.h"
#include "./programs/compile/compile.h"
#include "./programs/test/test_code.h"

// c
#include <stdio.h>

// print context
void MAIN__print__context(ANVIL__context* context) {
    u64 register_index;
    u64 row_items;

    // setup variables
    register_index = 0;

    // print program size
    printf("Program Size: [ %lu ]\n", ((ANVIL__u64)(*context).registers[ANVIL__rt__program_end_address] - (ANVIL__u64)(*context).registers[ANVIL__rt__program_start_address]));

    // print registers section header
    printf("Registers:\n");

    // print rows
    while (register_index < ANVIL__rt__TOTAL_COUNT) {
        // set row items
        row_items = 0;

        // print padding
        printf("\t%lu: [", (ANVIL__u64)register_index);

        // print columns
        while (register_index < ANVIL__rt__TOTAL_COUNT && row_items < 8) {
            // print register value
            printf(" %lu", (ANVIL__u64)(*context).registers[register_index]);

            // next
            register_index++;
            row_items++;
        }

        // print padding
        printf(" ]\n");
    }

    // print final padding
    printf("----------\n");

    return;
}

void MAIN__test__scratch() {
    ANVIL__context context;
    ANVIL__buffer program;

    // build program
    program = TEST__forge__program();

    // check if not built
    if (program.start == 0) {
        // inform user of failure
        printf("Program not built in function: %s", __func__);

        return;
    }

    // create context
    context = ANVIL__setup__context(program);

    // run program
    ANVIL__run__context(&context, ANVIL__define__run_forever);

    // DEBUG
    MAIN__print__context(&context);

    // clean up
    ANVIL__close__buffer(program);

    return;
}

void MAIN__test__compiler() {
    ANVIL__context context;
    ANVIL__buffer program;
    u8* inputs[] = {
        (u8*)"forge.write_register(integer.100)(a)",
    };

    // build program
    program = COMPILE__forge__program();

    // check if not built
    if (program.start == 0) {
        // inform user of failure
        printf("Program not built in function: %s", __func__);

        return;
    }

    // run each test
    for (u64 test = 0; test < (sizeof(inputs) / sizeof(u8*)); test++) {
        // create context
        context = ANVIL__setup__context(program);

        // set input
        ANVIL__set__input(&context, ANVIL__open__buffer_from_string(inputs[test], ANVIL__bt__false, ANVIL__bt__false));

        // run program
        ANVIL__run__context(&context, ANVIL__define__run_forever);

        // clean up
        ANVIL__close__buffer(program);
    }

    return;
}

void MAIN__test__code_writer() {
    ANVIL__context context;
    ANVIL__buffer program;

    // build program
    program = TESTCODE__forge__program();

    // check if not built
    if (program.start == 0) {
        // inform user of failure
        printf("Program not built in function: %s", __func__);

        return;
    }

    // create context
    context = ANVIL__setup__context(program);

    // run program
    ANVIL__run__context(&context, ANVIL__define__run_forever);

    // DEBUG
    MAIN__print__context(&context);

    // clean up
    ANVIL__close__buffer(program);

    return;
}

// entry point
int main() {
    // notify testing start
    printf("Starting Testing.\n");

    // perform tests
    MAIN__test__scratch();
    MAIN__test__compiler();
    MAIN__test__code_writer();

    // exit
    return 0;
}
