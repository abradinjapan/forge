#ifndef ANVIL__test_code
#define ANVIL__test_code

/* Include */
// essentials
#include "../code.h"

/* Offsets */
// offset types
typedef enum TESTCODE__ot {
    // main
    TESTCODE__ot__main__start,

	// test write bytes
	TESTCODE__ot__test_write_bytes__start,

    // count
    TESTCODE__ot__COUNT,
} TESTCODE__ot;

// offsets
typedef struct TESTCODE__offsets {
    ANVIL__offset offsets[TESTCODE__ot__COUNT];
} TESTCODE__offsets;

/* Test Write Bytes */
// register types
typedef enum TESTCODE__test_write_bytes {
	// preserve start
	TESTCODE__test_write_bytes__preserve__START = ANVIL__srt__start__workspace,

	// variables
	TESTCODE__test_write_bytes__write_to = TESTCODE__test_write_bytes__preserve__START,
	TESTCODE__test_write_bytes__write_offset,
	TESTCODE__test_write_bytes__data,
	TESTCODE__test_write_bytes__byte_size,
	TESTCODE__test_write_bytes__do_write,
	TESTCODE__test_write_bytes__new_write_to,
	TESTCODE__test_write_bytes__new_write_offset,
	TESTCODE__test_write_bytes__program_start,
	TESTCODE__test_write_bytes__program_end,
	TESTCODE__test_write_bytes__data_temp,

	// preserve end
	TESTCODE__test_write_bytes__preserve__END,
} TESTCODE__test_write_bytes;

// call function
void TESTCODE__code__call__test_write_bytes(ANVIL__workspace* workspace, TESTCODE__offsets* testcode_offsets, ANVIL__flag_ID flag) {
	// call function
	ANVIL__code__call__static(workspace, flag, (*testcode_offsets).offsets[TESTCODE__ot__test_write_bytes__start]);

	return;
}

// build function
void TESTCODE__code__test_write_bytes(ANVIL__workspace* workspace, TESTCODE__offsets* testcode_offsets, CODE__offsets* code_offsets) {
	// setup function offset
	(*testcode_offsets).offsets[TESTCODE__ot__test_write_bytes__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, TESTCODE__test_write_bytes__preserve__START, TESTCODE__test_write_bytes__preserve__END);

	// setup first pass variables
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__write_to);
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__write_offset);
	ANVIL__code__write_register(workspace, (ANVIL__register)123, TESTCODE__test_write_bytes__data);
	ANVIL__code__write_register(workspace, (ANVIL__register)8, TESTCODE__test_write_bytes__byte_size);
	ANVIL__code__write_register(workspace, (ANVIL__register)ANVIL__bt__false, TESTCODE__test_write_bytes__do_write);
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__new_write_to);
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__new_write_offset);

	// call byte writer
	CODE__code__call__write_bytes(workspace, code_offsets, ANVIL__sft__always_run, TESTCODE__test_write_bytes__write_to, TESTCODE__test_write_bytes__write_offset, TESTCODE__test_write_bytes__data, TESTCODE__test_write_bytes__byte_size, TESTCODE__test_write_bytes__do_write, TESTCODE__test_write_bytes__new_write_to, TESTCODE__test_write_bytes__new_write_offset);

	// print stats
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"Byte Write No Write Results:\nnew_write_to: ", ANVIL__bt__false, ANVIL__bt__false));
	ANVIL__code__debug__print_register_as_decimal(workspace, TESTCODE__test_write_bytes__new_write_to);
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\nnew_write_offset: ", ANVIL__bt__false, ANVIL__bt__false));
	ANVIL__code__debug__print_register_as_decimal(workspace, TESTCODE__test_write_bytes__new_write_offset);
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\n", ANVIL__bt__false, ANVIL__bt__false));

	// allocate memory
	ANVIL__code__request_memory(workspace, TESTCODE__test_write_bytes__new_write_offset, TESTCODE__test_write_bytes__program_start, TESTCODE__test_write_bytes__program_end);

	// setup second pass variables
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, TESTCODE__test_write_bytes__program_start, TESTCODE__test_write_bytes__write_to);
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__write_offset);
	ANVIL__code__write_register(workspace, (ANVIL__register)123, TESTCODE__test_write_bytes__data);
	ANVIL__code__write_register(workspace, (ANVIL__register)8, TESTCODE__test_write_bytes__byte_size);
	ANVIL__code__write_register(workspace, (ANVIL__register)ANVIL__bt__true, TESTCODE__test_write_bytes__do_write);
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__new_write_to);
	ANVIL__code__write_register(workspace, 0, TESTCODE__test_write_bytes__new_write_offset);

	// call byte writer
	CODE__code__call__write_bytes(workspace, code_offsets, ANVIL__sft__always_run, TESTCODE__test_write_bytes__write_to, TESTCODE__test_write_bytes__write_offset, TESTCODE__test_write_bytes__data, TESTCODE__test_write_bytes__byte_size, TESTCODE__test_write_bytes__do_write, TESTCODE__test_write_bytes__new_write_to, TESTCODE__test_write_bytes__new_write_offset);

	// print stats
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"Byte Write With Write Results:\nnew_write_to: ", ANVIL__bt__false, ANVIL__bt__false));
	ANVIL__code__debug__print_register_as_decimal(workspace, TESTCODE__test_write_bytes__new_write_to);
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\nnew_write_offset: ", ANVIL__bt__false, ANVIL__bt__false));
	ANVIL__code__debug__print_register_as_decimal(workspace, TESTCODE__test_write_bytes__new_write_offset);
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\n", ANVIL__bt__false, ANVIL__bt__false));

	// read data from buffer
	ANVIL__code__address_to_register(workspace, TESTCODE__test_write_bytes__program_start, ANVIL__srt__constant__64, TESTCODE__test_write_bytes__data_temp);

	// print data
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\nWritten Data: ", ANVIL__bt__false, ANVIL__bt__false));
	ANVIL__code__debug__print_register_as_decimal(workspace, TESTCODE__test_write_bytes__data_temp);
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\n", ANVIL__bt__false, ANVIL__bt__false));

	// close buffer
	ANVIL__code__return_memory(workspace, TESTCODE__test_write_bytes__program_start, TESTCODE__test_write_bytes__program_end);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, TESTCODE__test_write_bytes__preserve__START, TESTCODE__test_write_bytes__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

