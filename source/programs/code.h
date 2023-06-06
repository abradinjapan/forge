#ifndef ANVIL__code
#define ANVIL__code

/* Include */
// anvil
#include "essentials.h"

/* Offsets */
// offset type
typedef enum CODE__ot {
    // offsets
    CODE__ot__package_start,

    // write instructions
    CODE__ot__write_instruction__start,

    // count
    CODE__ot__COUNT = CODE__ot__write_instruction__start + ANVIL__it__COUNT,
} CODE__ot;

// offsets
typedef struct CODE__offsets {
    ANVIL__offset offsets[CODE__ot__COUNT];
} CODE__offsets;

/* Write Instruction Scraplets */
// code instruction ID
void CODE__code__write__instruction_ID(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write, ANVIL__register_ID instruction_ID) {
    // write byte
    ANVIL__code__write_register(workspace, (ANVIL__register)(sizeof(ANVIL__instruction_ID) * ANVIL__define__bits_in_byte), ANVIL__srt__temp__bit_count);
    ESS__code__call__write_byte(workspace, essential_offsets, ANVIL__sft__always_run, io_current_address, ANVIL__srt__temp__bit_count, do_write, instruction_ID, io_current_address);

    return;
}

// code flag ID
void CODE__code__write__flag_ID(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write, ANVIL__register_ID flag_ID) {
    // write byte
    ANVIL__code__write_register(workspace, (ANVIL__register)(sizeof(ANVIL__flag_ID) * ANVIL__define__bits_in_byte), ANVIL__srt__temp__bit_count);
    ESS__code__call__write_byte(workspace, essential_offsets, ANVIL__sft__always_run, io_current_address, ANVIL__srt__temp__bit_count, do_write, flag_ID, io_current_address);

    return;
}

// code operation ID
void CODE__code__write__operation_ID(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write, ANVIL__register_ID operation_ID) {
    // write byte
    ANVIL__code__write_register(workspace, (ANVIL__register)(sizeof(ANVIL__operation_ID) * ANVIL__define__bits_in_byte), ANVIL__srt__temp__bit_count);
    ESS__code__call__write_byte(workspace, essential_offsets, ANVIL__sft__always_run, io_current_address, ANVIL__srt__temp__bit_count, do_write, operation_ID, io_current_address);

    return;
}

// code register ID
void CODE__code__write__register_ID(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write, ANVIL__register_ID register_ID) {
    // write byte
    ANVIL__code__write_register(workspace, (ANVIL__register)(sizeof(ANVIL__register_ID) * ANVIL__define__bits_in_byte), ANVIL__srt__temp__bit_count);
    ESS__code__call__write_byte(workspace, essential_offsets, ANVIL__sft__always_run, io_current_address, ANVIL__srt__temp__bit_count, do_write, register_ID, io_current_address);

    return;
}

// code register
void CODE__code__write__register(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write, ANVIL__register_ID register_value) {
    // write bytes
    ANVIL__code__write_register(workspace, (ANVIL__register)(sizeof(ANVIL__register) * ANVIL__define__bits_in_byte), ANVIL__srt__temp__bit_count);
    ESS__code__call__write_byte(workspace, essential_offsets, ANVIL__sft__always_run, io_current_address, ANVIL__srt__temp__bit_count, do_write, register_value, io_current_address);

    return;
}

/* Write Instructions */
// code instruction stop
void CODE__code__instruction__stop(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write) {
	// write instruction ID
	ANVIL__code__write_register(workspace, (ANVIL__register)ANVIL__it__stop, ANVIL__srt__temp__write);
	CODE__code__write__instruction_ID(workspace, essential_offsets, io_current_address, do_write, ANVIL__srt__temp__write);

	return;
}

// code instruction write register
void CODE__code__instruction__write_register(ANVIL__workspace* workspace, ESS__offsets* essential_offsets, ANVIL__register_ID io_current_address, ANVIL__register_ID do_write, ANVIL__register_ID register_value, ANVIL__register_ID destination) {
	// write instruction ID
	ANVIL__code__write_register(workspace, (ANVIL__register)ANVIL__it__write_register, ANVIL__srt__temp__write);
	CODE__code__write__instruction_ID(workspace, essential_offsets, io_current_address, do_write, ANVIL__srt__temp__write);

	// write instruction parameters
	CODE__code__write__register(workspace, essential_offsets, io_current_address, do_write, register_value);
	CODE__code__write__register_ID(workspace, essential_offsets, io_current_address, do_write, destination);

	return;
}

/* Passes */
// setup pass get data
void CODE__code__start_pass__get_data(ANVIL__workspace* workspace, ANVIL__register_ID do_write, ANVIL__register_ID write_to) {
    // setup variables
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__false, do_write);
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, write_to);

    return;
}

// setup pass write program
void CODE__code__start_pass__write_program(ANVIL__workspace* workspace, ANVIL__register_ID do_write, ANVIL__register_ID write_to) {
    // setup variables
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__true, do_write);
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, write_to, write_to);

    return;
}

/* Build Package */
void CODE__code__package(ANVIL__workspace* workspace, CODE__offsets* code_offsets, ESS__offsets* essential_offsets) {
    // write functions
    // TODO

    return;
}

#endif
