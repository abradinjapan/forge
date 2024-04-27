#ifndef FROST__standard
#define FROST__standard

/* Include */
// anvil
//#include "../programs/essentials.h"
#include "../anvil/anvil.h"

/* Offsets */
// offset type
typedef enum STD__ot {
    // offsets
    STD__ot__package_start,

    // translation functions
    STD__ot__calculate_decimal_digit_count__start,
    STD__ot__calculate_decimal_digit_count__loop_start,
    STD__ot__calculate_decimal_digit_count__return,
    STD__ot__cell_to_unsigned_integer_string__start,
    STD__ot__cell_to_unsigned_integer_string__loop_start,
    STD__ot__cell_to_unsigned_integer_string__return,

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

/* Translation Functions */
// cell types
typedef enum STD__calculate_decimal_digit_count {
	// preserve start
	STD__calculate_decimal_digit_count__preserve__START = ANVIL__srt__start__workspace,

	// variables
	STD__calculate_decimal_digit_count__value = STD__calculate_decimal_digit_count__preserve__START,
	STD__calculate_decimal_digit_count__digit_count,
	STD__calculate_decimal_digit_count__zero,
	STD__calculate_decimal_digit_count__ten,

	// preserve end
	STD__calculate_decimal_digit_count__preserve__END,

	// inputs
	STD__calculate_decimal_digit_count__input__value = ANVIL__srt__start__function_io,

	// outputs
	STD__calculate_decimal_digit_count__output__digit_count = ANVIL__srt__start__function_io,
} STD__calculate_decimal_digit_count;

// call function
void STD__code__call__calculate_decimal_digit_count(ANVIL__workspace* workspace, STD__offsets* std_offsets, ANVIL__flag_ID flag, ANVIL__cell_ID input__value, ANVIL__cell_ID output__digit_count) {
	// pass inputs
	ANVIL__code__cell_to_cell(workspace, flag, input__value, STD__calculate_decimal_digit_count__input__value);

	// call function
	ANVIL__code__call__static(workspace, flag, (*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__start]);

	// pass outputs
	ANVIL__code__cell_to_cell(workspace, flag, STD__calculate_decimal_digit_count__output__digit_count, output__digit_count);

	return;
}

// build function
void STD__code__calculate_decimal_digit_count(ANVIL__workspace* workspace, STD__offsets* std_offsets) {
	// setup function offset
	(*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, STD__calculate_decimal_digit_count__preserve__START, STD__calculate_decimal_digit_count__preserve__END);

	// get inputs
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__calculate_decimal_digit_count__input__value, STD__calculate_decimal_digit_count__value);

    // check for zero
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, STD__calculate_decimal_digit_count__zero);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)1, STD__calculate_decimal_digit_count__digit_count);
    ANVIL__code__operate__jump__static(workspace, ANVIL__sft__always_run, STD__calculate_decimal_digit_count__zero, STD__calculate_decimal_digit_count__value, STD__calculate_decimal_digit_count__zero, ANVIL__sft__never_run, (*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__return]);

    // not a zero, set to default
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, STD__calculate_decimal_digit_count__digit_count);

    // setup remaining variables
    ANVIL__code__write_cell(workspace, (ANVIL__cell)10, STD__calculate_decimal_digit_count__ten);

	// setup loop start
	(*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__loop_start] = ANVIL__get__offset(workspace);

    // check for remaining digits
    ANVIL__code__operate__jump__static(workspace, ANVIL__sft__always_run, STD__calculate_decimal_digit_count__zero, STD__calculate_decimal_digit_count__value, STD__calculate_decimal_digit_count__zero, ANVIL__sft__never_run, (*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__return]);

    // perform divide
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_division, STD__calculate_decimal_digit_count__value, STD__calculate_decimal_digit_count__ten, ANVIL__unused_cell_ID, STD__calculate_decimal_digit_count__value);

    // increment count
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, STD__calculate_decimal_digit_count__digit_count, ANVIL__srt__constant__1, ANVIL__unused_cell_ID, STD__calculate_decimal_digit_count__digit_count);

    // jump to beginning
    ANVIL__code__jump__static(workspace, ANVIL__sft__always_run, (*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__loop_start]);

	// setup loop return
	(*std_offsets).offsets[STD__ot__calculate_decimal_digit_count__return] = ANVIL__get__offset(workspace);

	// setup outputs
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__calculate_decimal_digit_count__digit_count, STD__calculate_decimal_digit_count__output__digit_count);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, STD__calculate_decimal_digit_count__preserve__START, STD__calculate_decimal_digit_count__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

// cell types
typedef enum STD__cell_to_unsigned_integer_string {
	// preserve start
	STD__cell_to_unsigned_integer_string__preserve__START = ANVIL__srt__start__workspace,

	// variables
	STD__cell_to_unsigned_integer_string__value = STD__cell_to_unsigned_integer_string__preserve__START,
	STD__cell_to_unsigned_integer_string__number_start,
	STD__cell_to_unsigned_integer_string__number_end,
	STD__cell_to_unsigned_integer_string__current,
	STD__cell_to_unsigned_integer_string__digit_count,
	STD__cell_to_unsigned_integer_string__character,
	STD__cell_to_unsigned_integer_string__ten,
	STD__cell_to_unsigned_integer_string__base_digit_character,

	// preserve end
	STD__cell_to_unsigned_integer_string__preserve__END,

	// inputs
	STD__cell_to_unsigned_integer_string__input__value = ANVIL__srt__start__function_io,

	// outputs
	STD__cell_to_unsigned_integer_string__output__number_start = ANVIL__srt__start__function_io,
	STD__cell_to_unsigned_integer_string__output__number_end,
} STD__cell_to_unsigned_integer_string;

