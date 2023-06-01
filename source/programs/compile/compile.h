#ifndef ANVIL__compile
#define ANVIL__compile

/* Notes */
/*
    Lexing:
        Two pass:
            1) Count lexlings and determine validity.
            2) If first pass had no errors, record lexlings.

    Parsing:
    The program that reads the parser must be able to traverse through configuration numbers that it doesn't want.

    Abstracting:
    Abstracting is not the process of marking up the syntax tree, it is the process of representing program flow with a tree.
*/

/* Include */
// anvil
#include "../essentials.hpp"

/* Define */

/* Offsets */
// offset type
typedef enum COMPILE__ot {
    // offsets
    COMPILE__ot__package_start,

    // manager
    COMPILE__ot__compile__start,

    // lexing
    COMPILE__ot__lex__start,
    COMPILE__ot__lex__count__loop_start,
    COMPILE__ot__lex__record__loop_start,

    // count
    COMPILE__ot__COUNT,
} COMPILE__ot;

// offsets
typedef struct COMPILE__offsets {
    ANVIL__offset offsets[COMPILE__ot__COUNT];
} COMPILE__offsets;

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
