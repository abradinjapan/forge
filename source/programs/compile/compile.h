#ifndef ANVIL__compile
#define ANVIL__compile

/* Notes */
/*
    Parsing:
    The program that reads the parser must be able to traverse through configuration numbers that it doesn't want.

    Abstracting:
    Abstracting is not the process of marking up the syntax tree, it is the process of representing program flow with a tree.

    Note:
    Somehow, the blueprints for abstracting and parsing must be joined together.
    There are two tasks here:
        1) Extracting and validating the grammar.
        2) Extracting and validating the actions that the code does.
    
    Code Generation:
        What if instead of generating a parse tree blueprint inline, that if I generated the blueprint with code to make it more featureful.
            As in, offsets point to other parse nodes.
*/

/* Include */
// anvil
#include "../essentials.h"

/* Define */


/* Offsets */
// offset type
typedef enum COMPILE__ot {
    // manager
    COMPILE__ot__compile__start,

    // representer
    COMPILE__ot__represent__start,

    // count
    COMPILE__ot__COUNT,
} COMPILE__ot;

// offsets
typedef struct COMPILE__offsets {
    ANVIL__offset offsets[COMPILE__ot__COUNT];
    ANVIL__buffer representation;
} COMPILE__offsets;

/* Represent */
// register types
typedef enum COMPILE__represent {
	// preserve start
	COMPILE__represent__preserve__START = ANVIL__srt__start__workspace,

	// variables
	COMPILE__represent__subject_buffer_current = COMPILE__represent__preserve__START,
	COMPILE__represent__subject_buffer_end,
	COMPILE__represent__result_buffer_current,
	COMPILE__represent__result_buffer_end,
	COMPILE__represent__subject_character,
	COMPILE__represent__character_range_start,
	COMPILE__represent__character_range_end,
	COMPILE__represent__current_configuration,

	// preserve end
	COMPILE__represent__preserve__END,

	// inputs
	COMPILE__represent__input__subject_buffer_current = ANVIL__srt__start__function_io,
	COMPILE__represent__input__subject_buffer_end,
	COMPILE__represent__input__result_buffer_current,
	COMPILE__represent__input__result_buffer_end,

	// outputs
	COMPILE__represent__output__result_buffer_current = ANVIL__srt__start__function_io,
	COMPILE__represent__output__subject_buffer_current,
} COMPILE__represent;

// call function
void COMPILE__code__call__represent(ANVIL__workspace* workspace, COMPILE__offsets* compile_offsets, ANVIL__flag_ID flag, ANVIL__register_ID input__subject_buffer_current, ANVIL__register_ID input__subject_buffer_end, ANVIL__register_ID input__result_buffer_current, ANVIL__register_ID input__result_buffer_end, ANVIL__register_ID output__result_buffer_current, ANVIL__register_ID output__subject_buffer_current) {
	// pass inputs
	ANVIL__code__register_to_register(workspace, flag, input__subject_buffer_current, COMPILE__represent__input__subject_buffer_current);
	ANVIL__code__register_to_register(workspace, flag, input__subject_buffer_end, COMPILE__represent__input__subject_buffer_end);
	ANVIL__code__register_to_register(workspace, flag, input__result_buffer_current, COMPILE__represent__input__result_buffer_current);
	ANVIL__code__register_to_register(workspace, flag, input__result_buffer_end, COMPILE__represent__input__result_buffer_end);

	// call function
	ANVIL__code__call__static(workspace, flag, (*compile_offsets).offsets[COMPILE__ot__represent__start]);

	// pass outputs
	ANVIL__code__register_to_register(workspace, flag, COMPILE__represent__output__result_buffer_current, output__result_buffer_current);
	ANVIL__code__register_to_register(workspace, flag, COMPILE__represent__output__subject_buffer_current, output__subject_buffer_current);
}

// build function
void COMPILE__code__represent(ANVIL__workspace* workspace, COMPILE__offsets* compile_offsets) {
	// setup function offset
	(*compile_offsets).offsets[COMPILE__ot__represent__start] = ANVIL__get__offset(workspace);

	// setup function prologue
	ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, COMPILE__represent__preserve__START, COMPILE__represent__preserve__END);

	// get inputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, COMPILE__represent__input__subject_buffer_current, COMPILE__represent__subject_buffer_current);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, COMPILE__represent__input__subject_buffer_end, COMPILE__represent__subject_buffer_end);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, COMPILE__represent__input__result_buffer_current, COMPILE__represent__result_buffer_current);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, COMPILE__represent__input__result_buffer_end, COMPILE__represent__result_buffer_end);

	// code here


	// setup outputs
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, COMPILE__represent__result_buffer_current, COMPILE__represent__output__result_buffer_current);
	ANVIL__code__register_to_register(workspace, ANVIL__sft__always_run, COMPILE__represent__subject_buffer_current, COMPILE__represent__output__subject_buffer_current);

	// setup function epilogue
	ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, COMPILE__represent__preserve__START, COMPILE__represent__preserve__END);

	// return to caller
	ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

	return;
}

/* Compile */
// register types
typedef enum COMPILE__compile {
    // preserve start
    COMPILE__compile__preserve__START = ANVIL__srt__start__workspace,

    // variables
    COMPILE__compile__user_code_start = COMPILE__compile__preserve__START,
    COMPILE__compile__user_code_end,

    // preserve end
    COMPILE__compile__preserve__END,
} COMPILE__compile;

// build function
void COMPILE__code__compile(ANVIL__workspace* workspace, COMPILE__offsets* compile_offsets, ESS__offsets* essential_offsets) {
    // setup function offset
    (*compile_offsets).offsets[COMPILE__ot__compile__start] = ANVIL__get__offset(workspace);

    // setup function prologue
    ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, COMPILE__compile__preserve__START, COMPILE__compile__preserve__END);

    // DEBUG inform user of compiler starting
    ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"Compiling:\n", ANVIL__bt__false, ANVIL__bt__false));
    ESS__code__call__print_buffer(workspace, essential_offsets, ANVIL__sft__always_run, ANVIL__srt__input_buffer_start, ANVIL__srt__input_buffer_end);
    ESS__code__quick_print_buffer(workspace, ANVIL__open__buffer_from_string((u8*)"\n\n", ANVIL__bt__false, ANVIL__bt__false));

    // setup function epilogue
    ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, COMPILE__compile__preserve__START, COMPILE__compile__preserve__END);

    // jump to caller
    ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

    return;
}

/* Build Package */
void COMPILE__code__package(ANVIL__workspace* workspace, COMPILE__offsets* compile_offsets, ESS__offsets* essential_offsets) {
    // write functions
    COMPILE__code__compile(workspace, compile_offsets, essential_offsets);

    return;
}

/* Forge Program */
// build program
ANVIL__buffer COMPILE__forge__program() {
    ANVIL__buffer output;
    ANVIL__workspace workspace;
    COMPILE__offsets compile_offsets;
    ESS__offsets essential_offsets;

    // create workspace
    workspace = ANVIL__setup__workspace(&output);

    // create program
    for (ANVIL__pt pass = ANVIL__pt__get_offsets; pass < ANVIL__pt__COUNT; pass = (ANVIL__pt)((u64)pass + 1)) {
        // setup pass
        ANVIL__setup__pass(&workspace, pass);

        // build program
        ANVIL__code__start(&workspace, 1024, compile_offsets.offsets[COMPILE__ot__compile__start]);
        COMPILE__code__package(&workspace, &compile_offsets, &essential_offsets);
        ESS__code__package(&workspace, &essential_offsets);
    }

    return output;
}

#endif
