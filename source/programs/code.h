#ifndef ANVIL__code
#define ANVIL__code

/* Include */
// anvil
#include "essentials.hpp"

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
// blueprintling
typedef ANVIL__u64 CODE__blueprintling;

// blueprint type
typedef enum CODE__bt {
	CODE__bt__start_instruction,
	CODE__bt__mark__flag_ID,
	CODE__bt__mark__operation_ID,
	CODE__bt__mark__register_ID,
	CODE__bt__mark__register,
	CODE__bt__COUNT,
} CODE__bt;

// register types
typedef enum CODE__write_instruction {
	// preserve start
	CODE__write_instruction__preserve__START = ANVIL__srt__start__workspace,

	// variables
	CODE__write_instruction__write_to = CODE__write_instruction__preserve__START,
	CODE__write_instruction__program_offset,
	CODE__write_instruction__pass,
	CODE__write_instruction__data_byte_size,
	CODE__write_instruction__data,
	CODE__write_instruction__write_to,
	CODE__write_instruction__program_offset,

	// preserve end
	CODE__write_instruction__preserve__END,

	// inputs
	CODE__write_instruction__input__write_to = ANVIL__srt__start__function_io,
	CODE__write_instruction__input__program_offset,
	CODE__write_instruction__input__pass,
	CODE__write_instruction__input__data_byte_size,
	CODE__write_instruction__input__data,

	// outputs
	CODE__write_instruction__output__write_to = ANVIL__srt__start__function_io,
	CODE__write_instruction__output__program_offset,
} CODE__write_instruction;

// call function
void CODE__code__call__write_instruction(ANVIL__workspace* workspace, CODE__offsets* code_offsets, ANVIL__flag_ID flag, ANVIL__register_ID input__write_to, ANVIL__register_ID input__program_offset, ANVIL__register_ID input__pass, ANVIL__register_ID input__data_byte_size, ANVIL__register_ID input__data, ANVIL__register_ID output__write_to, ANVIL__register_ID output__program_offset) {
	// pass inputs
	ANVIL__code__register_to_register(workspace, flag, input__write_to, CODE__write_instruction__input__write_to);
	ANVIL__code__register_to_register(workspace, flag, input__program_offset, CODE__write_instruction__input__program_offset);
	ANVIL__code__register_to_register(workspace, flag, input__pass, CODE__write_instruction__input__pass);
	ANVIL__code__register_to_register(workspace, flag, input__data_byte_size, CODE__write_instruction__input__data_byte_size);
	ANVIL__code__register_to_register(workspace, flag, input__data, CODE__write_instruction__input__data);

	// call function
	ANVIL__code__call__static(workspace, flag, (*code_offsets).offsets[CODE__ot__write_instruction__start]);

	// pass outputs
	ANVIL__code__register_to_register(workspace, flag, CODE__write_instruction__output__write_to, output__write_to);
	ANVIL__code__register_to_register(workspace, flag, CODE__write_instruction__output__program_offset, output__program_offset);
}

// build function
void CODE__code__write_instruction(ANVIL__workspace* workspace, CODE__offsets* code_offsets) {
	CODE__blueprintling blueprint[] = {
		// stop
		CODE__bt__start_instruction,

		// write register
		CODE__bt__start_instruction,
			CODE__bt__mark__register,
			CODE__bt__mark__register_ID,
		
		// 
	};

	// setup function offset
	(*code_offsets).offsets[CODE__ot__write_instruction__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, CODE__write_instruction__preserve__START, CODE__write_instruction__preserve__END);

	// get inputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__input__write_to, CODE__write_instruction__write_to);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__input__program_offset, CODE__write_instruction__program_offset);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__input__pass, CODE__write_instruction__pass);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__input__data_byte_size, CODE__write_instruction__data_byte_size);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__input__data, CODE__write_instruction__data);

	// code here


	// setup outputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__output__write_to, CODE__write_instruction__write_to);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_instruction__output__program_offset, CODE__write_instruction__program_offset);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, CODE__write_instruction__preserve__START, CODE__write_instruction__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}


/* Build Package */
void CODE__code__package(ANVIL__workspace* workspace, CODE__offsets* code_offsets, ESS__offsets* essential_offsets) {
    // write functions
    // TODO

    return;
}

#endif
