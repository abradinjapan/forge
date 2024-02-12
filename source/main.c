// alloy
#include "./programs/test/test_anything.h"
#include "./compiler/compiler.h"

// c
#include <stdio.h>

// print context
void MAIN__print__context(ANVIL__context* context) {
    u64 cell_index;
    u64 row_items;

    // setup variables
    cell_index = 0;

    // print program size
    printf("Program Size: [ %lu ]\n", ((ANVIL__u64)(*context).cells[ANVIL__rt__program_end_address] - (ANVIL__u64)(*context).cells[ANVIL__rt__program_start_address]));

    // print cells section header
    printf("Cells:\n");

    // print rows
    while (cell_index < ANVIL__rt__TOTAL_COUNT) {
        // set row items
        row_items = 0;

        // print padding
        printf("\t%lu: [", (ANVIL__u64)cell_index);

        // print columns
        while (cell_index < ANVIL__rt__TOTAL_COUNT && row_items < 8) {
            // print cell value
            printf(" %lu", (ANVIL__u64)(*context).cells[cell_index]);

            // next
            cell_index++;
            row_items++;
        }

        // print padding
        printf(" ]\n");
    }

    // print final padding
    printf("----------\n");

    return;
}

// test code
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

// compiler file type
typedef enum MAIN__cft {
    // lexical tests
    MAIN__cft__lexical_test,

    // parsing tests
    MAIN__cft__parsing_test__1,
    MAIN__cft__parsing_test__2,
    MAIN__cft__parsing_test__3,
    MAIN__cft__parsing_test__4,

    // COUNT
    MAIN__cft__COUNT,
} MAIN__cft;

// test built in compiler
void MAIN__test__built_in_compiler() {
    ANVIL__bt compilation_error_occured;
    ANVIL__bt memory_error_occured;
    ANVIL__bt print_debug = ANVIL__bt__true;
    ANVIL__u8* programs[] = {
        // lexlical test
        (ANVIL__u8*)"((((()()()))((()))(!",

        // parsing tests
        (ANVIL__u8*)"main.hi_yo()() {\n\thello()()\n}\n",
        (ANVIL__u8*)"arguments(hi a)(lo b)\narguments(hi c)(b)\narguments(hi c)(b)",
        (ANVIL__u8*)"test.start()() {\n\thello()()\n}\n",
        (ANVIL__u8*)"main(a f)(b g) {\n\thi(a)(b h)\n\tthere(c i)(d)\n}\n\nthing(a)(b) {\n\thi(a)(b)\n\tthere(c)(d)\n\t@offset\n}",
    };

    // first test
    {
        // setup variables
        LIST__list user_codes;

        // setup list
        user_codes = LIST__open__list(sizeof(ANVIL__buffer) * 4, &memory_error_occured);

        // append files
        LIST__append__buffer(&user_codes, ANVIL__open__buffer_from_string(programs[MAIN__cft__lexical_test], ANVIL__bt__false, ANVIL__bt__true), &memory_error_occured);

        // print memory error
        if (memory_error_occured) {
            printf("Test Error: User code buffer could not be appended.\n");
        }

        // compile
        COMP__compile__files(user_codes, print_debug, &compilation_error_occured, &memory_error_occured);

        // close user codes buffer
        LIST__close__list(user_codes);
    }

    // second test
    {
        // setup variables
        LIST__list user_codes;

        // setup list
        user_codes = LIST__open__list(sizeof(ANVIL__buffer) * 4, &memory_error_occured);

        // append files
        LIST__append__buffer(&user_codes, ANVIL__open__buffer_from_string(programs[MAIN__cft__parsing_test__4], ANVIL__bt__false, ANVIL__bt__true), &memory_error_occured);

        // print memory error
        if (memory_error_occured) {
            printf("Test Error: User code buffer could not be appended.\n");
        }

        // compile
        COMP__compile__files(user_codes, print_debug, &compilation_error_occured, &memory_error_occured);

        // close user codes buffer
        LIST__close__list(user_codes);
    }

    return;
}

// entry point
int main() {
    // notify testing start
    printf("Starting Testing.\n");

    // perform tests
    MAIN__test__scratch();
    MAIN__test__built_in_compiler();

    // exit
    return 0;
}