/* Main */
// register types
typedef enum TESTCODE__main {
	// preserve start
	TESTCODE__main__preserve__START = ANVIL__srt__start__workspace,

	// variables
	TESTCODE__main__write_to = TESTCODE__main__preserve__START,

	// preserve end
	TESTCODE__main__preserve__END,

	// inputs

	// outputs
} TESTCODE__main;

// build function
void TESTCODE__code__main(ANVIL__workspace* workspace, TESTCODE__offsets* testcode_offsets) {
	// setup function offset
	(*testcode_offsets).offsets[TESTCODE__ot__main__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, TESTCODE__main__preserve__START, TESTCODE__main__preserve__END);

	// inform user of program start
	ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"Testing Coder\n", ANVIL__bt__false, ANVIL__bt__false));

	// test byte writer
	TESTCODE__code__call__test_write_bytes(workspace, testcode_offsets, ANVIL__sft__always_run);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, TESTCODE__main__preserve__START, TESTCODE__main__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

/* Code Package */
// create the package
void TESTCODE__code__package(ANVIL__workspace* workspace, TESTCODE__offsets* testcode_offsets, CODE__offsets* code_offsets) {
	// build functions
    TESTCODE__code__main(workspace, testcode_offsets);
	TESTCODE__code__test_write_bytes(workspace, testcode_offsets, code_offsets);
	
	return;
}

/* Forge Program */
// build program
ANVIL__buffer TESTCODE__forge__program() {
    ANVIL__buffer output;
    ANVIL__workspace workspace;
	CODE__offsets code_offsets;
    TESTCODE__offsets testcode_offsets;

    // create workspace
    workspace = ANVIL__setup__workspace(&output);

    // create program
    for (ANVIL__pt pass = ANVIL__pt__get_offsets; pass < ANVIL__pt__COUNT; pass++) {
        // setup pass
        ANVIL__setup__pass(&workspace, pass);

        // build program
        ANVIL__code__start(&workspace, 1024, testcode_offsets.offsets[TESTCODE__ot__main__start]);
		TESTCODE__code__package(&workspace, &testcode_offsets, &code_offsets);
		CODE__code__package(&workspace, &code_offsets);
    }

    return output;
}

#endif
