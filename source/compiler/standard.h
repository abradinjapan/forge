#ifndef FROST__standard
#define FROST__standard

/* Include */
// anvil
#include "../programs/essentials.h"

/* Offsets */
// offset type
typedef enum STD__ot {
    // offsets
    STD__ot__package_start,

    // printing functions
    STD__ot__print_binary__start,
    STD__ot__print_buffer_as_string__start,
    STD__ot__print_buffer_as_string__loop__start,
    STD__ot__print_buffer_as_string__loop__end,

    // count
    STD__ot__COUNT,
} STD__ot;

// offsets
typedef struct STD__offsets {
    ANVIL__offset offsets[STD__ot__COUNT];
} STD__offsets;

/* Printing Functions */
// cell types
typedef enum STD__print_binary {
	// preserve start
	STD__print_binary__preserve__START = ANVIL__srt__start__workspace,

	// variables
	STD__print_binary__value = STD__print_binary__preserve__START,
    STD__print_binary__bit,
    STD__print_binary__character,

	// preserve end
	STD__print_binary__preserve__END,

	// inputs
	STD__print_binary__input__value = ANVIL__srt__start__function_io,

	// outputs
} STD__print_binary;

// call function
void STD__code__call__print_binary(ANVIL__workspace* workspace, STD__offsets* standard_offsets, ANVIL__flag_ID flag, ANVIL__cell_ID input__value) {
	// pass inputs
	ANVIL__code__cell_to_cell(workspace, flag, input__value, STD__print_binary__input__value);

	// call function
	ANVIL__code__call__static(workspace, flag, (*standard_offsets).offsets[STD__ot__print_binary__start]);

	return;
}

// build function
void STD__code__print_binary(ANVIL__workspace* workspace, STD__offsets* standard_offsets) {
	// setup function offset
	(*standard_offsets).offsets[STD__ot__print_binary__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, STD__print_binary__preserve__START, STD__print_binary__preserve__END);

	// get inputs
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__print_binary__input__value, STD__print_binary__value);

	// print all bits
    // for each byte
    for (ANVIL__byte_count byte_index = 0; byte_index < sizeof(ANVIL__cell); byte_index++) {
        // for each bit
        for (ANVIL__bit_count bit_index = 0; bit_index < ANVIL__define__bits_in_byte; bit_index++) {
            // get bit
            ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__bits_and, STD__print_binary__value, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, STD__print_binary__bit);

            // get character
            ANVIL__code__write_cell(workspace, (ANVIL__cell)'0', STD__print_binary__character);
            ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, STD__print_binary__character, STD__print_binary__bit, ANVIL__unused_cell_ID, STD__print_binary__character);

            // print character
            ANVIL__code__debug__putchar(workspace, STD__print_binary__character);

            // next bit
            ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__bits_shift_lower, STD__print_binary__value, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, STD__print_binary__value);
        }

        // print separator if not end of loop
        if (byte_index < sizeof(ANVIL__cell) - 1) {
            ANVIL__code__write_cell(workspace, (ANVIL__cell)'_', STD__print_binary__character);
            ANVIL__code__debug__putchar(workspace, STD__print_binary__character);
        }
    }

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, STD__print_binary__preserve__START, STD__print_binary__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

// cell types
typedef enum STD__print_buffer_as_string {
	// preserve start
	STD__print_buffer_as_string__preserve__START = ANVIL__srt__start__workspace,

	// variables
	STD__print_buffer_as_string__buffer_start = STD__print_buffer_as_string__preserve__START,
	STD__print_buffer_as_string__buffer_end,
	STD__print_buffer_as_string__current,
	STD__print_buffer_as_string__character,

	// preserve end
	STD__print_buffer_as_string__preserve__END,

	// inputs
	STD__print_buffer_as_string__input__buffer_start = ANVIL__srt__start__function_io,
	STD__print_buffer_as_string__input__buffer_end,

	// outputs
} STD__print_buffer_as_string;

// call function
void STD__code__call__print_buffer_as_string(ANVIL__workspace* workspace, STD__offsets* std_offsets, ANVIL__flag_ID flag, ANVIL__cell_ID input__buffer_start, ANVIL__cell_ID input__buffer_end) {
	// pass inputs
	ANVIL__code__cell_to_cell(workspace, flag, input__buffer_start, STD__print_buffer_as_string__input__buffer_start);
	ANVIL__code__cell_to_cell(workspace, flag, input__buffer_end, STD__print_buffer_as_string__input__buffer_end);

	// call function
	ANVIL__code__call__static(workspace, flag, (*std_offsets).offsets[STD__ot__print_buffer_as_string__start]);

	return;
}

// build function
void STD__code__print_buffer_as_string(ANVIL__workspace* workspace, STD__offsets* std_offsets) {
	// setup function offset
	(*std_offsets).offsets[STD__ot__print_buffer_as_string__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__preserve__START, STD__print_buffer_as_string__preserve__END);

	// get inputs
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__input__buffer_start, STD__print_buffer_as_string__buffer_start);
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__input__buffer_end, STD__print_buffer_as_string__buffer_end);

	// setup current
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__buffer_start, STD__print_buffer_as_string__current);

    // setup loop start offset
	(*std_offsets).offsets[STD__ot__print_buffer_as_string__loop__start] = ANVIL__get__offset(workspace);

    // check if loop range is still valid and exit if necessary
    ANVIL__code__write_cell(workspace, (ANVIL__cell)(ANVIL__cell_integer_value)ANVIL__sft__temp, ANVIL__srt__temp__flag_ID);
    ANVIL__code__operate__jump__static(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__buffer_start, STD__print_buffer_as_string__current, STD__print_buffer_as_string__buffer_end, ANVIL__sft__always_run, (*std_offsets).offsets[STD__ot__print_buffer_as_string__loop__end]);

    // read character
    ANVIL__code__address_to_cell(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__current, ANVIL__srt__constant__ascii_character_bit_size, STD__print_buffer_as_string__character);

    // print character
    ANVIL__code__debug__putchar(workspace, STD__print_buffer_as_string__character);

    // next character
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, STD__print_buffer_as_string__current, ANVIL__srt__constant__ascii_character_byte_size, ANVIL__unused_cell_ID, STD__print_buffer_as_string__current);

    // jump to beginning of loop
    ANVIL__code__jump__static(workspace, ANVIL__sft__always_run, (*std_offsets).offsets[STD__ot__print_buffer_as_string__loop__start]);

    // setup loop end offset
	(*std_offsets).offsets[STD__ot__print_buffer_as_string__loop__end] = ANVIL__get__offset(workspace);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, STD__print_buffer_as_string__preserve__START, STD__print_buffer_as_string__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

/* Build Package */
// build package
void STD__code__package(ANVIL__workspace* workspace, STD__offsets* standard_offsets) {
    // write functions
    STD__code__print_binary(workspace, standard_offsets);
    STD__code__print_buffer_as_string(workspace, standard_offsets);

    return;
}

#endif