// call function
void STD__code__call__cell_to_unsigned_integer_string(ANVIL__workspace* workspace, STD__offsets* std_offsets, ANVIL__flag_ID flag, ANVIL__cell_ID input__value, ANVIL__cell_ID output__number_start, ANVIL__cell_ID output__number_end) {
	// pass inputs
	ANVIL__code__cell_to_cell(workspace, flag, input__value, STD__cell_to_unsigned_integer_string__input__value);

	// call function
	ANVIL__code__call__static(workspace, flag, (*std_offsets).offsets[STD__ot__cell_to_unsigned_integer_string__start]);

	// pass outputs
	ANVIL__code__cell_to_cell(workspace, flag, STD__cell_to_unsigned_integer_string__output__number_start, output__number_start);
	ANVIL__code__cell_to_cell(workspace, flag, STD__cell_to_unsigned_integer_string__output__number_end, output__number_end);

	return;
}

// build function
void STD__code__cell_to_unsigned_integer_string(ANVIL__workspace* workspace, STD__offsets* std_offsets) {
	// setup function offset
	(*std_offsets).offsets[STD__ot__cell_to_unsigned_integer_string__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__preserve__START, STD__cell_to_unsigned_integer_string__preserve__END);

	// get inputs
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__input__value, STD__cell_to_unsigned_integer_string__value);

	// setup variables
	ANVIL__code__write_cell(workspace, (ANVIL__cell)10, STD__cell_to_unsigned_integer_string__ten);
	ANVIL__code__write_cell(workspace, (ANVIL__cell)'0', STD__cell_to_unsigned_integer_string__base_digit_character);

	// calculate digit count
	STD__code__call__calculate_decimal_digit_count(workspace, std_offsets, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__value, STD__cell_to_unsigned_integer_string__digit_count);

	// allocate output
	ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_multiply, ANVIL__srt__constant__ascii_character_byte_size, STD__cell_to_unsigned_integer_string__digit_count, ANVIL__unused_cell_ID, ANVIL__srt__temp__length);
	ANVIL__code__request_memory(workspace, ANVIL__srt__temp__length, STD__cell_to_unsigned_integer_string__number_start, STD__cell_to_unsigned_integer_string__number_end);

	// init to zero
	ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__base_digit_character, ANVIL__srt__constant__ascii_character_bit_size, STD__cell_to_unsigned_integer_string__number_start);

	// convert to string
	// setup current
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__number_end, STD__cell_to_unsigned_integer_string__current);

	// setup loop start offset
	std_offsets->offsets[STD__ot__cell_to_unsigned_integer_string__loop_start] = ANVIL__get__offset(workspace);

	// check for loop end
	ANVIL__code__operate__jump__static(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__number_start, STD__cell_to_unsigned_integer_string__current, STD__cell_to_unsigned_integer_string__number_end, ANVIL__sft__always_run, std_offsets->offsets[STD__ot__cell_to_unsigned_integer_string__return]);

	// calculate character
	ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_modulous, STD__cell_to_unsigned_integer_string__value, STD__cell_to_unsigned_integer_string__ten, ANVIL__unused_cell_ID, STD__cell_to_unsigned_integer_string__character);
	ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, STD__cell_to_unsigned_integer_string__base_digit_character, STD__cell_to_unsigned_integer_string__character, ANVIL__unused_cell_ID, STD__cell_to_unsigned_integer_string__character);

	// write character
	ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__character, ANVIL__srt__constant__ascii_character_bit_size, STD__cell_to_unsigned_integer_string__current);

	// next character
	ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_division, STD__cell_to_unsigned_integer_string__value, STD__cell_to_unsigned_integer_string__ten, ANVIL__unused_cell_ID, STD__cell_to_unsigned_integer_string__value);
	ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_subtract, STD__cell_to_unsigned_integer_string__current, ANVIL__srt__constant__ascii_character_byte_size, ANVIL__unused_cell_ID, STD__cell_to_unsigned_integer_string__current);
	ANVIL__code__jump__static(workspace, ANVIL__sft__always_run, std_offsets->offsets[STD__ot__cell_to_unsigned_integer_string__loop_start]);

	// setup return
	std_offsets->offsets[STD__ot__cell_to_unsigned_integer_string__return] = ANVIL__get__offset(workspace);

	// setup outputs
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__number_start, STD__cell_to_unsigned_integer_string__output__number_start);
	ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__number_end, STD__cell_to_unsigned_integer_string__output__number_end);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, STD__cell_to_unsigned_integer_string__preserve__START, STD__cell_to_unsigned_integer_string__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

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
	STD__code__calculate_decimal_digit_count(workspace, standard_offsets);
	STD__code__cell_to_unsigned_integer_string(workspace, standard_offsets);

    return;
}

#endif
