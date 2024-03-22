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
    ANVIL__allocations allocations;
    ANVIL__bt allocations_memory_failure = ANVIL__bt__false;

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

    // create allocations
    allocations = ANVIL__open__allocations(&allocations_memory_failure);

    // add program as an allocation
    ANVIL__remember__allocation(&allocations, program, &allocations_memory_failure);

    // if allocations were opened
    if (allocations_memory_failure == ANVIL__bt__false) {
        // run program
        ANVIL__run__context(&allocations, &context, ANVIL__define__run_forever);
    }

    // DEBUG
    // MAIN__print__context(&context);

    // clean up
    ANVIL__close__allocations(&allocations);
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

    // accounting tests
    MAIN__cft__accounting_test__1,
    MAIN__cft__accounting_test__2,

    // COUNT
    MAIN__cft__COUNT,
} MAIN__cft;

// test built in compiler
void MAIN__test__built_in_compiler() {
    COMP__error error = COMP__create_null__error();
    ANVIL__buffer error_json;
    ANVIL__bt memory_error_occured;
    ANVIL__bt print_debug = ANVIL__bt__true;
    ANVIL__u8* programs[MAIN__cft__COUNT] = {
        // lexical tests
        (ANVIL__u8*)"((((()()()))((()))(!",

        // parsing tests
        (ANVIL__u8*)"main.hi_yo()() = {\n\thello()()\n}\n",
        (ANVIL__u8*)"test.start()() = {\n\thello()()\n}\n",
        (ANVIL__u8*)"main(a f)(b g) = {\n\thi(a)(b h)\n\tthere(c i)(d)\n}\n\nthing(a)(b) = {\n\thi(a)(b)\n\tthere(c)(d)\n\t@offset\n}",

        // accounting tests
        (ANVIL__u8*)"main()() = {\n\ttest()()\n}\ntest()() = {\n\n}",
        (ANVIL__u8*)"foo(a)(b) = {\n\tbar(a)(b b)\n}\nbar(a)(b c) = {\n\n}",
    };

    // first test
    {
        // setup variables
        ANVIL__list user_codes;

        // setup list
        user_codes = ANVIL__open__list(sizeof(ANVIL__buffer) * 4, &memory_error_occured);

        // append files
        ANVIL__list__append__buffer(&user_codes, ANVIL__open__buffer_from_string(programs[MAIN__cft__lexical_test], ANVIL__bt__false, ANVIL__bt__true), &memory_error_occured);

        // print memory error
        if (memory_error_occured) {
            printf("Test Error: User code buffer could not be appended.\n");
        }

        // compile
        COMP__compile__files(user_codes, print_debug, &error);

        // check for error
        if (COMP__check__error_occured(&error) == ANVIL__bt__true) {
            // print error
            error_json = COMP__serialize__error_json(error, &memory_error_occured);
            ANVIL__print__buffer(error_json);

            // clean up
            COMP__close__error(error);
            ANVIL__close__buffer(error_json);
        }

        // close user codes buffer
        ANVIL__close__list(user_codes);
    }

    // second test
    {
        // setup variables
        ANVIL__list user_codes;

        // setup list
        user_codes = ANVIL__open__list(sizeof(ANVIL__buffer) * 4, &memory_error_occured);

        // append files
        ANVIL__list__append__buffer(&user_codes, ANVIL__open__buffer_from_string(programs[MAIN__cft__parsing_test__3], ANVIL__bt__false, ANVIL__bt__true), &memory_error_occured);

        // print memory error
        if (memory_error_occured) {
            printf("Test Error: User code buffer could not be appended.\n");
        }

        // compile
        COMP__compile__files(user_codes, print_debug, &error);

        // check for error
        if (COMP__check__error_occured(&error) == ANVIL__bt__true) {
            // print error
            error_json = COMP__serialize__error_json(error, &memory_error_occured);
            ANVIL__print__buffer(error_json);

            // clean up
            COMP__close__error(error);
            ANVIL__close__buffer(error_json);
        }

        // close user codes buffer
        ANVIL__close__list(user_codes);
    }

    // third test
    {
        // setup variables
        ANVIL__list user_codes;

        // setup list
        user_codes = ANVIL__open__list(sizeof(ANVIL__buffer) * 4, &memory_error_occured);

        // append files
        ANVIL__list__append__buffer(&user_codes, ANVIL__open__buffer_from_string(programs[MAIN__cft__accounting_test__1], ANVIL__bt__false, ANVIL__bt__true), &memory_error_occured);
        ANVIL__list__append__buffer(&user_codes, ANVIL__open__buffer_from_string(programs[MAIN__cft__accounting_test__2], ANVIL__bt__false, ANVIL__bt__true), &memory_error_occured);

        // print memory error
        if (memory_error_occured) {
            printf("Test Error: User code buffer could not be appended.\n");
        }

        // compile
        COMP__compile__files(user_codes, print_debug, &error);

        // check for error
        if (COMP__check__error_occured(&error) == ANVIL__bt__true) {
            // print error
            error_json = COMP__serialize__error_json(error, &memory_error_occured);
            ANVIL__print__buffer(error_json);

            // clean up
            COMP__close__error(error);
            ANVIL__close__buffer(error_json);
        }

        // close user codes buffer
        ANVIL__close__list(user_codes);
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
