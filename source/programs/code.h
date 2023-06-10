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

    // write datum
    CODE__ot__write_bytes__start,
    CODE__ot__write_bytes__skip_writing,

    // write instructions
    CODE__ot__write_instruction__start,

    // create context
    CODE__ot__create_context__start,

    // count
    CODE__ot__COUNT = CODE__ot__write_instruction__start + ANVIL__it__COUNT,
} CODE__ot;

// offsets
typedef struct CODE__offsets {
    ANVIL__offset offsets[CODE__ot__COUNT];
} CODE__offsets;

/* Write Bytes */
// register types
typedef enum CODE__write_bytes {
	// preserve start
	CODE__write_bytes__preserve__START = ANVIL__srt__start__workspace,

	// variables
	CODE__write_bytes__write_to = CODE__write_bytes__preserve__START,
	CODE__write_bytes__write_offset,
	CODE__write_bytes__data,
	CODE__write_bytes__byte_size,
	CODE__write_bytes__do_write,
	CODE__write_bytes__new_write_to,
	CODE__write_bytes__new_write_offset,
	CODE__write_bytes__bit_size,

	// preserve end
	CODE__write_bytes__preserve__END,

	// inputs
	CODE__write_bytes__input__write_to = ANVIL__srt__start__function_io,
	CODE__write_bytes__input__write_offset,
	CODE__write_bytes__input__data,
	CODE__write_bytes__input__byte_size,
	CODE__write_bytes__input__do_write,

	// outputs
	CODE__write_bytes__output__new_write_to = ANVIL__srt__start__function_io,
	CODE__write_bytes__output__new_write_offset,
} CODE__write_bytes;

// call function
void CODE__code__call__write_bytes(ANVIL__workspace* workspace, CODE__offsets* code_offsets, ANVIL__flag_ID flag, ANVIL__register_ID input__write_to, ANVIL__register_ID input__write_offset, ANVIL__register_ID input__data, ANVIL__register_ID input__byte_size, ANVIL__register_ID input__do_write, ANVIL__register_ID output__new_write_to, ANVIL__register_ID output__new_write_offset) {
	// pass inputs
	ANVIL__code__register_to_register(workspace, flag, input__write_to, CODE__write_bytes__input__write_to);
	ANVIL__code__register_to_register(workspace, flag, input__write_offset, CODE__write_bytes__input__write_offset);
	ANVIL__code__register_to_register(workspace, flag, input__data, CODE__write_bytes__input__data);
	ANVIL__code__register_to_register(workspace, flag, input__byte_size, CODE__write_bytes__input__byte_size);
	ANVIL__code__register_to_register(workspace, flag, input__do_write, CODE__write_bytes__input__do_write);

	// call function
	ANVIL__code__call__static(workspace, flag, (*code_offsets).offsets[CODE__ot__write_bytes__start]);

	// pass outputs
	ANVIL__code__register_to_register(workspace, flag, CODE__write_bytes__output__new_write_to, output__new_write_to);
	ANVIL__code__register_to_register(workspace, flag, CODE__write_bytes__output__new_write_offset, output__new_write_offset);

	return;
}

// build function
void CODE__code__write_bytes(ANVIL__workspace* workspace, CODE__offsets* code_offsets) {
	// setup function offset
	(*code_offsets).offsets[CODE__ot__write_bytes__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, CODE__write_bytes__preserve__START, CODE__write_bytes__preserve__END);

	// get inputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__input__write_to, CODE__write_bytes__write_to);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__input__write_offset, CODE__write_bytes__write_offset);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__input__data, CODE__write_bytes__data);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__input__byte_size, CODE__write_bytes__byte_size);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__input__do_write, CODE__write_bytes__do_write);

	// skip past writing if writing is disabled
    ANVIL__code__operate__jump__static(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, CODE__write_bytes__do_write, ANVIL__srt__constant__0, ANVIL__sft__never_run, (*code_offsets).offsets[CODE__ot__write_bytes__skip_writing]);

    // setup bit size
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_multiply, CODE__write_bytes__byte_size, ANVIL__srt__constant__bits_in_byte, ANVIL__unused_register_ID, CODE__write_bytes__bit_size);

    // write bytes
    ANVIL__code__register_to_address(workspace, CODE__write_bytes__data, CODE__write_bytes__bit_size, CODE__write_bytes__write_to);

	// setup skip writing offset
	(*code_offsets).offsets[CODE__ot__write_bytes__skip_writing] = ANVIL__get__offset(workspace);

    // advance pointer & offset
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, CODE__write_bytes__write_to, CODE__write_bytes__byte_size, ANVIL__unused_register_ID, CODE__write_bytes__new_write_to);
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, CODE__write_bytes__write_offset, CODE__write_bytes__byte_size, ANVIL__unused_register_ID, CODE__write_bytes__new_write_offset);

	// setup outputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__new_write_to, CODE__write_bytes__output__new_write_to);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__write_bytes__new_write_offset, CODE__write_bytes__output__new_write_offset);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, CODE__write_bytes__preserve__START, CODE__write_bytes__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

/* Write Instruction Scraplets */

