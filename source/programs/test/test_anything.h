#ifndef ANVIL__test_stack
#define ANVIL__test_stack

/* Include */
// essentials
#include "../essentials.h"

// code
#include "../code.h"

/* Offsets */
// offsets
typedef enum TEST__ot {
    TEST__ot__main__start,
    TEST__ot__start__message_1,
    TEST__ot__build_program__start,

    TEST__ot__COUNT,
} TEST__ot;

typedef struct TEST__offsets {
    ANVIL__offset offsets[TEST__ot__COUNT];
} TEST__offsets;

/* Create Program */
// register types
typedef enum TEST__build_program {
	// preserve start
	TEST__build_program__preserve__START = ANVIL__srt__start__workspace,

	// variables
	TEST__build_program__write_to = TEST__build_program__preserve__START,
	TEST__build_program__do_write,

	// preserve end
	TEST__build_program__preserve__END,

	// inputs
	TEST__build_program__input__write_to = ANVIL__srt__start__function_io,
	TEST__build_program__input__do_write,

	// outputs
	TEST__build_program__output__write_to = ANVIL__srt__start__function_io,
} TEST__build_program;

// call function
void TEST__code__call__build_program(ANVIL__workspace* workspace, TEST__offsets* test_offsets, ANVIL__flag_ID flag, ANVIL__register_ID input__write_to, ANVIL__register_ID input__do_write, ANVIL__register_ID output__write_to) {
	// pass inputs
	ANVIL__code__register_to_register(workspace, flag, input__write_to, TEST__build_program__input__write_to);
	ANVIL__code__register_to_register(workspace, flag, input__do_write, TEST__build_program__input__do_write);

	// call function
	ANVIL__code__call__static(workspace, flag, (*test_offsets).offsets[TEST__ot__build_program__start]);

	// pass outputs
	ANVIL__code__register_to_register(workspace, flag, TEST__build_program__output__write_to, output__write_to);
}

// build function
void TEST__code__build_program(ANVIL__workspace* workspace, TEST__offsets* test_offsets, ESS__offsets* essential_offsets) {
	// setup function offset
	(*test_offsets).offsets[TEST__ot__build_program__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, TEST__build_program__preserve__START, TEST__build_program__preserve__END);

	// get inputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, TEST__build_program__input__write_to, TEST__build_program__write_to);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, TEST__build_program__input__do_write, TEST__build_program__do_write);

	// code here
    CODE__code__instruction__stop(workspace, essential_offsets, TEST__build_program__write_to, TEST__build_program__do_write);

	// setup outputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, TEST__build_program__write_to, TEST__build_program__output__write_to);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, TEST__build_program__preserve__START, TEST__build_program__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}


/* Main */
// register types
typedef enum TEST__rt__main {
    // preserve start
    TEST__rt__main__preserve__START = ANVIL__srt__start__workspace,

    // variables
    TEST__rt__main__buffer_address = TEST__rt__main__preserve__START,
    TEST__rt__main__buffer_start,
    TEST__rt__main__buffer_end,
    TEST__rt__main__buffer_length,
    TEST__rt__main__do_write,
    TEST__rt__main__write_to,
    
    // preserve end
    TEST__rt__main__preserve__END,
} TEST__rt__main;

// create main
void TEST__code__main(ANVIL__workspace* workspace, TEST__offsets* test_offsets, ESS__offsets* essential_offsets) {
    // setup function start
    (*test_offsets).offsets[TEST__ot__main__start] = ANVIL__get__offset(workspace);

    // preserve workspace
    ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, TEST__rt__main__preserve__START, TEST__rt__main__preserve__END);

    // print test characters
    ANVIL__code__write_register(workspace, (ANVIL__register)'H', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    ANVIL__code__write_register(workspace, (ANVIL__register)'i', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    ANVIL__code__write_register(workspace, (ANVIL__register)'!', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    ANVIL__code__write_register(workspace, (ANVIL__register)'\n', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);

    // get buffer
    ANVIL__code__calculate_statically__offset_address(workspace, ANVIL__sft__always_run, (*test_offsets).offsets[TEST__ot__start__message_1], TEST__rt__main__buffer_address);
    ESS__code__call__retrieve_buffer__explicit(workspace, essential_offsets, ANVIL__sft__always_run, TEST__rt__main__buffer_address, TEST__rt__main__buffer_start, TEST__rt__main__buffer_end);
    ESS__code__call__print_buffer(workspace, essential_offsets, ANVIL__sft__always_run, TEST__rt__main__buffer_start, TEST__rt__main__buffer_end);

    // print buffer stats
    ANVIL__code__write_register(workspace, (ANVIL__register)'\n', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    ANVIL__code__debug__print_register_as_decimal(workspace, TEST__rt__main__buffer_start);
    ANVIL__code__write_register(workspace, (ANVIL__register)'\n', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    ANVIL__code__debug__print_register_as_decimal(workspace, TEST__rt__main__buffer_end);
    ANVIL__code__write_register(workspace, (ANVIL__register)'\n', ANVIL__srt__temp__write);
    ANVIL__code__debug__putchar(workspace, ANVIL__srt__temp__write);
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_subtract, TEST__rt__main__buffer_end, TEST__rt__main__buffer_start, ANVIL__unused_register_ID, TEST__rt__main__buffer_length);
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, TEST__rt__main__buffer_length, ANVIL__srt__constant__1, ANVIL__unused_register_ID, TEST__rt__main__buffer_length);
    ANVIL__code__debug__print_register_as_decimal(workspace, TEST__rt__main__buffer_length);

    // get user input
    ANVIL__code__debug__fgets(workspace, TEST__rt__main__buffer_start, TEST__rt__main__buffer_end);
    ESS__code__call__print_buffer(workspace, essential_offsets, ANVIL__sft__always_run, TEST__rt__main__buffer_start, TEST__rt__main__buffer_end);

    // clean up user input
    ANVIL__code__return_memory(workspace, TEST__rt__main__buffer_start, TEST__rt__main__buffer_end);

    // restore workspace
    ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, TEST__rt__main__preserve__START, TEST__rt__main__preserve__END);

    // setup return
    ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

    // setup buffer offset
    (*test_offsets).offsets[TEST__ot__start__message_1] = ANVIL__get__offset(workspace);

    // write file address
    ANVIL__code__buffer(workspace, ANVIL__open__buffer_from_string((u8*)"./binary/filler.txt", ANVIL__bt__false, ANVIL__bt__false));

    return;
}

/* Package Building */
// create program
void TEST__code__package(ANVIL__workspace* workspace, TEST__offsets* test_offsets, ESS__offsets* essential_offsets) {
    // build program
    TEST__code__main(workspace, test_offsets, essential_offsets);

    return;
}

/* Forge Program */
// build program
ANVIL__buffer TEST__forge__program() {
    ANVIL__buffer output;
    ANVIL__workspace workspace;
    TEST__offsets test_offsets;
    ESS__offsets essential_offsets;

    // create workspace
    workspace = ANVIL__setup__workspace(&output);

    // create program
    for (ANVIL__pt pass = ANVIL__pt__get_offsets; pass < ANVIL__pt__COUNT; pass = (ANVIL__pt)((u64)pass + 1)) {
        // setup pass
        ANVIL__setup__pass(&workspace, pass);

        // build program
        ANVIL__code__start(&workspace, 1024, test_offsets.offsets[TEST__ot__main__start]);
        TEST__code__package(&workspace, &test_offsets, &essential_offsets);
        ESS__code__package(&workspace, &essential_offsets);
    }

    return output;
}

#endif