/* Passes */
// setup pass get data
void CODE__code__start_pass__get_data(ANVIL__workspace* workspace, ANVIL__register_ID do_write, ANVIL__register_ID write_to) {
    // setup variables
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__false, do_write);
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, write_to);

    return;
}

// setup pass write program
void CODE__code__start_pass__write_program(ANVIL__workspace* workspace, ANVIL__register_ID do_write, ANVIL__register_ID input__write_to, ANVIL__register_ID output__write_to) {
    // setup variables
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__true, do_write);
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, input__write_to, output__write_to);

    return;
}

/* Create Context */
// register types
typedef enum CODE__create_context {
	// preserve start
	CODE__create_context__preserve__START = ANVIL__srt__start__workspace,

	// variables
	CODE__create_context__program_buffer_start = CODE__create_context__preserve__START,
	CODE__create_context__program_buffer_end,
	CODE__create_context__context_buffer_start,
	CODE__create_context__context_buffer_end,
    CODE__create_context__allocation_size,
    CODE__create_context__context_buffer_current,

	// preserve end
	CODE__create_context__preserve__END,

	// inputs
	CODE__create_context__input__program_buffer_start = ANVIL__srt__start__function_io,
	CODE__create_context__input__program_buffer_end,

	// outputs
	CODE__create_context__output__context_buffer_start = ANVIL__srt__start__function_io,
	CODE__create_context__output__context_buffer_end,
} CODE__create_context;

// call function
void CODE__code__call__create_context(ANVIL__workspace* workspace, CODE__offsets* code_offsets, ANVIL__flag_ID flag, ANVIL__register_ID input__program_buffer_start, ANVIL__register_ID input__program_buffer_end, ANVIL__register_ID output__context_buffer_start, ANVIL__register_ID output__context_buffer_end) {
	// pass inputs
	ANVIL__code__register_to_register(workspace, flag, input__program_buffer_start, CODE__create_context__input__program_buffer_start);
	ANVIL__code__register_to_register(workspace, flag, input__program_buffer_end, CODE__create_context__input__program_buffer_end);

	// call function
	ANVIL__code__call__static(workspace, flag, (*code_offsets).offsets[CODE__ot__create_context__start]);

	// pass outputs
	ANVIL__code__register_to_register(workspace, flag, CODE__create_context__output__context_buffer_start, output__context_buffer_start);
	ANVIL__code__register_to_register(workspace, flag, CODE__create_context__output__context_buffer_end, output__context_buffer_end);

	return;
}

// build function
void CODE__code__create_context(ANVIL__workspace* workspace, CODE__offsets* code_offsets) {
	// setup function offset
	(*code_offsets).offsets[CODE__ot__create_context__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, CODE__create_context__preserve__START, CODE__create_context__preserve__END);

	// get inputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__create_context__input__program_buffer_start, CODE__create_context__program_buffer_start);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__create_context__input__program_buffer_end, CODE__create_context__program_buffer_end);

	// setup allocation size
    ANVIL__code__write_register(workspace, (ANVIL__register)sizeof(ANVIL__context), CODE__ot__create_context__start);

    // allocate
    ANVIL__code__request_memory(workspace, CODE__ot__create_context__start, CODE__create_context__context_buffer_start, CODE__create_context__context_buffer_end);

    // setup writing to context
    ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__create_context__context_buffer_start, CODE__create_context__context_buffer_current);

    // initialize context
    {
        // initialize program buffer start
        ANVIL__code__register_to_address(workspace, CODE__create_context__program_buffer_start, ANIVL__srt__constant__register_bit_count, CODE__create_context__context_buffer_current);
        ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, CODE__create_context__context_buffer_current, ANIVL__srt__constant__register_byte_count, ANVIL__unused_register_ID, CODE__create_context__context_buffer_current);
        
        // initialize program buffer current
        ANVIL__code__register_to_address(workspace, CODE__create_context__program_buffer_start, ANIVL__srt__constant__register_bit_count, CODE__create_context__context_buffer_current);
        ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, CODE__create_context__context_buffer_current, ANIVL__srt__constant__register_byte_count, ANVIL__unused_register_ID, CODE__create_context__context_buffer_current);
        
        // initialize program buffer end
        ANVIL__code__register_to_address(workspace, CODE__create_context__program_buffer_end, ANIVL__srt__constant__register_bit_count, CODE__create_context__context_buffer_current);
        ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, CODE__create_context__context_buffer_current, ANIVL__srt__constant__register_byte_count, ANVIL__unused_register_ID, CODE__create_context__context_buffer_current);
    }

	// setup outputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__create_context__context_buffer_start, CODE__create_context__output__context_buffer_start);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, CODE__create_context__context_buffer_end, CODE__create_context__output__context_buffer_end);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, CODE__create_context__preserve__START, CODE__create_context__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

/* Build Package */
void CODE__code__package(ANVIL__workspace* workspace, CODE__offsets* code_offsets) {
    // write functions
    CODE__code__write_bytes(workspace, code_offsets);
    CODE__code__create_context(workspace, code_offsets);

    return;
}

#endif
