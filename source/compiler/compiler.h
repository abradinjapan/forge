#ifndef FROST__compiler
#define FROST__compiler

/* Include */
// anvil / basic
#include "../anvil/anvil.h"
#include "standard.h"

/* Define */
// basics
typedef ANVIL__u64 COMP__tab_count;

// aliases
typedef ANVIL__buffer COMP__current;

// tracking types
typedef ANVIL__u64 COMP__file_index;
typedef ANVIL__u64 COMP__line_number;
typedef ANVIL__u64 COMP__character_index;

// lexing types
typedef ANVIL__u8 COMP__lexling_type;
typedef ANVIL__address COMP__lexling_address;
typedef COMP__lexling_address COMP__lexling_start;
typedef COMP__lexling_address COMP__lexling_end;
typedef ANVIL__u64 COMP__lexling_index;
typedef ANVIL__u64 COMP__lexling_depth; // used for comments and strings

// parsing types
typedef ANVIL__u64 COMP__argument_index;

// accounting types
typedef ANVIL__u64 COMP__io_count;
typedef COMP__io_count COMP__input_count;
typedef COMP__io_count COMP__output_count;
typedef ANVIL__u64 COMP__accountling_index;
typedef COMP__accountling_index COMP__variable_index;
typedef COMP__accountling_index COMP__call_index;
typedef COMP__accountling_index COMP__offset_index;
typedef COMP__accountling_index COMP__flag_index;
typedef COMP__accountling_index COMP__statement_index;
typedef COMP__accountling_index COMP__header_index;
typedef COMP__accountling_index COMP__string_index;

// generation types
typedef ANVIL__u64 COMP__abstraction_index;

// defines
#define COMP__define__null_offset_ID -1
#define COMP__define__null_call_ID -1
#define COMP__define__u64_max_base_10_conversion_character_limit 20
#define COMP__define__invalid_abstraction_call_ID -1

char* COMP__global__predefined_cell_name_strings[] = {
    "frost.error_code",
};
char* COMP__global__predefined_flag_name_strings[] = {
    "frost.always",
    "frost.never",
    "frost.temporary",
};
char* COMP__global__argument_type_name_strings[] = {
    "invalid",
    "variable",
    "input_variable",
    "output_variable",
    "double_variable",
    "body_variable",
    "predefined_variable",
    "offset",
    "flag",
    "flag_user_defined",
    "flag_predefined",
    "boolean",
    "binary",
    "integer",
    "hexadecimal",
    "string",
};
char* COMP__global__accountling_call_type_name_strings[] = {
    "frost.set",
    "frost.print.integer.signed",
    "frost.print.integer.unsigned",
    "frost.print.character",
    "frost.print.buffer_as_string",
    "frost.print.binary",
    "frost.io.cell_to_address",
    "frost.io.address_to_cell",
    "frost.io.file_to_buffer",
    "frost.io.buffer_to_file",
    "frost.copy",
    "frost.copy.buffer",
    "frost.memory.request",
    "frost.memory.return",
    "frost.buffer.calculate_length",
    "frost.cast.cell_to_unsigned_integer_string",
    "frost.integer.add",
    "frost.integer.subtract",
    "frost.integer.multiply",
    "frost.integer.divide",
    "frost.integer.modulous",
    "frost.integer.within_range",
    "frost.binary.or",
    "frost.binary.invert",
    "frost.binary.and",
    "frost.binary.xor",
    "frost.binary.shift_higher",
    "frost.binary.shift_lower",
    "frost.binary.overwrite",
    "frost.flag.get",
    "frost.flag.set",
    "frost.flag.invert",
    "frost.flag.or",
    "frost.flag.and",
    "frost.flag.xor",
    "frost.jump",
    "frost.reset.error_code",
};

// program stage type
typedef enum COMP__pst {
    // stages
    COMP__pst__invalid, // invalid
    COMP__pst__lexing, // lexing files
    COMP__pst__parsing, // parsing files
    COMP__pst__accounting, // semantics for all files
    COMP__pst__generating, // building program
    COMP__pst__running, // running program

    // count
    COMP__pst__COUNT,
} COMP__pst;

/* Essentials */
ANVIL__u64 COMP__calculate__exponent(ANVIL__u64 base, ANVIL__u64 exponent) {
    ANVIL__u64 output = 1;

    // if zero
    if (exponent == 0) {
        return output;
    }

    // calculate number
    while (exponent > 0) {
        // multiply
        output *= base;

        // next exponent
        exponent--;
    }

    return output;
}

// parsing buffer location
typedef struct COMP__character_location {
    COMP__file_index file_index;
    COMP__line_number line_number;
    COMP__character_index character_index;
} COMP__character_location;

// create custom character marker location
COMP__character_location COMP__create__character_location(COMP__file_index file_index, COMP__line_number line_number, COMP__character_index character_index) {
    COMP__character_location output;

    output.file_index = file_index;
    output.line_number = line_number;
    output.character_index = character_index;

    return output;
}

// create null character location
COMP__character_location COMP__create_null__character_location() {
    return COMP__create__character_location(-1, -1, -1);
}

// print tabs
void COMP__print__tabs(COMP__tab_count tab_count) {
    // print tabs
    while (tab_count > 0) {
        // print tab
        printf("\t");

        // next tab
        tab_count--;
    }

    return;
}

/* Translation */
// calculate the amount of digits in a decimal number
ANVIL__length COMP__calculate__decimals_digit_count_in_number(ANVIL__u64 number) {
    ANVIL__length output = 0;

    // check for zero
    if (number == 0) {
        return 1;
    }

    // while there are digits left
    while (number > 0) {
        // divide by ten
        number /= 10;

        // increment digit count
        output++;
    }

    return output;
}

// create buffer string from number
ANVIL__buffer COMP__translate__integer_value_to_string(ANVIL__u64 number) {
    ANVIL__buffer output;
    ANVIL__length digit_count;

    // count digits
    digit_count = COMP__calculate__decimals_digit_count_in_number(number);
    
    // allocate output
    output = ANVIL__open__buffer(sizeof(ANVIL__character) * digit_count);

    // if allocation succeded
    if (ANVIL__check__empty_buffer(output) == ANVIL__bt__false) {
        // calculate characters
        for (ANVIL__u64 i = digit_count; i > 0; i--) {
            // create character
            ((ANVIL__character*)output.start)[i - 1] = '0' + (number % 10);

            // next character
            number /= 10;
        }
    }

    return output;
}

/* Error */
// error information
typedef struct COMP__error {
    ANVIL__bt occured;
    ANVIL__buffer message;
    COMP__character_location location;
    ANVIL__bt memory_error_occured;
} COMP__error;

// create custom error
COMP__error COMP__create__error(ANVIL__bt occured, ANVIL__buffer message, COMP__character_location location, ANVIL__bt memory_error_occured) {
    COMP__error output;

    // setup output
    output.occured = occured;
    output.message = message;
    output.location = location;
    output.memory_error_occured = memory_error_occured;

    return output;
}

// create null error
COMP__error COMP__create_null__error() {
    // return empty
    return COMP__create__error(ANVIL__bt__false, ANVIL__create_null__buffer(), COMP__create_null__character_location(), ANVIL__bt__false);
}

// open a specific error
COMP__error COMP__open__error(const char* message, COMP__character_location location) {
    return COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)message, ANVIL__bt__true, ANVIL__bt__false), location, ANVIL__bt__false);
}

// open a generic memory allocation error
COMP__error COMP__open__internal_memory_error() {
    return COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Internal memory error.", ANVIL__bt__true, ANVIL__bt__false), COMP__create_null__character_location(), ANVIL__bt__true);
}

// create an error report in json
ANVIL__buffer COMP__serialize__error_json(COMP__error error, ANVIL__bt* error_occured) {
    ANVIL__buffer output;
    ANVIL__list json;
    ANVIL__buffer temp_buffer;

    // initialize json string builder
    json = ANVIL__open__list(sizeof(ANVIL__character) * 2048, error_occured);

    // generate json
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"{\n\t\"message\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    ANVIL__list__append__buffer_data(&json, error.message, error_occured);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\",\n\t\"file_index\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    temp_buffer = COMP__translate__integer_value_to_string(error.location.file_index);
    ANVIL__list__append__buffer_data(&json, temp_buffer, error_occured);
    ANVIL__close__buffer(temp_buffer);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\",\n\t\"line_number\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    temp_buffer = COMP__translate__integer_value_to_string(error.location.line_number);
    ANVIL__list__append__buffer_data(&json, temp_buffer, error_occured);
    ANVIL__close__buffer(temp_buffer);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\",\n\t\"character_index\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    temp_buffer = COMP__translate__integer_value_to_string(error.location.character_index);
    ANVIL__list__append__buffer_data(&json, temp_buffer, error_occured);
    ANVIL__close__buffer(temp_buffer);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\"\n}\n", ANVIL__bt__false, ANVIL__bt__false), error_occured);

    // create buffer from list
    output = ANVIL__list__open_buffer_from_list(&json, error_occured);

    // clean up list
    ANVIL__close__list(json);

    return output;
}

// check to see if an error occured
ANVIL__bt COMP__check__error_occured(COMP__error* error) {
    // check for memory error
    if ((*error).memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__open__internal_memory_error();
    }

    return (*error).occured;
}

// close an error
void COMP__close__error(COMP__error error) {
    // clean up buffers
    ANVIL__close__buffer(error.message);
    
    return;
}

/* Abstracted Anvil Functions */
// open a list but the error is a compiler error
ANVIL__list COMP__open__list(ANVIL__list_increase list_increase, COMP__error* error) {
    // open with error
    return ANVIL__open__list(list_increase, &((*error).memory_error_occured));
}

// append a buffer but the error is compiler
void COMP__append__buffer(ANVIL__list* list, ANVIL__buffer buffer, COMP__error* error) {
    ANVIL__list__append__buffer(list, buffer, &((*error).memory_error_occured));

    return;
}

/* Current */
// check if a current buffer is still valid
ANVIL__bt COMP__check__current_within_range(ANVIL__buffer current) {
    return (current.start <= current.end);
}

// calculate a current buffer from a list // NOTE: buffer cannot be null or calculation fails!
COMP__current COMP__calculate__current_from_list_filled_index(ANVIL__list* list) {
    return ANVIL__create__buffer((*list).buffer.start, (*list).buffer.start + (*list).filled_index - 1);
}

// check for a character at a current
ANVIL__bt COMP__check__character_range_at_current(ANVIL__buffer current, ANVIL__character range_start, ANVIL__character range_end) {
    return ((*(ANVIL__character*)current.start) >= range_start) && ((*(ANVIL__character*)current.start) <= range_end);
}

// calculate the amounnt of items in one list (assumes all items are same size!)
ANVIL__list_filled_index COMP__calculate__list_content_count(ANVIL__list list, size_t item_size) {
    return list.filled_index / item_size;
}

/* Lexer */
// lexling types
typedef enum COMP__lt {
    COMP__lt__invalid,
    COMP__lt__left_parenthesis,
    COMP__lt__right_parenthesis,
    COMP__lt__left_curly_bracket,
    COMP__lt__right_curly_bracket,
    COMP__lt__name,
    COMP__lt__at,
    COMP__lt__hashtag,
    COMP__lt__equals,
    COMP__lt__string_literal,
    COMP__lt__end_of_file,
    COMP__lt__COUNT,
} COMP__lt;

// lexling type
typedef struct COMP__lexling {
    COMP__lexling_type type;
    ANVIL__buffer value;
    COMP__character_location location;
} COMP__lexling;

// create custom lexling
COMP__lexling COMP__create__lexling(COMP__lexling_type type, ANVIL__buffer value, COMP__character_location location) {
    COMP__lexling output;

    // setup output
    output.type = type;
    output.value = value;
    output.location = location;

    return output;
}

// create null lexling
COMP__lexling COMP__create_null__lexling() {
    // return empty
    return COMP__create__lexling(COMP__lt__invalid, ANVIL__create_null__buffer(), COMP__create_null__character_location());
}

// lexlings
typedef struct COMP__lexlings {
    ANVIL__list data;
} COMP__lexlings;

// create custom lexlings
COMP__lexlings COMP__create__lexlings(ANVIL__list list) {
    COMP__lexlings output;

    // setup output
    output.data = list;

    return output;
}

// create null lexlings
COMP__lexlings COMP__create_null__lexlings() {
    // return empty
    return COMP__create__lexlings(ANVIL__create_null__list());
}

// close lexlings
void COMP__close__lexlings(COMP__lexlings lexlings) {
    // close buffer
    ANVIL__close__list(lexlings.data);

    return;
}

// append a lexling to the list
void COMP__append__lexling(COMP__lexlings* lexlings, COMP__lexling lexling, COMP__error* error) {
    // request space
    ANVIL__list__request__space(&((*lexlings).data), sizeof(COMP__lexling), &(*error).memory_error_occured);

    // append data
    (*(COMP__lexling*)ANVIL__calculate__list_current_address(&((*lexlings).data))) = lexling;

    // increase fill
    (*lexlings).data.filled_index += sizeof(COMP__lexling);

    return;
}

// append a lexlings to the list
void COMP__append__lexlings(ANVIL__list* list, COMP__lexlings lexlings, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__lexlings), &(*error).memory_error_occured);

    // append data
    (*(COMP__lexlings*)ANVIL__calculate__list_current_address(list)) = lexlings;

    // increase fill
    (*list).filled_index += sizeof(COMP__lexlings);

    return;
}

// read a lexling from an address
COMP__lexling COMP__read__lexling_from_current(COMP__current current) {
    // return struct
    return *(COMP__lexling*)current.start;
}

// check one character
ANVIL__bt COMP__calculate__valid_character_range(COMP__current current, ANVIL__character start, ANVIL__character end) {
    // perform calculation
    return (ANVIL__bt)((*(ANVIL__character*)(current.start) >= start) && (*(ANVIL__character*)(current.start) <= end));
}

// check one character for a name character
ANVIL__bt COMP__calculate__valid_name_character(COMP__current current) {
    return (COMP__calculate__valid_character_range(current, 'a', 'z') || COMP__calculate__valid_character_range(current, 'A', 'Z') || COMP__calculate__valid_character_range(current, '0', '9') || COMP__calculate__valid_character_range(current, '_', '_') || COMP__calculate__valid_character_range(current, '.', '.'));
}

// calculate character index
COMP__character_index COMP__calculate__character_index(ANVIL__buffer main_buffer, ANVIL__buffer current) {
    return (COMP__character_index)(current.start - main_buffer.start);
}

// lex a program
COMP__lexlings COMP__compile__lex(ANVIL__buffer user_code, COMP__file_index file_index, COMP__error* error) {
    COMP__lexlings output;
    COMP__current current;
    COMP__line_number current_line_number;
    COMP__lexling_start temp_start;
    COMP__lexling_end temp_end;

    // setup output
    output.data = COMP__open__list(sizeof(COMP__lexling) * 64, error);

    // check for error
    if (COMP__check__error_occured(error)) {
        // return empty
        return COMP__create_null__lexlings();
    }

    // setup current & locations
    current = user_code;
    current_line_number = 1;

    // lex program
    while (COMP__check__current_within_range(current)) {
        // skip comments and whitespace
        while (COMP__check__current_within_range(current) && (COMP__calculate__valid_character_range(current, 0, 32) || COMP__calculate__valid_character_range(current, '[', '['))) {
            // skip whitespace
            while (COMP__check__current_within_range(current) && COMP__calculate__valid_character_range(current, 0, 32)) {
                // check for new line
                if (COMP__calculate__valid_character_range(current, '\n', '\n') || COMP__calculate__valid_character_range(current, '\r', '\r')) {
                    // next line
                    current_line_number++;
                }

                // next character
                current.start += sizeof(ANVIL__character);
            }

            // skip comments
            if (COMP__check__current_within_range(current) && COMP__calculate__valid_character_range(current, '[', '[')) {
                COMP__lexling_depth depth = 1;

                // next character
                current.start += sizeof(ANVIL__character);

                // skip past characters
                while (COMP__check__current_within_range(current) && depth > 0) {
                    // check for new line
                    if (COMP__calculate__valid_character_range(current, '\n', '\n') || COMP__calculate__valid_character_range(current, '\r', '\r')) {
                        // next line
                        current_line_number++;
                    }
                    // check for opening comment
                    if (COMP__calculate__valid_character_range(current, '[', '[')) {
                        // increase depth
                        depth++;
                    }
                    // check for closing comment
                    if (COMP__calculate__valid_character_range(current, ']', ']')) {
                        // decrease depth
                        depth--;
                    }

                    // next character
                    current.start += sizeof(ANVIL__character);
                }

                // check for unfinished comment
                if (depth > 0) {
                    // set error
                    *error = COMP__open__error("Lexing Error: Comment ended with end of file instead of proper closing.", COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current)));

                    goto quit;
                }
            }
        }

        // check for out of range
        if (COMP__check__current_within_range(current) == ANVIL__bt__false) {
            goto quit;
        }

        // check for lexlings
        if (COMP__calculate__valid_character_range(current, '(', '(')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_parenthesis, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, ')', ')')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_parenthesis, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '{', '{')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_curly_bracket, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '}', '}')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_curly_bracket, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_name_character(current)) {
            // get lexling start and setup temp end
            temp_start = current.start;
            temp_end = temp_start - 1;

            // get lexling size
            while (COMP__check__current_within_range(current) && COMP__calculate__valid_name_character(current)) {
                // next character
                current.start += sizeof(ANVIL__character);
                temp_end += sizeof(ANVIL__character);
            }

            // record lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__name, ANVIL__create__buffer(temp_start, temp_end), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, ANVIL__create__buffer(temp_start, temp_end)))), error);
        } else if (COMP__calculate__valid_character_range(current, '@', '@')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__at, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '#', '#')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__hashtag, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '=', '=')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__equals, ANVIL__create__buffer(current.start, current.start), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '"', '"')) {
            ANVIL__buffer data;

            // get string start
            data.start = current.start;

            // advance current
            current.start += sizeof(ANVIL__character);

            // search for string end
            while (COMP__check__current_within_range(current) && COMP__calculate__valid_character_range(current, '\"', '\"') == ANVIL__bt__false) {
                // next character
                current.start += sizeof(ANVIL__character);
            }

            // check for end of file
            if (COMP__check__current_within_range(current) == ANVIL__bt__false) {
                // string ended abruptly
                *error = COMP__open__error("Lexical Error: String ended at the end of a file and not with a (\").", COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current)));

                goto quit;
            }

            // finish string data
            data.end = current.start;

            // append lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__string_literal, data, COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, data))), error);

            // next character
            current.start += sizeof(ANVIL__character);
        // no lexling found
        } else {
            // open error
            *error = COMP__open__error("Lexical Error: Invalid character.", COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current)));

            // quit
            goto quit;
        }

        // check for error
        if (COMP__check__error_occured(error)) {
            // return lexlings as they are
            goto quit;
        }
    }

    // quit
    quit:

    // append eof lexling
    COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__end_of_file, ANVIL__open__buffer_from_string((u8*)"[EOF]", ANVIL__bt__false, ANVIL__bt__false), COMP__create__character_location(file_index, current_line_number, COMP__calculate__character_index(user_code, current))), error);

    return output;
}

// print lexlings
void COMP__debug__print_lexlings(COMP__lexlings lexlings) {
    COMP__current current;
    COMP__lexling temp;

    // setup current
    current = lexlings.data.buffer;

    // print header
    printf("Lexlings:\n");

    // print each lexling
    while (current.start < lexlings.data.buffer.start + lexlings.data.filled_index) {
        // get lexling
        temp = COMP__read__lexling_from_current(current);

        // next lexling
        current.start += sizeof(COMP__lexling);

        // print lexling type
        printf("\t%lu [ %lu, %lu ] [ file_index: %lu, line_number: %lu, character_index: %lu ] : ", (ANVIL__u64)temp.type, (ANVIL__u64)temp.value.start, (ANVIL__u64)temp.value.end, temp.location.file_index, (ANVIL__u64)temp.location.line_number, (ANVIL__u64)temp.location.character_index);
        fflush(stdout);

        // print lexling string
        ANVIL__print__buffer(temp.value);

        // print new line
        printf("\n");
    }

    return;
}

/* Names */
// name struct
typedef struct COMP__name {
    COMP__lexling lexling;
} COMP__name;

// create name
COMP__name COMP__create__name(COMP__lexling lexling) {
    COMP__name output;

    output.lexling = lexling;

    return output;
}

// create null name
COMP__name COMP__create_null__name() {
    return COMP__create__name(COMP__create_null__lexling());
}

// append name
void COMP__append__name(ANVIL__list* list, COMP__name data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__name), &(*error).memory_error_occured);

    // append data
    (*(COMP__name*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__name);

    return;
}

/* Parser */
// parsing argument type
typedef enum COMP__pat {
    COMP__pat__invalid,
    COMP__pat__variable,
    COMP__pat__variable__input,
    COMP__pat__variable__output,
    COMP__pat__variable__double,
    COMP__pat__variable__body,
    COMP__pat__variable__predefined,
    COMP__pat__offset,
    COMP__pat__flag,
    COMP__pat__flag__user_defined,
    COMP__pat__flag__predefined,
    COMP__pat__literal__boolean,
    COMP__pat__literal__binary,
    COMP__pat__literal__integer,
    COMP__pat__literal__hexadecimal,
    COMP__pat__literal__string,

    // count
    COMP__pat__COUNT,
} COMP__pat;

// parsling argument
typedef struct COMP__parsling_argument {
    COMP__pat type;
    COMP__name text;
    ANVIL__cell_integer_value value;
} COMP__parsling_argument;

// create a custom argument
COMP__parsling_argument COMP__create__parsling_argument(COMP__pat type, COMP__name text, ANVIL__cell_integer_value value) {
    COMP__parsling_argument output;

    // setup output
    output.type = type;
    output.text = text;
    output.value = value;

    return output;
}

// check if arguments have the same text
ANVIL__bt COMP__check__parsling_arguments_have_same_text(COMP__parsling_argument a, COMP__parsling_argument b) {
    return ANVIL__calculate__buffer_contents_equal(a.text.lexling.value, b.text.lexling.value);
}

// find parsling argument index in list
COMP__argument_index COMP__find__parsling_argument_index__by_name(ANVIL__list list, COMP__parsling_argument argument) {
    COMP__argument_index output = 0;

    // setup current
    COMP__current current_argument = COMP__calculate__current_from_list_filled_index(&list);

    // find argument
    while (COMP__check__current_within_range(current_argument)) {
        // check arg
        if (COMP__check__parsling_arguments_have_same_text(*(COMP__parsling_argument*)current_argument.start, argument)) {
            // found
            return output;
        }

        // next arg
        output++;
        current_argument.start += sizeof(COMP__parsling_argument);
    }

    // not found
    return output;
}

// translate string to boolean
ANVIL__bt COMP__translate__string_to_boolean(ANVIL__buffer string, ANVIL__cell_integer_value* value) {
    // check possible values
    if (ANVIL__calculate__buffer_contents_equal(string, ANVIL__open__buffer_from_string((u8*)"frost.boolean.false", ANVIL__bt__false, ANVIL__bt__false)) == ANVIL__bt__true) {
        *value = (ANVIL__cell_integer_value)(ANVIL__bt__false);

        return ANVIL__bt__true;
    }
    if (ANVIL__calculate__buffer_contents_equal(string, ANVIL__open__buffer_from_string((u8*)"frost.boolean.true", ANVIL__bt__false, ANVIL__bt__false)) == ANVIL__bt__true) {
        *value = (ANVIL__cell_integer_value)(ANVIL__bt__true);

        return ANVIL__bt__true;
    }

    return ANVIL__bt__false;
}

// translate string to binary
ANVIL__bt COMP__translate__string_to_binary(ANVIL__buffer string, ANVIL__cell_integer_value* value) {
    ANVIL__buffer prefix = ANVIL__open__buffer_from_string((u8*)"frost.binary.", ANVIL__bt__false, ANVIL__bt__false);
    ANVIL__buffer current;
    ANVIL__u64 character_count_limit = sizeof(ANVIL__u64) * ANVIL__define__bits_in_byte;
    ANVIL__u64 character_count = 0;

    // check for prefix
    if (ANVIL__calculate__buffer_starts_with_buffer(string, prefix) == ANVIL__bt__false) {
        // not a binary literal
        *value = ANVIL__define__null_address;

        return ANVIL__bt__false;
    }

    // setup current
    current = ANVIL__create__buffer(string.start + ANVIL__calculate__buffer_length(prefix), string.end);

    // pre check for all valid characters
    while (COMP__check__current_within_range(current)) {
        // check character
        if ((COMP__check__character_range_at_current(current, '0', '1') || COMP__check__character_range_at_current(current, '_', '_')) == ANVIL__bt__false) {
            // not a binary literal
            *value = ANVIL__define__null_address;

            return ANVIL__bt__false;
        }

        // count binary character
        if (COMP__check__character_range_at_current(current, '0', '1') == ANVIL__bt__true) {
            character_count++;
        }

        // advance current
        current.start += sizeof(ANVIL__character);
    }

    // check for sane character limit
    if (character_count > character_count_limit) {
        // binary literal to large, conversion failed
        *value = ANVIL__define__null_address;

        return ANVIL__bt__false;
    }

    // setup bit index
    ANVIL__bit_count bit_index = 0;

    // reset current
    current = ANVIL__create__buffer(string.start + ANVIL__calculate__buffer_length(prefix), string.end);

    // convert binary string to binary number
    while (COMP__check__current_within_range(current)) {
        if (COMP__check__character_range_at_current(current, '_', '_') == ANVIL__bt__false) {
            // append value
            *value += (((*(ANVIL__character*)current.start) - '0') << bit_index);

            // next bit index
            bit_index++;
        }

        // next character
        current.start += sizeof(ANVIL__character);
    }

    return ANVIL__bt__true;
}

// translate string to integer
ANVIL__bt COMP__translate__string_to_integer(ANVIL__buffer string, ANVIL__cell_integer_value* value) {
    ANVIL__buffer prefix = ANVIL__open__buffer_from_string((u8*)"frost.integer.", ANVIL__bt__false, ANVIL__bt__false);
    ANVIL__buffer suffix;
    ANVIL__u64 digit = 0;

    // check for prefix
    if (ANVIL__calculate__buffer_starts_with_buffer(string, prefix) == ANVIL__bt__false) {
        // not an integer literal
        *value = ANVIL__define__null_address;

        return ANVIL__bt__false;
    }

    // create suffix
    suffix = ANVIL__create__buffer(string.start + ANVIL__calculate__buffer_length(prefix), string.end);

    // translate number
    // if number is negative
    if (*(ANVIL__character*)suffix.start == (ANVIL__character)'n') {
        // for each character
        for (COMP__character_index i = ANVIL__calculate__buffer_length(suffix); i > 1; i--) {
            // check for valid character
            if (((((ANVIL__character*)suffix.start)[i - 1] >= '0' && ((ANVIL__character*)suffix.start)[i - 1] <= '9') || ((ANVIL__character*)suffix.start)[i - 1] == '_') == ANVIL__bt__false) {
                // invalid character
                *value = ANVIL__define__null_address;

                return ANVIL__bt__false;
            }

            // if calculable character
            if (((ANVIL__character*)suffix.start)[i - 1] != '_') {
                // add value
                *value += COMP__calculate__exponent(10, digit) * (((ANVIL__character*)suffix.start)[i - 1] - '0');

                // next digit power
                digit++;
            }
        }

        // turn number negative using twos compliment
        *value = ~(*value);
        *value = (*value) + 1;
    // if number is positive
    } else {
        // for each character
        for (COMP__character_index i = ANVIL__calculate__buffer_length(suffix); i > 0; i--) {
            // check for valid character
            if (((((ANVIL__character*)suffix.start)[i - 1] >= '0' && ((ANVIL__character*)suffix.start)[i - 1] <= '9') || ((ANVIL__character*)suffix.start)[i - 1] == '_') == ANVIL__bt__false) {
                // invalid character
                *value = ANVIL__define__null_address;

                return ANVIL__bt__false;
            }

            // if calculable character
            if (((ANVIL__character*)suffix.start)[i - 1] != '_') {
                // add value
                *value += COMP__calculate__exponent(10, digit) * (((ANVIL__character*)suffix.start)[i - 1] - '0');

                // next digit power
                digit++;
            }
        }
    }

    return ANVIL__bt__true;
}

// translate character to hexadecimal
ANVIL__cell_integer_value COMP__translate__character_to_hexadecimal(ANVIL__character character, ANVIL__bt* invalid_character) {
    // set character as valid
    *invalid_character = ANVIL__bt__false;

    // translate character
    if (character >= '0' && character <= '9') {
        return character - '0';
    } else if (character >= 'a' && character <= 'f') {
        return character - 'a' + 10;
    } else if (character >= 'A' && character <= 'F') {
        return character - 'A' + 10;
    } else {
        // invalid character
        *invalid_character = ANVIL__bt__true;
    }

    return ANVIL__define__null_address;
}

// translate string to hexedecimal
ANVIL__bt COMP__translate__string_to_hexedecimal(ANVIL__buffer string, ANVIL__cell_integer_value* value) {
    ANVIL__buffer prefix = ANVIL__open__buffer_from_string((u8*)"frost.hexadecimal.", ANVIL__bt__false, ANVIL__bt__false);
    ANVIL__buffer suffix;
    ANVIL__buffer current;
    ANVIL__bt invalid_character;
    ANVIL__cell_integer_value hex_digit;

    // check for prefix
    if (ANVIL__calculate__buffer_starts_with_buffer(string, prefix) == ANVIL__bt__false) {
        // not a hexadecimal literal
        *value = ANVIL__define__null_address;

        return ANVIL__bt__false;
    }

    // create suffix
    suffix = ANVIL__create__buffer(string.start + ANVIL__calculate__buffer_length(prefix), string.end);

    // create current
    current = suffix;

    // setup value
    *value = ANVIL__define__null_address;

    // translate number
    while (COMP__check__current_within_range(current)) {
        // check separator
        if (*(ANVIL__character*)current.start == '_') {
            // skip
            current.start += sizeof(ANVIL__character);

            continue;
        }
        
        // check digit
        hex_digit = COMP__translate__character_to_hexadecimal(*(ANVIL__character*)current.start, &invalid_character);
        if (invalid_character == ANVIL__bt__true) {
            // invalid digit, invalid hex string
            *value = ANVIL__define__null_address;

            return ANVIL__bt__false;
        }

        // append digit
        *value = (*value) << 4;
        *value = (*value) & (~15);
        *value = (*value) + hex_digit;

        // next character
        current.start += sizeof(ANVIL__character);
    }

    return ANVIL__bt__true;
}

// setup null argument
COMP__parsling_argument COMP__create_null__parsling_argument() {
    return COMP__create__parsling_argument(COMP__pat__invalid, COMP__create_null__name(), 0);
}

// statement type
typedef enum COMP__st {
    COMP__st__invalid,
    COMP__st__abstraction_call,
    COMP__st__abstraction_header,
    COMP__st__offset,

    // count
    COMP__st__COUNT,
} COMP__st;

// one statement
typedef struct COMP__parsling_statement {
    COMP__st type;

    // name
    COMP__parsling_argument name;

    // abstraction call data
    ANVIL__list inputs; // COMP__parsling_argument
    ANVIL__list outputs; // COMP__parsling_argument

    // metadata
    COMP__input_count input_count;
    COMP__output_count output_count;
} COMP__parsling_statement;

// create a custom statement
COMP__parsling_statement COMP__create__parsling_statement(COMP__st type, COMP__parsling_argument name, ANVIL__list inputs, ANVIL__list outputs, COMP__input_count input_count, COMP__output_count output_count) {
    COMP__parsling_statement output;

    // setup output
    output.type = type;
    output.name = name;
    output.inputs = inputs;
    output.outputs = outputs;
    output.input_count = input_count;
    output.output_count = output_count;

    return output;
}

// create a null statement
COMP__parsling_statement COMP__create_null__parsling_statement() {
    // return empty
    return COMP__create__parsling_statement(COMP__st__invalid, COMP__create_null__parsling_argument(), ANVIL__create_null__list(), ANVIL__create_null__list(), 0, 0);
}

// one abstraction
typedef struct COMP__parsling_abstraction {
    COMP__parsling_statement header;
    ANVIL__list statements; // COMP__parsling_statement
} COMP__parsling_abstraction;

// create a custom abstraction
COMP__parsling_abstraction COMP__create__parsling_abstraction(COMP__parsling_statement header, ANVIL__list statements) {
    COMP__parsling_abstraction output;

    // setup output
    output.header = header;
    output.statements = statements;

    return output;
}

// create a null abstraction
COMP__parsling_abstraction COMP__create_null__parsling_abstraction() {
    // return empty
    return COMP__create__parsling_abstraction(COMP__create_null__parsling_statement(), ANVIL__create_null__list());
}

// one program
typedef struct COMP__parsling_program {
    ANVIL__list abstractions; // COMP__parsling_abstraction
} COMP__parsling_program;

// create a custom program
COMP__parsling_program COMP__create__parsling_program(ANVIL__list abstractions) {
    COMP__parsling_program output;

    // setup output
    output.abstractions = abstractions;

    return output;
}

// create a null program
COMP__parsling_program COMP__create_null__parsling_program() {
    // return empty
    return COMP__create__parsling_program(ANVIL__create_null__list());
}

// append parsling argument
void COMP__append__parsling_argument(ANVIL__list* list, COMP__parsling_argument data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_argument), &(*error).memory_error_occured);

    // append data
    (*(COMP__parsling_argument*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_argument);

    return;
}

// append parsling statement
void COMP__append__parsling_statement(ANVIL__list* list, COMP__parsling_statement data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_statement), &(*error).memory_error_occured);

    // append data
    (*(COMP__parsling_statement*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_statement);

    return;
}

// append parsling abstraction
void COMP__append__parsling_abstraction(ANVIL__list* list, COMP__parsling_abstraction data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_abstraction), &(*error).memory_error_occured);

    // append data
    (*(COMP__parsling_abstraction*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_abstraction);

    return;
}

// append parsling program
void COMP__append__parsling_program(ANVIL__list* list, COMP__parsling_program data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_program), &(*error).memory_error_occured);

    // append data
    (*(COMP__parsling_program*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_program);

    return;
}

// close statement
void COMP__close__parsling_statement(COMP__parsling_statement statement) {
    // close io
    if (ANVIL__check__empty_list(statement.inputs) == ANVIL__bt__false) {
        ANVIL__close__list(statement.inputs);
    }
    if (ANVIL__check__empty_list(statement.outputs) == ANVIL__bt__false) {
        ANVIL__close__list(statement.outputs);
    }

    return;
}

// close statements
void COMP__close__parsling_statements(ANVIL__list list) {
    COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&list);

    // clean up each statement
    while (COMP__check__current_within_range(current_statement)) {
        // clean up statement
        COMP__close__parsling_statement(*(COMP__parsling_statement*)current_statement.start);

        // next statement
        current_statement.start += sizeof(COMP__parsling_statement);
    }

    // clean up statements buffer
    ANVIL__close__list(list);

    return;
}

// close abstraction
void COMP__close__parsling_abstraction(COMP__parsling_abstraction abstraction) {
    // close header
    COMP__close__parsling_statement(abstraction.header);
    
    // close statements
    if (ANVIL__check__empty_list(abstraction.statements) == ANVIL__bt__false) {
        COMP__close__parsling_statements(abstraction.statements);
    }

    return;
}

// close parsling program
void COMP__close__parsling_program(COMP__parsling_program program) {
    COMP__current current = COMP__calculate__current_from_list_filled_index(&(program.abstractions));

    // free each abstraction
    while (COMP__check__current_within_range(current)) {
        // free abstraction
        COMP__close__parsling_abstraction(*(COMP__parsling_abstraction*)current.start);

        // next abstraction
        current.start += sizeof(COMP__parsling_abstraction);
    }

    // free the abstraction list
    ANVIL__close__list(program.abstractions);

    return;
}

// create a name from a lexing
COMP__name COMP__create__name_from_lexling_current(COMP__current lexling_current) {
    COMP__name output;

    // setup
    output.lexling = COMP__read__lexling_from_current(lexling_current);

    return output;
}

// advance the current buffer by N number of lexlings
void COMP__advance__lexling_current(COMP__current* current, COMP__lexling_index lexling_count) {
    // advance the pointer
    (*current).start += sizeof(COMP__lexling) * lexling_count;

    return;
}

// revert lexling current by N number of lexlings
void COMP__revert__lexling_current(COMP__current* current, COMP__lexling_index lexling_count) {
    (*current).start -= sizeof(COMP__lexling) * lexling_count;

    return;
}

// parse arguments
ANVIL__list COMP__parse__arguments(COMP__current* current, COMP__io_count* count, ANVIL__bt is_header, COMP__error* error) {
    ANVIL__list output;
    ANVIL__cell_integer_value value = 0;

    // init count
    *count = 0;

    // open names list
    output = COMP__open__list(sizeof(COMP__parsling_argument) * 8, error);

    // check for error
    if (COMP__check__error_occured(error)) {
        return output;
    }
    
    // check for opening parenthesis
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__left_parenthesis) {
        // next lexling
        COMP__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMP__open__error("Parse Error: Arguments is missing opening parenthesis.", COMP__read__lexling_from_current(*current).location);

        return output;
    }

    // get arguments
    while (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type != COMP__lt__right_parenthesis) {
        COMP__parsling_argument argument = COMP__create_null__parsling_argument();

        // check type
        // is variable / literal
        if (COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
            // determine value / is variable
            if (COMP__translate__string_to_boolean(COMP__read__lexling_from_current(*current).value, &value)) {
                argument = COMP__create__parsling_argument(COMP__pat__literal__boolean, COMP__create__name_from_lexling_current(*current), value);
            } else if (COMP__translate__string_to_binary(COMP__read__lexling_from_current(*current).value, &value)) {
                argument = COMP__create__parsling_argument(COMP__pat__literal__binary, COMP__create__name_from_lexling_current(*current), value);
            } else if (COMP__translate__string_to_integer(COMP__read__lexling_from_current(*current).value, &value)) {
                argument = COMP__create__parsling_argument(COMP__pat__literal__integer, COMP__create__name_from_lexling_current(*current), value);
            } else if (COMP__translate__string_to_hexedecimal(COMP__read__lexling_from_current(*current).value, &value)) {
                argument = COMP__create__parsling_argument(COMP__pat__literal__hexadecimal, COMP__create__name_from_lexling_current(*current), value);
            } else {
                // must be a variable
                argument = COMP__create__parsling_argument(COMP__pat__variable, COMP__create__name_from_lexling_current(*current), 0);
            }
        // offset
        } else if (COMP__read__lexling_from_current(*current).type == COMP__lt__at) {
            // advance current past at
            COMP__advance__lexling_current(current, 1);

            // if correct type
            if (COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
                // get name
                argument = COMP__create__parsling_argument(COMP__pat__offset, COMP__create__name_from_lexling_current(*current), 0);
            // error
            } else {
                *error = COMP__open__error("Parse Error: Offset is missing name.", COMP__read__lexling_from_current(*current).location);

                return output;
            }
        // flag
        } else if (COMP__read__lexling_from_current(*current).type == COMP__lt__hashtag) {
            // advance current past hashtag
            COMP__advance__lexling_current(current, 1);

            // if correct type
            if (COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
                // get name
                argument = COMP__create__parsling_argument(COMP__pat__flag, COMP__create__name_from_lexling_current(*current), 0);
            // error
            } else {
                *error = COMP__open__error("Parse Error: Flag is missing name.", COMP__read__lexling_from_current(*current).location);

                return output;
            }
        // string literal
        } else if (COMP__read__lexling_from_current(*current).type == COMP__lt__string_literal) {
            // get argument
            argument = COMP__create__parsling_argument(COMP__pat__literal__string, COMP__create__name_from_lexling_current(*current), 0);
        // error
        } else {
            *error = COMP__open__error("Parse Error: Unrecognized argument type.", COMP__read__lexling_from_current(*current).location);

            return output;
        }

        // check argument for variable only
        if (is_header == ANVIL__bt__true && argument.type != COMP__pat__variable) {
            // set error
            *error = COMP__open__error("Parse Error: A non-variable argument was detected in a header.", COMP__read__lexling_from_current(*current).location);

            return output;
        }

        // append argument
        COMP__append__parsling_argument(&output, argument, error);
        
        // increment count
        *count = *count + 1;

        // check for error
        if (COMP__check__error_occured(error)) {
            return output;
        }

        // next current
        COMP__advance__lexling_current(current, 1);
    }

    // check for closing parenthesis
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__right_parenthesis) {
        // next lexling
        COMP__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMP__open__error("Parse Error: Arguments is missing closing parenthesis.", COMP__read__lexling_from_current(*current).location);

        return output;
    }

    return output;
}

// parse one statement
COMP__parsling_statement COMP__parse__statement(COMP__current* current, ANVIL__bt is_header, COMP__error* error) {
    COMP__parsling_statement output = COMP__create_null__parsling_statement();
    COMP__input_count input_count;
    COMP__output_count output_count;

    // check for offset
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__at) {
        // advance current
        COMP__advance__lexling_current(current, 1);

        // check for offset name
        if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
            // set name
            output.name = COMP__create__parsling_argument(COMP__pat__offset, COMP__create__name_from_lexling_current(*current), 0);

            // advance current
            COMP__advance__lexling_current(current, 1);
        // invalid syntax
        } else {
            // set error
            *error = COMP__open__error("Parse Error: Offset statement name is an invalid lexling.", COMP__read__lexling_from_current(*current).location);

            return output;
        }

        // null initialize unused data
        output.inputs = ANVIL__create_null__list();
        output.outputs = ANVIL__create_null__list();

        // setup type
        output.type = COMP__st__offset;
    // is an abstraction call
    } else if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
        // get name
        output.name = COMP__create__parsling_argument(COMP__pat__offset, COMP__create__name_from_lexling_current(*current), 0);

        // advance current
        COMP__advance__lexling_current(current, 1);

        // get inputs
        output.inputs = COMP__parse__arguments(current, &input_count, is_header, error);
        output.input_count = input_count;

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }

        // get outputs
        output.outputs = COMP__parse__arguments(current, &output_count, is_header, error);
        output.output_count = output_count;

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }

        // set type
        if (is_header == ANVIL__bt__true) {
            output.type = COMP__st__abstraction_header;
        } else {
            output.type = COMP__st__abstraction_call;
        }
    // error
    } else {
        // set error
        *error = COMP__open__error("Parse Error: Unrecognized statement type.", COMP__read__lexling_from_current(*current).location);

        return output;
    }

    return output;
}

// parse an abstraction
COMP__parsling_abstraction COMP__parse__abstraction(COMP__current* current, COMP__error* error) {
    COMP__parsling_abstraction output = COMP__create_null__parsling_abstraction();

    // check for eof
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__end_of_file) {
        return output;
    }

    // parse header
    output.header = COMP__parse__statement(current, ANVIL__bt__true, error);
    
    // check for error
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // check for equals sign
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__equals) {
        // no saving data necessary, next lexling
        COMP__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        *error = COMP__open__error("Parse Error: An abstraction definition has an equals sign missing.", COMP__read__lexling_from_current(*current).location);

        // quit
        return output;
    }

    // check for scope opener
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__left_curly_bracket) {
        // advance current
        COMP__advance__lexling_current(current, 1);
    // scope opener not found, error
    } else {
        // set error
        *error = COMP__open__error("Parse Error: Scope is missing left curly bracket.", COMP__read__lexling_from_current(*current).location);

        return output;
    }

    // parse statements
    // open statements list
    output.statements = COMP__open__list(sizeof(COMP__parsling_statement) * 16, error);
    
    // get statements
    while (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type != COMP__lt__right_curly_bracket) {
        // parse statement
        COMP__parsling_statement statement = COMP__parse__statement(current, ANVIL__bt__false, error);

        // add statement
        COMP__append__parsling_statement(&output.statements, statement, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }
    }

    // check for scope closer
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__right_curly_bracket) {
        // advance current
        COMP__advance__lexling_current(current, 1);
    // scope opener not found, error
    } else {
        // setup error
        *error = COMP__open__error("Parse Error: Scope is missing right curly bracket.", COMP__read__lexling_from_current(*current).location);

        return output;
    }

    return output;
}

// parse a file (program)
COMP__parsling_program COMP__parse__program(COMP__lexlings lexlings, COMP__error* error) {
    COMP__parsling_program output;
    COMP__parsling_abstraction temp;
    COMP__current current;

    // setup current
    current = COMP__calculate__current_from_list_filled_index(&lexlings.data);

    // open the abstraction list
    output.abstractions = COMP__open__list(sizeof(COMP__parsling_abstraction) * 64, error);

    // check for memory error
    if (COMP__check__error_occured(error)) {
        goto quit;
    }

    // parse abstractions
    while (COMP__check__current_within_range(current)) {
        // if end of file
        if (COMP__read__lexling_from_current(current).type == COMP__lt__end_of_file) {
            // finished parsing
            goto quit;
        }
        
        // parse abstraction
        temp = COMP__parse__abstraction(&current, error);

        // append abstraction
        COMP__append__parsling_abstraction(&(output.abstractions), temp, error);

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            goto quit;
        }
    }

    // quit
    quit:

    return output;
}

ANVIL__buffer COMP__convert__parsed_argument_type_to_string_buffer(COMP__pat argument_type) {
    return ANVIL__open__buffer_from_string((u8*)(COMP__global__argument_type_name_strings[argument_type]), ANVIL__bt__false, ANVIL__bt__false);
}

// print argument
void COMP__print__parsling_argument(COMP__parsling_argument argument) {
    // print type
    printf("[");
    ANVIL__print__buffer(COMP__convert__parsed_argument_type_to_string_buffer(argument.type));
    printf("]");

    // print data
    if (argument.type == COMP__pat__variable || COMP__pat__variable__input || COMP__pat__variable__output || COMP__pat__variable__double || COMP__pat__variable__body || COMP__pat__variable__predefined || COMP__pat__offset || COMP__pat__flag || COMP__pat__literal__string) {
        ANVIL__print__buffer(argument.text.lexling.value);
    } else if (argument.type == COMP__pat__literal__boolean || COMP__pat__literal__binary || COMP__pat__literal__hexadecimal) {
        ANVIL__print__buffer(argument.text.lexling.value);
        printf("[%lu]", argument.value);
    } else if (argument.type == COMP__pat__literal__integer) {
        ANVIL__print__buffer(argument.text.lexling.value);
        printf("[%lu, %li]", argument.value, argument.value);
    }

    return;
}

// print arguments
void COMP__print__parsling_arguments(ANVIL__list* arguments) {
    COMP__current current = COMP__calculate__current_from_list_filled_index(arguments);

    // print opener
    printf("(");

    // for each name
    while (COMP__check__current_within_range(current)) {
        // print separator
        if (current.start != (*arguments).buffer.start) {
            // print
            printf(" ");
        }

        // print argument
        COMP__print__parsling_argument(*(COMP__parsling_argument*)current.start);

        // next buffer
        current.start += sizeof(COMP__parsling_argument);
    }

    // print closer
    printf(")");

    return;
}

// print statement
void COMP__print__parsed_statement(COMP__parsling_statement statement) {
    // print statement
    if (statement.type == COMP__st__offset) {
        // print offset information
        printf("@");
        ANVIL__print__buffer(statement.name.text.lexling.value);
    } else if (statement.type == COMP__st__abstraction_call || statement.type == COMP__st__abstraction_header) {
        // print statement
        ANVIL__print__buffer(statement.name.text.lexling.value);

        // print inputs
        COMP__print__parsling_arguments(&statement.inputs);

        // print outputs
        COMP__print__parsling_arguments(&statement.outputs);
    }

    return;
}

// print an abstraction
void COMP__print__parsed_abstraction(COMP__parsling_abstraction abstraction) {
    COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&abstraction.statements);

    // print header
    printf("\tAbstraction: ");
    if (abstraction.header.type != COMP__st__invalid) {
        COMP__print__parsed_statement(abstraction.header);
    }

    // new line for statements
    printf("\n");

    // print statements
    if (ANVIL__check__empty_list(abstraction.statements) == ANVIL__bt__false) {
        // print each statement
        while (COMP__check__current_within_range(current_statement)) {
            // print formatting
            printf("\t\t");

            // print statement
            COMP__print__parsed_statement(*(COMP__parsling_statement*)current_statement.start);

            // print new line
            printf("\n");

            // advance current
            current_statement.start += sizeof(COMP__parsling_statement);
        }
    }

    return;
}

// print a program
void COMP__print__parsed_program(COMP__parsling_program program) {
    COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&(program.abstractions));
    
    // print header
    printf("Parsed Program:\n");

    // print each function
    while (COMP__check__current_within_range(current_abstraction)) {
        // print function
        COMP__print__parsed_abstraction(*(COMP__parsling_abstraction*)current_abstraction.start);

        // advance current
        current_abstraction.start += sizeof(COMP__parsling_abstraction);
    }

    return;
}

/* Accounting */
// predefined variable type
typedef enum COMP__pvt {
    // variables
    COMP__pvt__error_code,

    // count
    COMP__pvt__COUNT,
} COMP__pvt;

// predefined flag type
typedef enum COMP__pft {
    // flags
    COMP__pft__always_run,
    COMP__pft__never_run,
    COMP__pft__temporary,

    // count
    COMP__pft__COUNT,
} COMP__pft;

// accountling argument
typedef struct COMP__accountling_argument {
    COMP__pat type;
    COMP__accountling_index index;
    COMP__parsling_argument value;
} COMP__accountling_argument;

// create accountling argument
COMP__accountling_argument COMP__create__accountling_argument(COMP__pat type, COMP__accountling_index index, COMP__parsling_argument value) {
    COMP__accountling_argument output;

    output.type = type;
    output.index = index;
    output.value = value;

    return output;
}

// create null accountling argument
COMP__accountling_argument COMP__create_null__accountling_argument() {
    return COMP__create__accountling_argument(COMP__pat__invalid, COMP__define__null_call_ID, COMP__create_null__parsling_argument());
}

// append accountling argument
void COMP__append__accountling_argument(ANVIL__list* list, COMP__accountling_argument data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_argument), &(*error).memory_error_occured);

    // append data
    (*(COMP__accountling_argument*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_argument);

    return;
}

// blueprintling
typedef ANVIL__u64 COMP__blueprintling;

// accountling call type
typedef enum COMP__act {
    // start
    COMP__act__START = 0,

    // acts
    // sets
    COMP__act__set__boolean = COMP__act__START,
    COMP__act__set__binary,
    COMP__act__set__integer,
    COMP__act__set__hexadecimal,
    COMP__act__set__offset,
    COMP__act__set__flag_ID,
    COMP__act__set__string,

    // io
    COMP__act__io__cell_to_address,
    COMP__act__io__address_to_cell,
    COMP__act__io__file_to_buffer,
    COMP__act__io__buffer_to_file,

    // copy
    COMP__act__copy__cell,
    COMP__act__copy__buffer,

    // memory
    COMP__act__memory__request_memory,
    COMP__act__memory__return_memory,

    // buffers
    COMP__act__buffer__calculate_length,

    // casts
    COMP__act__cast__cell_to_unsigned_integer_string,

    // prints
    COMP__act__print__binary,
    COMP__act__print__signed_integer,
    COMP__act__print__unsigned_integer,
    COMP__act__print__character,
    COMP__act__print__buffer_as_string,

    // integers
    COMP__act__integer__add,
    COMP__act__integer__subtract,
    COMP__act__integer__multiply,
    COMP__act__integer__divide,
    COMP__act__integer__modulous,
    COMP__act__integer__within_range,

    // binary
    COMP__act__binary__or,
    COMP__act__binary__invert,
    COMP__act__binary__and,
    COMP__act__binary__xor,
    COMP__act__binary__shift_higher,
    COMP__act__binary__shift_lower,
    COMP__act__binary__overwrite,

    // flags
    COMP__act__flag__get,
    COMP__act__flag__set,
    COMP__act__flag__invert,
    COMP__act__flag__or,
    COMP__act__flag__and,
    COMP__act__flag__xor,

    // jumps
    COMP__act__jump,

    // etc
    COMP__act__reset_error_code_cell,

    // end
    COMP__act__END,

    // user defined
    COMP__act__user_defined = COMP__act__END,

    // invalid
    COMP__act__invalid,

    // count
    COMP__act__COUNT = COMP__act__END - COMP__act__START,
} COMP__act;

// calculate user defined call ID
COMP__abstraction_index COMP__calculate__abstraction_index_from_call_index(COMP__call_index call_index) {
    return call_index - COMP__act__user_defined;
}

// accountling variable type (all predefined variables)
typedef enum COMP__avt {
    // start
    COMP__avt__START = 0,

    // avts
    COMP__avt__invalid = COMP__avt__START,
    COMP__avt__error_code,

    // end
    COMP__avt__END,

    // user defined
    COMP__avt__user_defined = COMP__avt__END,

    // count
    COMP__avt__COUNT = COMP__avt__END - COMP__avt__START,
} COMP__avt;

// accountling header
typedef struct COMP__accountling_abstraction_header {
    COMP__parsling_statement header;
    COMP__call_index call_index;
} COMP__accountling_abstraction_header;

// create custom accountling abstraction header
COMP__accountling_abstraction_header COMP__create__accountling_abstraction_header(COMP__parsling_statement header, COMP__call_index call_index) {
    COMP__accountling_abstraction_header output;

    output.header = header;
    output.call_index = call_index;

    return output;
}

// create null accountling abstraction header
COMP__accountling_abstraction_header COMP__create_null__accountling_abstraction_header() {
    return COMP__create__accountling_abstraction_header(COMP__create_null__parsling_statement(), COMP__act__invalid);
}

// close accountling abstraction header
void COMP__close__accountling_abstraction_header(COMP__accountling_abstraction_header header) {
    // close header
    COMP__close__parsling_statement(header.header);

    return;
}

// accountling statement
typedef struct COMP__accountling_statement {
    // type
    COMP__st type;

    // call
    COMP__accountling_abstraction_header header;
    ANVIL__list inputs; // COMP__accountling_argument
    ANVIL__list outputs; // COMP__accountling_argument

    // offset
    COMP__offset_index offset_ID;
} COMP__accountling_statement;

// setup custom accountling statement
COMP__accountling_statement COMP__create__accountling_statement(COMP__st type, COMP__accountling_abstraction_header header, ANVIL__list inputs, ANVIL__list outputs, COMP__offset_index offset_ID) {
    COMP__accountling_statement output;

    output.type = type;
    output.header = header;
    output.inputs = inputs;
    output.outputs = outputs;
    output.offset_ID = offset_ID;

    return output;
}

// create null accountling statement
COMP__accountling_statement COMP__create_null__accountling_statement() {
    return COMP__create__accountling_statement(COMP__st__invalid, COMP__create_null__accountling_abstraction_header(), ANVIL__create_null__list(), ANVIL__create_null__list(), COMP__define__null_offset_ID);
}

// close statement
void COMP__close__accountling_statement(COMP__accountling_statement statement) {
    // close io
    if (statement.type == COMP__st__abstraction_call) {
        ANVIL__close__list(statement.inputs);
        ANVIL__close__list(statement.outputs);
    }

    return;
}

// get statement input by index
COMP__accountling_argument COMP__get__abstractling_statement_argument_by_index(ANVIL__list list, COMP__argument_index index) {
    return ((COMP__accountling_argument*)list.buffer.start)[index];
}

// accountling abstraction
typedef struct COMP__accountling_abstraction {
    COMP__parsling_statement header;
    ANVIL__list* predefined_variables; // COMP__parsling_argument
    ANVIL__list* predefined_flags; // COMP__parsling_argument
    ANVIL__list strings; // COMP__parsling_argument
    ANVIL__list converted_strings; // ANVIL__buffer
    ANVIL__list inputs; // COMP__parsling_argument
    ANVIL__list outputs; // COMP__parsling_argument
    ANVIL__list doubles; // COMP__parsling_argument (doubles are variables that are both inputs and outputs)
    ANVIL__list variables; // COMP__parsling_argument
    ANVIL__list offsets; // COMP__parsling_argument
    ANVIL__list flags; // COMP__parsling_argument
    ANVIL__list statements; // COMP__accountling_statement
} COMP__accountling_abstraction;

// create custom accountling abstraction
COMP__accountling_abstraction COMP__create__accountling_abstraction(COMP__parsling_statement header, ANVIL__list* predefined_variables, ANVIL__list* predefined_flags, ANVIL__list strings, ANVIL__list converted_strings, ANVIL__list inputs, ANVIL__list outputs, ANVIL__list doubles, ANVIL__list variables, ANVIL__list offsets, ANVIL__list flags, ANVIL__list statements) {
    COMP__accountling_abstraction output;

    output.header = header;
    output.predefined_variables = predefined_variables;
    output.predefined_flags = predefined_flags;
    output.strings = strings;
    output.converted_strings = converted_strings;
    output.inputs = inputs;
    output.outputs = outputs;
    output.doubles = doubles;
    output.variables = variables;
    output.offsets = offsets;
    output.flags = flags;
    output.statements = statements;

    return output;
}

// create null accountling abstraction
COMP__accountling_abstraction COMP__create_null__accountling_abstraction() {
    return COMP__create__accountling_abstraction(COMP__create_null__parsling_statement(), ANVIL__define__null_address, ANVIL__define__null_address, ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list());
}

// append accountling statement
void COMP__append__accountling_statement(ANVIL__list* list, COMP__accountling_statement data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_statement), &(*error).memory_error_occured);

    // append data
    (*(COMP__accountling_statement*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_statement);

    return;
}

// append accountling abstraction
void COMP__append__accountling_abstraction(ANVIL__list* list, COMP__accountling_abstraction data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_abstraction), &(*error).memory_error_occured);

    // append data
    (*(COMP__accountling_abstraction*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_abstraction);

    return;
}

// close statements
void COMP__close__accountling_statements(ANVIL__list statements) {
    // check for empty statements
    if (ANVIL__check__empty_list(statements)) {
        // no need to free anything
        return;
    }

    // setup current
    COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&statements);

    // for each statement
    while (COMP__check__current_within_range(current_statement)) {
        // close statement
        COMP__close__accountling_statement(*(COMP__accountling_statement*)current_statement.start);

        // next statement
        current_statement.start += sizeof(COMP__accountling_statement);
    }

    // close lists
    ANVIL__close__list(statements);

    return;
}

// close accountling abstraction
void COMP__close__accountling_abstraction(COMP__accountling_abstraction abstraction) {
    // close lists
    if (ANVIL__check__empty_list(abstraction.inputs) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.inputs);
    }
    if (ANVIL__check__empty_list(abstraction.outputs) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.outputs);
    }
    if (ANVIL__check__empty_list(abstraction.doubles) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.doubles);
    }
    if (ANVIL__check__empty_list(abstraction.variables) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.variables);
    }
    if (ANVIL__check__empty_list(abstraction.offsets) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.offsets);
    }
    if (ANVIL__check__empty_list(abstraction.flags) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.flags);
    }
    if (ANVIL__check__empty_list(abstraction.strings) == ANVIL__bt__false) {
        ANVIL__close__list(abstraction.strings);
    }
    if (ANVIL__check__empty_list(abstraction.converted_strings) == ANVIL__bt__false) {
        COMP__current current_buffer = COMP__calculate__current_from_list_filled_index(&abstraction.converted_strings);

        // for each buffer
        while (COMP__check__current_within_range(current_buffer)) {
            // free buffer
            ANVIL__close__buffer(*(ANVIL__buffer*)current_buffer.start);

            // next buffer
            current_buffer.start += sizeof(ANVIL__buffer);
        }

        // close list
        ANVIL__close__list(abstraction.converted_strings);
    }

    // close statements
    COMP__close__accountling_statements(abstraction.statements);

    return;
}

// accountling blueprint type
typedef enum COMP__abt {
    // start
    COMP__abt__START = COMP__act__END,

    // types
    COMP__abt__define_call = COMP__abt__START,
    COMP__abt__end_of_blueprint,

    // end
    COMP__abt__END,

    // count
    COMP__abt__COUNT = COMP__abt__END - COMP__abt__START,
} COMP__abt;

// blueprint name index type
typedef enum COMP__bnit {
    // names
    COMP__bnit__set,
    COMP__bnit__print__signed_integer,
    COMP__bnit__print__unsigned_integer,
    COMP__bnit__print__character,
    COMP__bnit__print__buffer_as_string,
    COMP__bnit__print__binary,
    COMP__bnit__io__cell_to_address,
    COMP__bnit__io__address_to_cell,
    COMP__bnit__io__file_to_buffer,
    COMP__bnit__io__buffer_to_file,
    COMP__bnit__copy__cell,
    COMP__bnit__copy__buffer,
    COMP__bnit__memory__request_memory,
    COMP__bnit__memory__return_memory,
    COMP__bnit__buffer__calculate_length,
    COMP__bnit__cast__cell_to_unsigned_integer_string,
    COMP__bnit__integer_add,
    COMP__bnit__integer_subtract,
    COMP__bnit__integer_multiply,
    COMP__bnit__integer_divide,
    COMP__bnit__integer_modulous,
    COMP__bnit__integer_within_range,
    COMP__bnit__binary__or,
    COMP__bnit__binary__invert,
    COMP__bnit__binary__and,
    COMP__bnit__binary__xor,
    COMP__bnit__binary__shift_higher,
    COMP__bnit__binary__shift_lower,
    COMP__bnit__binary__overwrite,
    COMP__bnit__flag__get,
    COMP__bnit__flag__set,
    COMP__bnit__flag__invert,
    COMP__bnit__flag__or,
    COMP__bnit__flag__and,
    COMP__bnit__flag__xor,
    COMP__bnit__jump,
    COMP__bnit__reset_error_code,

    // stats
    COMP__bnit__COUNT,
} COMP__bnit;

// append accountling abstraction header
void COMP__append__accountling_abstraction_header(ANVIL__list* list, COMP__accountling_abstraction_header data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_abstraction_header), &(*error).memory_error_occured);

    // append data
    (*(COMP__accountling_abstraction_header*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_abstraction_header);

    return;
}

// get and validate one header
COMP__accountling_abstraction_header COMP__account__get_abstraction_header(COMP__parsling_abstraction abstraction, COMP__call_index abstraction_call_ID, COMP__error* error) {
    COMP__accountling_abstraction_header output;

    output.header = abstraction.header;
    output.call_index = COMP__act__user_defined + abstraction_call_ID;

    // validate inputs
    for (ANVIL__length i = 0; i < output.header.input_count; i++) {
        // get argument
        COMP__parsling_argument argument = ((COMP__parsling_argument*)output.header.inputs.buffer.start)[i];

        // check argument validity
        if (argument.type != COMP__pat__variable) {
            *error = COMP__open__error("Accounting Error: An abstraction input header argument contains illegal argument type.", argument.text.lexling.location);

            return output;
        }
    }

    // validate outputs
    for (ANVIL__length i = 0; i < output.header.output_count; i++) {
        // get argument
        COMP__parsling_argument argument = ((COMP__parsling_argument*)output.header.outputs.buffer.start)[i];

        // check argument validity
        if (argument.type != COMP__pat__variable) {
            *error = COMP__open__error("Accounting Error: An abstraction output header argument contains illegal argument type.", argument.text.lexling.location);

            return output;
        }
    }

    return output;
}

// calculate blueprint list size
COMP__header_index COMP__calculate__call_blueprint_entry_count(ANVIL__list call_blueprint) {
    return call_blueprint.filled_index / sizeof(COMP__accountling_abstraction_header);
}

// check if argument is in variable category
ANVIL__bt COMP__check__argument_is_variable_type(COMP__pat argument_type) {
    return (argument_type == COMP__pat__variable) || (argument_type == COMP__pat__variable__body) || (argument_type == COMP__pat__variable__double) || (argument_type == COMP__pat__variable__input) || (argument_type == COMP__pat__variable__output) || (argument_type == COMP__pat__variable__predefined);
}

// check if argument is in flag category
ANVIL__bt COMP__check__argument_is_flag_type(COMP__pat argument_type) {
    return (argument_type == COMP__pat__flag) || (argument_type == COMP__pat__flag__user_defined) || (argument_type == COMP__pat__flag__predefined);
}

// check if an accountling already exists
COMP__header_index COMP__find__accountling_header_index(ANVIL__list call_blueprint, COMP__parsling_statement searching_for) {
    // search the blueprint for the correct header
    // setup current
    COMP__current current_header = COMP__calculate__current_from_list_filled_index(&call_blueprint);
    COMP__header_index header_index = 0;

    // search for match
    while (COMP__check__current_within_range(current_header)) {
        // get current header
        COMP__accountling_abstraction_header header = *(COMP__accountling_abstraction_header*)current_header.start;

        // check if abstraction matches
        if(COMP__check__parsling_arguments_have_same_text(header.header.name, searching_for.name)) {
            // check if io is same size
            if ((ANVIL__calculate__lists_have_same_fill_size(&searching_for.inputs, &header.header.inputs) && ANVIL__calculate__lists_have_same_fill_size(&searching_for.outputs, &header.header.outputs)) == ANVIL__bt__false) {
                goto next_header;
            }

            // setup currents for input
            COMP__current current_statement_io = COMP__calculate__current_from_list_filled_index(&searching_for.inputs);
            COMP__current current_header_io = COMP__calculate__current_from_list_filled_index(&header.header.inputs);

            // if inputs are not empty
            if (ANVIL__check__empty_buffer(current_statement_io) == ANVIL__bt__false) {
                // check inputs
                while (COMP__check__current_within_range(current_statement_io) && COMP__check__current_within_range(current_header_io)) {
                    // get types
                    COMP__pat statement_io_type = (*(COMP__parsling_argument*)current_statement_io.start).type;
                    COMP__pat header_io_type = (*(COMP__parsling_argument*)current_header_io.start).type;

                    // check types
                    // if type is in variable category
                    if (COMP__check__argument_is_variable_type(statement_io_type) && COMP__check__argument_is_variable_type(header_io_type)) {
                        // next input
                        goto next_input;
                    } else if (COMP__check__argument_is_flag_type(statement_io_type) && COMP__check__argument_is_flag_type(header_io_type)) {
                        // next input
                        goto next_input;
                    } else if (statement_io_type != header_io_type) {
                        // not a match
                        goto next_header;
                    }

                    // next input
                    next_input:
                    current_statement_io.start += sizeof(COMP__parsling_argument);
                    current_header_io.start += sizeof(COMP__parsling_argument);
                }
            }

            // re-setup currents
            current_statement_io = COMP__calculate__current_from_list_filled_index(&searching_for.outputs);
            current_header_io = COMP__calculate__current_from_list_filled_index(&header.header.outputs);

            // if outputs are not empty
            if (ANVIL__check__empty_buffer(current_statement_io) == ANVIL__bt__false) {
                // check outputs
                while (COMP__check__current_within_range(current_statement_io) && COMP__check__current_within_range(current_header_io)) {
                    // get types
                    COMP__pat statement_io_type = (*(COMP__parsling_argument*)current_statement_io.start).type;
                    COMP__pat header_io_type = (*(COMP__parsling_argument*)current_header_io.start).type;

                    // check types
                    // if type is in variable category
                    if (COMP__check__argument_is_variable_type(statement_io_type) && COMP__check__argument_is_variable_type(header_io_type)) {
                        // next output
                        goto next_output;
                    } else if (COMP__check__argument_is_flag_type(statement_io_type) && COMP__check__argument_is_flag_type(header_io_type)) {
                        // next output
                        goto next_output;
                    } else if (statement_io_type != header_io_type) {
                        // not a match
                        goto next_header;
                    }

                    // next output
                    next_output:
                    current_statement_io.start += sizeof(COMP__parsling_argument);
                    current_header_io.start += sizeof(COMP__parsling_argument);
                }
            }

            // match!
            return header_index;
        }

        // next header
        next_header:
        current_header.start += sizeof(COMP__accountling_abstraction_header);
        header_index++;
    }

    // match not found
    return COMP__calculate__call_blueprint_entry_count(call_blueprint);
}

// create call blueprint
ANVIL__list COMP__generate__call_blueprint(ANVIL__list parsling_programs, COMP__error* error) {
    const COMP__blueprintling blueprint[] = {
        // sets
        COMP__abt__define_call,
            COMP__act__set__boolean,
            COMP__bnit__set,
            1,
            COMP__pat__literal__boolean,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set__binary,
            COMP__bnit__set,
            1,
            COMP__pat__literal__binary,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set__integer,
            COMP__bnit__set,
            1,
            COMP__pat__literal__integer,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set__hexadecimal,
            COMP__bnit__set,
            1,
            COMP__pat__literal__hexadecimal,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set__offset,
            COMP__bnit__set,
            1,
            COMP__pat__offset,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set__flag_ID,
            COMP__bnit__set,
            1,
            COMP__pat__flag,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set__string,
            COMP__bnit__set,
            1,
            COMP__pat__literal__string,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
        
        // io
        COMP__abt__define_call,
            COMP__act__io__cell_to_address,
            COMP__bnit__io__cell_to_address,
            3,
            COMP__pat__flag,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__io__address_to_cell,
            COMP__bnit__io__address_to_cell,
            3,
            COMP__pat__flag,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__io__file_to_buffer,
            COMP__bnit__io__file_to_buffer,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__io__buffer_to_file,
            COMP__bnit__io__buffer_to_file,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
        
        // copies
        COMP__abt__define_call,
            COMP__act__copy__cell,
            COMP__bnit__copy__cell,
            1,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__copy__buffer,
            COMP__bnit__copy__buffer,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
        
        // memory
        COMP__abt__define_call,
            COMP__act__memory__request_memory,
            COMP__bnit__memory__request_memory,
            1,
            COMP__pat__variable,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__memory__return_memory,
            COMP__bnit__memory__return_memory,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            0,
        
        // buffers
        COMP__abt__define_call,
            COMP__act__buffer__calculate_length,
            COMP__bnit__buffer__calculate_length,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        
        // casting
        COMP__abt__define_call,
            COMP__act__cast__cell_to_unsigned_integer_string,
            COMP__bnit__cast__cell_to_unsigned_integer_string,
            1,
            COMP__pat__variable,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
        
        // printing
        COMP__abt__define_call,
            COMP__act__print__signed_integer,
            COMP__bnit__print__signed_integer,
            1,
            COMP__pat__variable,
            0,
        COMP__abt__define_call,
            COMP__act__print__unsigned_integer,
            COMP__bnit__print__unsigned_integer,
            1,
            COMP__pat__variable,
            0,
        COMP__abt__define_call,
            COMP__act__print__character,
            COMP__bnit__print__character,
            1,
            COMP__pat__variable,
            0,
        COMP__abt__define_call,
            COMP__act__print__buffer_as_string,
            COMP__bnit__print__buffer_as_string,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            0,
        COMP__abt__define_call,
            COMP__act__print__binary,
            COMP__bnit__print__binary,
            1,
            COMP__pat__variable,
            0,
        
        // integer operations
        COMP__abt__define_call,
            COMP__act__integer__add,
            COMP__bnit__integer_add,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__integer__subtract,
            COMP__bnit__integer_subtract,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__integer__multiply,
            COMP__bnit__integer_multiply,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__integer__divide,
            COMP__bnit__integer_divide,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__integer__modulous,
            COMP__bnit__integer_modulous,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__integer__within_range,
            COMP__bnit__integer_within_range,
            4,
            COMP__pat__variable,
            COMP__pat__variable,
            COMP__pat__variable,
            COMP__pat__flag,
            1,
            COMP__pat__flag,
        
        // binary operations
        COMP__abt__define_call,
            COMP__act__binary__or,
            COMP__bnit__binary__or,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__binary__invert,
            COMP__bnit__binary__invert,
            1,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__binary__and,
            COMP__bnit__binary__and,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__binary__xor,
            COMP__bnit__binary__xor,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__binary__shift_higher,
            COMP__bnit__binary__shift_higher,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__binary__shift_lower,
            COMP__bnit__binary__shift_lower,
            2,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__binary__overwrite,
            COMP__bnit__binary__overwrite,
            3,
            COMP__pat__variable,
            COMP__pat__variable,
            COMP__pat__variable,
            1,
            COMP__pat__variable,

        // flags
        COMP__abt__define_call,
            COMP__act__flag__get,
            COMP__bnit__flag__get,
            1,
            COMP__pat__flag,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__flag__set,
            COMP__bnit__flag__set,
            1,
            COMP__pat__variable,
            1,
            COMP__pat__flag,
        COMP__abt__define_call,
            COMP__act__flag__invert,
            COMP__bnit__flag__invert,
            1,
            COMP__pat__flag,
            1,
            COMP__pat__flag,
        COMP__abt__define_call,
            COMP__act__flag__or,
            COMP__bnit__flag__or,
            2,
            COMP__pat__flag,
            COMP__pat__flag,
            1,
            COMP__pat__flag,
        COMP__abt__define_call,
            COMP__act__flag__and,
            COMP__bnit__flag__and,
            2,
            COMP__pat__flag,
            COMP__pat__flag,
            1,
            COMP__pat__flag,
        COMP__abt__define_call,
            COMP__act__flag__xor,
            COMP__bnit__flag__xor,
            2,
            COMP__pat__flag,
            COMP__pat__flag,
            1,
            COMP__pat__flag,
        
        // jumps
        COMP__abt__define_call,
            COMP__act__jump,
            COMP__bnit__jump,
            2,
            COMP__pat__flag,
            COMP__pat__offset,
            0,
        
        // etc
        COMP__abt__define_call,
            COMP__act__reset_error_code_cell,
            COMP__bnit__reset_error_code,
            0,
            0,
        COMP__abt__end_of_blueprint,
    };

    // open output
    ANVIL__list output = COMP__open__list(sizeof(COMP__accountling_abstraction_header) * 256, error);
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // setup blueprint current
    COMP__current current_blueprintling = ANVIL__create__buffer((ANVIL__address)blueprint, (ANVIL__address)(blueprint + sizeof(blueprint) - 1));

    // create and add predefined headers to list
    while (COMP__check__current_within_range(current_blueprintling) && (*(COMP__blueprintling*)current_blueprintling.start) == COMP__abt__define_call) {
        // check for define call
        if ((*(COMP__blueprintling*)current_blueprintling.start) == COMP__abt__define_call) {
            // advance current
            current_blueprintling.start += sizeof(COMP__blueprintling);
        } else {
            *error = COMP__open__internal_memory_error();

            return output;
        }

        // open new header
        COMP__accountling_abstraction_header header;
        
        // set statement type
        header.header.type = COMP__st__abstraction_header;

        // set call type
        header.call_index = (COMP__act)(*(COMP__blueprintling*)current_blueprintling.start);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // set name
        header.header.name = COMP__create__parsling_argument(COMP__pat__variable, COMP__create__name(COMP__create__lexling(COMP__lt__name, ANVIL__open__buffer_from_string((ANVIL__u8*)COMP__global__accountling_call_type_name_strings[*(COMP__blueprintling*)current_blueprintling.start], ANVIL__bt__false, ANVIL__bt__false), COMP__create_null__character_location())), 0);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // get input count
        header.header.input_count = (*(COMP__blueprintling*)current_blueprintling.start);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // open inputs
        header.header.inputs = COMP__open__list(sizeof(COMP__parsling_argument) * 8, error);

        // get inputs
        for (COMP__blueprintling i = 0; i < header.header.input_count; i++) {
            // add argument
            COMP__append__parsling_argument(&header.header.inputs, COMP__create__parsling_argument((COMP__pat)(*(COMP__blueprintling*)current_blueprintling.start), COMP__create__name(COMP__create__lexling(COMP__lt__name, ANVIL__open__buffer_from_string((u8*)"(internal)", ANVIL__bt__false, ANVIL__bt__false), COMP__create_null__character_location())), 0), error);

            // next blueprintling
            current_blueprintling.start += sizeof(COMP__blueprintling);
        }

        // get output count
        header.header.output_count = (*(COMP__blueprintling*)current_blueprintling.start);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // open outputs
        header.header.outputs = COMP__open__list(sizeof(COMP__parsling_argument) * 8, error);

        // get outputs
        for (COMP__blueprintling i = 0; i < header.header.output_count; i++) {
            // add argument
            COMP__append__parsling_argument(&header.header.outputs, COMP__create__parsling_argument((COMP__pat)(*(COMP__blueprintling*)current_blueprintling.start), COMP__create__name(COMP__create__lexling(COMP__lt__name, ANVIL__open__buffer_from_string((u8*)"(internal)", ANVIL__bt__false, ANVIL__bt__false), COMP__create_null__character_location())), 0), error);

            // next blueprintling
            current_blueprintling.start += sizeof(COMP__blueprintling);
        }

        // append header
        COMP__append__accountling_abstraction_header(&output, header, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }
    }

    // get all user defined headers
    // setup current
    COMP__current current_file = COMP__calculate__current_from_list_filled_index(&parsling_programs);

    // setup abstraction call ID
    COMP__call_index abstraction_call_ID = 0;

    // across all files
    while (COMP__check__current_within_range(current_file)) {
        // setup current
        COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&(*(COMP__parsling_program*)current_file.start).abstractions);

        // get each abstraction header
        while (COMP__check__current_within_range(current_abstraction)) {
            // get abstraction header
            COMP__accountling_abstraction_header header = COMP__account__get_abstraction_header(*(COMP__parsling_abstraction*)current_abstraction.start, abstraction_call_ID, error);

            // next abstraction call ID
            abstraction_call_ID++;

            // check for error
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // check to see if header already exists
            if (COMP__find__accountling_header_index(output, header.header) < COMP__calculate__call_blueprint_entry_count(output)) {
                // setup error
                *error = COMP__open__error("Accounting Error: An abstraction is already defined.", header.header.name.text.lexling.location);

                return output;
            }

            // append header
            COMP__append__accountling_abstraction_header(&output, header, error);
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // next abstraction
            current_abstraction.start += sizeof(COMP__parsling_abstraction);
        }

        // next file
        current_file.start += sizeof(COMP__parsling_program);
    }

    return output;
}

// verify all statements across all files
void COMP__account__verify_all_calls(ANVIL__list parsling_programs, ANVIL__list call_blueprint, COMP__error* error) {
    // setup current file
    COMP__current current_file = COMP__calculate__current_from_list_filled_index(&parsling_programs);

    // across all files
    while (COMP__check__current_within_range(current_file)) {
        // setup current abstraction
        COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&(*(COMP__parsling_program*)current_file.start).abstractions);

        // across all abstractions
        while (COMP__check__current_within_range(current_abstraction)) {
            // setup current statment
            COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&(*(COMP__parsling_abstraction*)current_abstraction.start).statements);

            // across all statements
            while (COMP__check__current_within_range(current_statement)) {
                // setup header to search for
                COMP__parsling_statement searching_for = *(COMP__parsling_statement*)current_statement.start;

                // if the statement is a call
                if (searching_for.type == COMP__st__abstraction_call) {
                    // verify header exists
                    if (COMP__find__accountling_header_index(call_blueprint, searching_for) >= COMP__calculate__call_blueprint_entry_count(call_blueprint)) {
                        // does not exist
                        *error = COMP__open__error("Accounting Error: A statement calls a non-existent abstraction / instruction.", searching_for.name.text.lexling.location);

                        return;
                    }
                }

                // next statement
                current_statement.start += sizeof(COMP__parsling_statement);
            }

            // next abstraction
            current_abstraction.start += sizeof(COMP__parsling_abstraction);
        }

        // next file
        current_file.start += sizeof(COMP__parsling_program);
    }

    // no errors, valid
    return;
}

// accountling program
typedef struct COMP__accountling_program {
    ANVIL__list call_blueprint; // COMP__accountling_abstraction_header
    ANVIL__list abstractions; // COMP__accountling_abstraction
    ANVIL__list predefined_variables; // COMP__parsling_argument
    ANVIL__list predefined_flags; // COMP__parsling_argument
    COMP__call_index main_abstraction_ID;
} COMP__accountling_program;

// create custom accountling program
COMP__accountling_program COMP__create__accountling_program(ANVIL__list call_blueprint, ANVIL__list abstractions, ANVIL__list predefined_variables, ANVIL__list predefined_flags, COMP__call_index main_abstraction_ID) {
    COMP__accountling_program output;

    output.call_blueprint = call_blueprint;
    output.abstractions = abstractions;
    output.predefined_variables = predefined_variables;
    output.predefined_flags = predefined_flags;
    output.main_abstraction_ID = main_abstraction_ID;

    return output;
}

// create null accountling program
COMP__accountling_program COMP__create_null__accountling_program() {
    return COMP__create__accountling_program(ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list(), COMP__define__null_call_ID);
}

// close accountling program
void COMP__close__accountling_program(COMP__accountling_program program) {
    // close predefined blueprints
    // get current
    COMP__current current_header = COMP__calculate__current_from_list_filled_index(&program.call_blueprint);

    // close all predefined blueprints
    while (COMP__check__current_within_range(current_header)) {
        // get header
        COMP__accountling_abstraction_header header = *(COMP__accountling_abstraction_header*)current_header.start;

        // if a predefined header
        if (header.call_index < COMP__act__user_defined) {
            // close header
            COMP__close__accountling_abstraction_header(header);
        }

        // next header
        current_header.start += sizeof(COMP__accountling_abstraction_header);
    }

    // close blueprint list
    ANVIL__close__list(program.call_blueprint);

    // close abstractions
    // check abstraction list exists
    if (ANVIL__check__empty_list(program.abstractions)) {
        return;
    }

    // get current
    COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&program.abstractions);

    // close each abstraction
    while (COMP__check__current_within_range(current_abstraction)) {
        COMP__close__accountling_abstraction(*(COMP__accountling_abstraction*)current_abstraction.start);

        // next abstraction
        current_abstraction.start += sizeof(COMP__accountling_abstraction);
    }

    // close abstraction list
    ANVIL__close__list(program.abstractions);

    // close predefineds
    ANVIL__close__list(program.predefined_variables);
    ANVIL__close__list(program.predefined_flags);

    return;
}

// convert string to converted string
ANVIL__buffer COMP__account__convert_string(COMP__parsling_argument string, COMP__error* error) {
    ANVIL__buffer output = ANVIL__create_null__buffer();
    ANVIL__length character_count = 0;

    // count characters
    for (COMP__character_index i = 1; i < ANVIL__calculate__buffer_length(string.text.lexling.value) - 1; i++) {
        // check for escape sequence
        if (((ANVIL__character*)string.text.lexling.value.start)[i] == '%') {
            // check for characters available
            if (i + 3 <= ANVIL__calculate__buffer_length(string.text.lexling.value) - 1) {
                // validate characters
                ANVIL__bt invalid_hexadecimal_character_1;
                ANVIL__bt invalid_hexadecimal_character_2;
                ANVIL__bt semi_colon_missing;
                COMP__translate__character_to_hexadecimal(((ANVIL__character*)string.text.lexling.value.start)[i + 1], &invalid_hexadecimal_character_1);
                COMP__translate__character_to_hexadecimal(((ANVIL__character*)string.text.lexling.value.start)[i + 2], &invalid_hexadecimal_character_2);
                semi_colon_missing = (ANVIL__bt)(((ANVIL__character*)string.text.lexling.value.start)[i + 3] != ';');

                // determine validity
                if (invalid_hexadecimal_character_1 != ANVIL__bt__false || invalid_hexadecimal_character_2 != ANVIL__bt__false || semi_colon_missing != ANVIL__bt__false) {
                    // invalid escape sequence
                    *error = COMP__open__error("Accounting Error: String literal has invalid escape sequences.", string.text.lexling.location);
                    
                    return output;
                }

                // skip past characters
                i += 3;
            // error
            } else {
                *error = COMP__open__error("Accounting Error: String literal has invalid escape sequences.", string.text.lexling.location);

                return output;
            }
        }

        // next character
        character_count++;
    }

    // check for empty string
    if (character_count == 0) {
        // return empty string
        return output;
    }

    // allocate string
    output = ANVIL__open__buffer(character_count);
    if (ANVIL__check__empty_buffer(output)) {
        *error = COMP__open__internal_memory_error();

        return output;
    }

    // translate string
    character_count = 0;
    for (COMP__character_index i = 1; i < ANVIL__calculate__buffer_length(string.text.lexling.value) - 1; i++) {
        // check for escape sequence
        if (((ANVIL__character*)string.text.lexling.value.start)[i] == '%') {
            // validate characters
            ANVIL__bt invalid_hexadecimal_character_1;
            ANVIL__bt invalid_hexadecimal_character_2;
            ANVIL__character a = COMP__translate__character_to_hexadecimal(((ANVIL__character*)string.text.lexling.value.start)[i + 1], &invalid_hexadecimal_character_1);
            ANVIL__character b = COMP__translate__character_to_hexadecimal(((ANVIL__character*)string.text.lexling.value.start)[i + 2], &invalid_hexadecimal_character_2);

            // write character
            ((ANVIL__character*)output.start)[character_count] = b + (a << 4);
            
            // skip past characters
            i += 3;
        } else {
            // write character
            ((ANVIL__character*)output.start)[character_count] = ((ANVIL__character*)string.text.lexling.value.start)[i];
        }

        // next character
        character_count++;
    }

    return output;
}

// check to see if argument is in list
COMP__parsling_argument COMP__account__get_argument_in_list__by_text(ANVIL__list* arguments, COMP__parsling_argument searching_for, ANVIL__bt* found) {
    // setup current
    COMP__current current_argument = COMP__calculate__current_from_list_filled_index(arguments);

    // search for argument by name
    while (COMP__check__current_within_range(current_argument)) {
        // get current argument
        COMP__parsling_argument argument = *(COMP__parsling_argument*)current_argument.start;

        // if names are identical
        if (COMP__check__parsling_arguments_have_same_text(argument, searching_for)) {
            // found in list
            *found = ANVIL__bt__true;
            return argument;
        }

        // next argument
        current_argument.start += sizeof(COMP__parsling_argument);
    }

    // not in list
    *found = ANVIL__bt__false;
    return COMP__create_null__parsling_argument();;
}

// get accountling argument list
ANVIL__list COMP__account__accountling_argument_list(COMP__accountling_abstraction* abstraction, ANVIL__list parsling_arguments, COMP__error* error) {
    // open list
    ANVIL__list output = COMP__open__list(sizeof(COMP__accountling_argument) * 16, error);
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // get arguments
    // setup current
    COMP__current current_argument = COMP__calculate__current_from_list_filled_index(&parsling_arguments);

    // get arguments
    while (COMP__check__current_within_range(current_argument)) {
        // get argument
        COMP__parsling_argument argument = *(COMP__parsling_argument*)current_argument.start;

        // create argument ID by type
        if (argument.type == COMP__pat__variable__input) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).inputs, argument), argument), error);
        } else if (argument.type == COMP__pat__variable__output) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).outputs, argument), argument), error);
        } else if (argument.type == COMP__pat__variable || argument.type == COMP__pat__variable__body) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).variables, argument), argument), error);
        } else if (argument.type == COMP__pat__variable__double) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).doubles, argument), argument), error);
        } else if (argument.type == COMP__pat__literal__boolean || argument.type == COMP__pat__literal__binary || argument.type == COMP__pat__literal__integer || argument.type == COMP__pat__literal__hexadecimal) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, argument.value, argument), error);
        } else if (argument.type == COMP__pat__offset) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).offsets, argument), argument), error);
        } else if (argument.type == COMP__pat__variable__predefined) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name(*(*abstraction).predefined_variables, argument), argument), error);
        } else if (argument.type == COMP__pat__flag__predefined) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name(*(*abstraction).predefined_flags, argument), argument), error);
        } else if (argument.type == COMP__pat__flag__user_defined) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).flags, argument), argument), error);
        } else if (argument.type == COMP__pat__literal__string) {
            COMP__append__accountling_argument(&output, COMP__create__accountling_argument(argument.type, COMP__find__parsling_argument_index__by_name((*abstraction).strings, argument), argument), error);
        } else {
            // error
            *error = COMP__open__error("Internal Error: Unsupported argument type in accountling argument list.", argument.text.lexling.location);

            return output;
        }

        // check for error
        if (COMP__check__error_occured(error)) {
            break;
        }

        // next argument
        current_argument.start += sizeof(COMP__parsling_argument);
    }

    return output;
}

// account an abstraction
COMP__accountling_abstraction COMP__account__abstraction(ANVIL__list call_blueprint, ANVIL__list* predefined_variables, ANVIL__list* predefined_flags, COMP__parsling_abstraction parsling_abstraction, COMP__error* error) {
    COMP__accountling_abstraction output = COMP__create_null__accountling_abstraction();

    // get name
    output.header = parsling_abstraction.header;

    // get variables
    // validate that all variables are not used before declared
    {
        // get predefined variables
        output.predefined_variables = predefined_variables;

        // get abstraction inputs
        {
            // open inputs
            output.inputs = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // setup current
            COMP__current current_input = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.header.inputs);

            // get inputs
            while (COMP__check__current_within_range(current_input)) {
                // get argument to look for
                COMP__parsling_argument searching_for = *(COMP__parsling_argument*)current_input.start;

                // get found variable back
                ANVIL__bt found_predefined;
                ANVIL__bt found_variable;
                COMP__account__get_argument_in_list__by_text(predefined_variables, searching_for, &found_predefined);
                COMP__account__get_argument_in_list__by_text(&output.inputs, searching_for, &found_variable);

                // if variable already exists
                if (found_predefined == ANVIL__bt__true) {
                    // variable declared twice
                    *error = COMP__open__error("Accounting Error: A predefined variable was illegally used as an input.", searching_for.text.lexling.location);

                    return output;
                }

                // modify variable type
                searching_for.type = COMP__pat__variable__input;

                // if variable already exists
                if (found_variable == ANVIL__bt__true) {
                    // variable declared twice
                    *error = COMP__open__error("Accounting Error: A duplicate header input was detected.", searching_for.text.lexling.location);

                    return output;
                }

                // add variable
                COMP__append__parsling_argument(&output.inputs, searching_for, error);
                if (COMP__check__error_occured(error)) {
                    return output;
                }

                // next argument
                current_input.start += sizeof(COMP__parsling_argument);
            }
        }

        // get abstraction outputs
        {
            // open outputs
            output.outputs = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // setup current
            COMP__current current_output = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.header.outputs);

            // get outputs
            while (COMP__check__current_within_range(current_output)) {
                // get argument to look for
                COMP__parsling_argument searching_for = *(COMP__parsling_argument*)current_output.start;

                // get found variable back
                ANVIL__bt found_predefined;
                ANVIL__bt found_variable;
                COMP__account__get_argument_in_list__by_text(predefined_variables, searching_for, &found_predefined);
                COMP__account__get_argument_in_list__by_text(&output.outputs, searching_for, &found_variable);

                // if variable already exists
                if (found_predefined == ANVIL__bt__true) {
                    // variable declared twice
                    *error = COMP__open__error("Accounting Error: A predefined variable was illegally used as an output.", searching_for.text.lexling.location);

                    return output;
                }

                // modify variable type
                searching_for.type = COMP__pat__variable__output;

                // if variable already exists
                if (found_variable == ANVIL__bt__true) {
                    // variable declared twice
                    *error = COMP__open__error("Accounting Error: A duplicate header output was detected.", searching_for.text.lexling.location);

                    return output;
                }

                // add variable
                COMP__append__parsling_argument(&output.outputs, searching_for, error);
                if (COMP__check__error_occured(error)) {
                    return output;
                }

                // next argument
                current_output.start += sizeof(COMP__parsling_argument);
            }
        }

        // get abstraction doubles
        {
            // open doubles
            output.doubles = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // setup current
            COMP__current current_input = COMP__calculate__current_from_list_filled_index(&output.inputs);

            // get outputs
            while (COMP__check__current_within_range(current_input)) {
                // get argument to look for
                COMP__parsling_argument searching_for = *(COMP__parsling_argument*)current_input.start;

                // check for output duplicate
                ANVIL__bt found_duplicate;
                COMP__account__get_argument_in_list__by_text(&output.outputs, searching_for, &found_duplicate);

                // check for double
                if (found_duplicate == ANVIL__bt__true) {
                    // modify variable type
                    searching_for.type = COMP__pat__variable__double;

                    // add variable
                    COMP__append__parsling_argument(&output.doubles, searching_for, error);
                    if (COMP__check__error_occured(error)) {
                        return output;
                    }
                }

                // next argument
                current_input.start += sizeof(COMP__parsling_argument);
            }
        }

        // get abstraction body variables
        {
            // open variables
            output.variables = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // setup current
            COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

            // across all statements
            while (COMP__check__current_within_range(current_statement)) {
                // get one statement
                COMP__parsling_statement statement = *(COMP__parsling_statement*)current_statement.start;

                // if statement is abstraction call
                if (statement.type == COMP__st__abstraction_call) {
                    // validate inputs
                    // setup current
                    COMP__current current_input = COMP__calculate__current_from_list_filled_index(&statement.inputs);

                    // check all inputs
                    while (COMP__check__current_within_range(current_input)) {
                        // get input
                        COMP__parsling_argument argument = *(COMP__parsling_argument*)current_input.start;

                        // if current argument is a variable
                        if (argument.type == COMP__pat__variable) {
                            // assuming its not already defined
                            ANVIL__bt found_predefined;
                            COMP__account__get_argument_in_list__by_text(output.predefined_variables, argument, &found_predefined);
                            if (found_predefined == ANVIL__bt__true) {
                                goto next_input;
                            }

                            // check if argument already exists
                            ANVIL__bt found_input;
                            ANVIL__bt found_body;
                            COMP__account__get_argument_in_list__by_text(&output.inputs, argument, &found_input);
                            COMP__account__get_argument_in_list__by_text(&output.variables, argument, &found_body);
                            if ((found_input || found_body) == ANVIL__bt__false) {
                                *error = COMP__open__error("Accounting Error: A variable was used before it was declared.", argument.text.lexling.location);

                                return output;
                            }
                        }

                        // next input
                        next_input:
                        current_input.start += sizeof(COMP__parsling_argument);
                    }

                    // validate outputs
                    // setup current
                    COMP__current current_output = COMP__calculate__current_from_list_filled_index(&statement.outputs);

                    // check all outputs
                    while (COMP__check__current_within_range(current_output)) {
                        // get output
                        COMP__parsling_argument argument = *(COMP__parsling_argument*)current_output.start;
                    
                        // if current argument is a variable
                        if (argument.type == COMP__pat__variable) {
                            // assuming its not already defined
                            ANVIL__bt found_predefined;
                            COMP__account__get_argument_in_list__by_text(output.predefined_variables, argument, &found_predefined);
                            if (found_predefined == ANVIL__bt__true) {
                                *error = COMP__open__error("Accounting Error: Predefined variables cannot be written to.", argument.text.lexling.location);

                                return output;
                            }

                            // check if argument already exists
                            ANVIL__bt found_input;
                            ANVIL__bt found_output;
                            ANVIL__bt found_body;
                            COMP__account__get_argument_in_list__by_text(&output.inputs, argument, &found_input);
                            COMP__account__get_argument_in_list__by_text(&output.outputs, argument, &found_output);
                            COMP__account__get_argument_in_list__by_text(&output.variables, argument, &found_body);
                            if ((found_input || found_output || found_body) == ANVIL__bt__false) {
                                // setup argument type
                                argument.type = COMP__pat__variable__body;

                                // create variable
                                COMP__append__parsling_argument(&output.variables, argument, error);
                                if (COMP__check__error_occured(error)) {
                                    return output;
                                }
                            }
                        }

                        // next output
                        current_output.start += sizeof(COMP__parsling_argument);
                    }
                }

                // next statement
                current_statement.start += sizeof(COMP__parsling_statement);
            }
        }
    }

    // get offsets
    {
        // open list
        output.offsets = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }

        // setup current
        COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

        // each statement
        while (COMP__check__current_within_range(current_statement)) {
            COMP__parsling_statement statement = *(COMP__parsling_statement*)current_statement.start;

            // if a statement offset
            if (statement.type == COMP__st__offset) {
                // if offset doesnt already exist
                ANVIL__bt found_offset;
                COMP__account__get_argument_in_list__by_text(&output.offsets, statement.name, &found_offset);
                if (found_offset == ANVIL__bt__false) {
                    // append offset declaration
                    COMP__append__parsling_argument(&output.offsets, statement.name, error);
                    if (COMP__check__error_occured(error)) {
                        return output;
                    }
                // already defined, error
                } else {
                    *error = COMP__open__error("Accounting Error: Offset is declared more than once.", statement.name.text.lexling.location);

                    return output;
                }
            }

            // next statement
            current_statement.start += sizeof(COMP__parsling_statement);
        }

        // verify all offsets used exist
        // setup current statement
        current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

        // for each statement
        while (COMP__check__current_within_range(current_statement)) {
            // get statement
            COMP__parsling_statement statement = *(COMP__parsling_statement*)current_statement.start;

            // if statement is abstraction call
            if (statement.type == COMP__st__abstraction_call) {
                // setup current input
                COMP__current current_input = COMP__calculate__current_from_list_filled_index(&statement.inputs);

                // for each input
                while (COMP__check__current_within_range(current_input)) {
                    // get input
                    COMP__parsling_argument input_argument = *(COMP__parsling_argument*)current_input.start;

                    // if argument is offset
                    if (input_argument.type == COMP__pat__offset) {
                        // check if input exists
                        ANVIL__bt found_offset;
                        COMP__account__get_argument_in_list__by_text(&output.offsets, input_argument, &found_offset);
                        if (found_offset == ANVIL__bt__false) {
                            // offset nonexistent
                            *error = COMP__open__error("Accounting Error: Offset was used but never declared.", input_argument.text.lexling.location);

                            return output;
                        }
                    }

                    // next argument
                    current_input.start += sizeof(COMP__parsling_argument);
                }

                // setup current output
                COMP__current current_output = COMP__calculate__current_from_list_filled_index(&statement.outputs);

                // for each output
                while (COMP__check__current_within_range(current_output)) {
                    // get output
                    COMP__parsling_argument output_argument = *(COMP__parsling_argument*)current_output.start;

                    // if argument is offset
                    if (output_argument.type == COMP__pat__offset) {
                        // check if output exists
                        ANVIL__bt found_offset;
                        COMP__account__get_argument_in_list__by_text(&output.offsets, output_argument, &found_offset);
                        if (found_offset == ANVIL__bt__false) {
                            // offset nonexistent
                            *error = COMP__open__error("Accounting Error: Offset was used but never declared.", output_argument.text.lexling.location);

                            return output;
                        }
                    }

                    // next argument
                    current_output.start += sizeof(COMP__parsling_argument);
                }
            }

            // next statement
            current_statement.start += sizeof(COMP__parsling_statement);
        }
    }

    // get flags
    {
        // setup predefined flags
        output.predefined_flags = predefined_flags;

        // open list
        output.flags = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }

        // verify all flags used exist
        // setup current statement
        COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

        // for each statement
        while (COMP__check__current_within_range(current_statement)) {
            // get statement
            COMP__parsling_statement statement = *(COMP__parsling_statement*)current_statement.start;

            // if statement is abstraction call
            if (statement.type == COMP__st__abstraction_call) {
                // setup current input
                COMP__current current_input = COMP__calculate__current_from_list_filled_index(&statement.inputs);

                // for each input
                while (COMP__check__current_within_range(current_input)) {
                    // get input
                    COMP__parsling_argument input_argument = *(COMP__parsling_argument*)current_input.start;

                    // if argument is flag
                    if (input_argument.type == COMP__pat__flag) {
                        // check if flag exists
                        ANVIL__bt found_predefined_flag;
                        ANVIL__bt found_user_defined_flag;
                        COMP__account__get_argument_in_list__by_text(output.predefined_flags, input_argument, &found_predefined_flag);
                        COMP__account__get_argument_in_list__by_text(&output.flags, input_argument, &found_user_defined_flag);
                        if (found_predefined_flag == ANVIL__bt__false && found_user_defined_flag == ANVIL__bt__false) {
                            // flag nonexistent
                            *error = COMP__open__error("Accounting Error: Flag was used but never declared.", input_argument.text.lexling.location);

                            return output;
                        }
                    }

                    // next argument
                    current_input.start += sizeof(COMP__parsling_argument);
                }

                // setup current output
                COMP__current current_output = COMP__calculate__current_from_list_filled_index(&statement.outputs);

                // for each output
                while (COMP__check__current_within_range(current_output)) {
                    // get output
                    COMP__parsling_argument output_argument = *(COMP__parsling_argument*)current_output.start;

                    // if argument is flag
                    if (output_argument.type == COMP__pat__flag) {
                        // check if flag exists
                        ANVIL__bt found_predefined_flag;
                        ANVIL__bt found_user_defined_flag;
                        COMP__account__get_argument_in_list__by_text(output.predefined_flags, output_argument, &found_predefined_flag);
                        COMP__account__get_argument_in_list__by_text(&output.flags, output_argument, &found_user_defined_flag);
                        if (found_predefined_flag == ANVIL__bt__true) {
                            // predefined flags cannot be set, error
                            *error = COMP__open__error("Accounting Error: Predefined flags cannot be written to.", output_argument.text.lexling.location);

                            return output;
                        }
                        if (found_user_defined_flag == ANVIL__bt__false) {
                            // declare new flag
                            COMP__append__parsling_argument(&output.flags, output_argument, error);
                            if (COMP__check__error_occured(error)) {
                                return output;
                            }
                        }
                    }

                    // next argument
                    current_output.start += sizeof(COMP__parsling_argument);
                }
            }

            // next statement
            current_statement.start += sizeof(COMP__parsling_statement);
        }
    }

    // get strings
    {
        // open strings
        output.strings = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }
        output.converted_strings = COMP__open__list(sizeof(ANVIL__buffer) * 16, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }

        // get current statement
        COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

        // for each statement
        while (COMP__check__current_within_range(current_statement)) {
            // get statement
            COMP__parsling_statement statement = *(COMP__parsling_statement*)current_statement.start;

            // if statement is call
            if (statement.type == COMP__st__abstraction_call) {
                // get inputs
                COMP__current current_input = COMP__calculate__current_from_list_filled_index(&statement.inputs);

                // for each input
                while (COMP__check__current_within_range(current_input)) {
                    // get input
                    COMP__parsling_argument* input = (COMP__parsling_argument*)current_input.start;

                    // if argument is string
                    if (input->type == COMP__pat__literal__string) {
                        // append string
                        COMP__append__parsling_argument(&output.strings, *input, error);
                        if (COMP__check__error_occured(error)) {
                            return output;
                        }

                        // translate string data
                        COMP__append__buffer(&output.converted_strings, COMP__account__convert_string(*input, error), error);
                        if (COMP__check__error_occured(error)) {
                            return output;
                        }
                    }

                    // next input
                    current_input.start += sizeof(COMP__parsling_argument);
                }
            }

            // next statement
            current_statement.start += sizeof(COMP__parsling_statement);
        }
    }

    // modify parsling statements to reflect accurate variable types (predefined, input, output, double & body) & flag types (predefined & user defined)
    {
        // get current statement
        COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

        // for each statement
        while (COMP__check__current_within_range(current_statement)) {
            // get statement
            COMP__parsling_statement statement = *(COMP__parsling_statement*)current_statement.start;

            // if statement is call
            if (statement.type == COMP__st__abstraction_call) {
                // modify inputs
                // get inputs
                COMP__current current_input = COMP__calculate__current_from_list_filled_index(&statement.inputs);

                // for each input
                while (COMP__check__current_within_range(current_input)) {
                    // get input
                    COMP__parsling_argument* input = (COMP__parsling_argument*)current_input.start;

                    // if argument is variable
                    if (input->type == COMP__pat__variable) {
                        // find variable by type
                        ANVIL__bt found_predefined;
                        ANVIL__bt found_double;
                        ANVIL__bt found_input;
                        ANVIL__bt found_output;
                        ANVIL__bt found_body;
                        COMP__account__get_argument_in_list__by_text(output.predefined_variables, *input, &found_predefined);
                        COMP__account__get_argument_in_list__by_text(&output.doubles, *input, &found_double);
                        COMP__account__get_argument_in_list__by_text(&output.inputs, *input, &found_input);
                        COMP__account__get_argument_in_list__by_text(&output.outputs, *input, &found_output);
                        COMP__account__get_argument_in_list__by_text(&output.variables, *input, &found_body);

                        // modify appropriately
                        if (found_predefined) {
                            input->type = COMP__pat__variable__predefined;
                        } else if (found_double) {
                            input->type = COMP__pat__variable__double;
                        } else if (found_input) {
                            input->type = COMP__pat__variable__input;
                        } else if (found_output) {
                            input->type = COMP__pat__variable__output;
                        } else if (found_body) {
                            input->type = COMP__pat__variable__body;
                        } else {
                            // error
                            *error = COMP__open__error("Internal Error: Unrecognized variable type when modifying statement variable types, oops.", input->text.lexling.location);
                        }
                    } else if (input->type == COMP__pat__flag) {
                        // find variable by type
                        ANVIL__bt found_predefined;
                        ANVIL__bt found_user_defined;
                        COMP__account__get_argument_in_list__by_text(output.predefined_flags, *input, &found_predefined);
                        COMP__account__get_argument_in_list__by_text(&output.flags, *input, &found_user_defined);

                        // modify appropriately
                        if (found_predefined) {
                            input->type = COMP__pat__flag__predefined;
                        } else if (found_user_defined) {
                            input->type = COMP__pat__flag__user_defined;
                        } else {
                            // error
                            *error = COMP__open__error("Internal Error: Unrecognized variable type when modifying statement input flag types, oops.", input->text.lexling.location);
                        }
                    }

                    // next input
                    current_input.start += sizeof(COMP__parsling_argument);
                }
                
                // modify outputs
                // get outputs
                COMP__current current_output = COMP__calculate__current_from_list_filled_index(&statement.outputs);

                // for each output
                while (COMP__check__current_within_range(current_output)) {
                    // get output
                    COMP__parsling_argument* output_argument = (COMP__parsling_argument*)current_output.start;

                    // if argument is variable
                    if (output_argument->type == COMP__pat__variable) {
                        // find variable by type
                        ANVIL__bt found_predefined;
                        ANVIL__bt found_double;
                        ANVIL__bt found_input;
                        ANVIL__bt found_output;
                        ANVIL__bt found_body;
                        COMP__account__get_argument_in_list__by_text(output.predefined_variables, *output_argument, &found_predefined);
                        COMP__account__get_argument_in_list__by_text(&output.doubles, *output_argument, &found_double);
                        COMP__account__get_argument_in_list__by_text(&output.inputs, *output_argument, &found_input);
                        COMP__account__get_argument_in_list__by_text(&output.outputs, *output_argument, &found_output);
                        COMP__account__get_argument_in_list__by_text(&output.variables, *output_argument, &found_body);

                        // modify appropriately
                        if (found_predefined) {
                            output_argument->type = COMP__pat__variable__predefined;
                        } else if (found_double) {
                            output_argument->type = COMP__pat__variable__double;
                        } else if (found_input) {
                            output_argument->type = COMP__pat__variable__input;
                        } else if (found_output) {
                            output_argument->type = COMP__pat__variable__output;
                        } else if (found_body) {
                            output_argument->type = COMP__pat__variable__body;
                        } else {
                            // error
                            *error = COMP__open__error("Internal Error: Unrecognized variable type when modifying statement variable types, oops.", output_argument->text.lexling.location);
                        }
                    } else if (output_argument->type == COMP__pat__flag) {
                        // find variable by type
                        ANVIL__bt found_predefined;
                        ANVIL__bt found_user_defined;
                        COMP__account__get_argument_in_list__by_text(output.predefined_flags, *output_argument, &found_predefined);
                        COMP__account__get_argument_in_list__by_text(&output.flags, *output_argument, &found_user_defined);

                        // modify appropriately
                        if (found_predefined) {
                            output_argument->type = COMP__pat__flag__predefined;
                        } else if (found_user_defined) {
                            output_argument->type = COMP__pat__flag__user_defined;
                        } else {
                            // error
                            *error = COMP__open__error("Internal Error: Unrecognized variable type when modifying statement output flag types, oops.", output_argument->text.lexling.location);
                        }
                    }

                    // next output
                    current_output.start += sizeof(COMP__parsling_argument);
                }
            }

            // next statement
            current_statement.start += sizeof(COMP__parsling_statement);
        }
    }

    // get statements
    {
        // open list
        output.statements = COMP__open__list(sizeof(COMP__accountling_statement) * 16, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }

        // setup offsets index
        COMP__offset_index offset_ID = 0;

        // convert statements
        // get current
        COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&parsling_abstraction.statements);

        // for each statement
        while (COMP__check__current_within_range(current_statement)) {
            // get statement
            COMP__parsling_statement parsling_statement = *(COMP__parsling_statement*)current_statement.start;

            // setup new statement
            COMP__accountling_statement accountling_statement;

            // translate statement
            // statement is a call
            if (parsling_statement.type == COMP__st__abstraction_call) {
                // set type
                accountling_statement.type = parsling_statement.type;

                // get accountling header index
                COMP__header_index header_index = COMP__find__accountling_header_index(call_blueprint, parsling_statement);
                if (header_index > COMP__calculate__call_blueprint_entry_count(call_blueprint)) {
                    *error = COMP__open__error("Internal Error: Header was not found during lookup.\n", parsling_statement.name.text.lexling.location);

                    return output;
                }

                // get header
                accountling_statement.header = ((COMP__accountling_abstraction_header*)call_blueprint.buffer.start)[header_index];

                // get io
                accountling_statement.inputs = COMP__account__accountling_argument_list(&output, parsling_statement.inputs, error);
                if (COMP__check__error_occured(error)) {
                    return output;
                }
                accountling_statement.outputs = COMP__account__accountling_argument_list(&output, parsling_statement.outputs, error);
                if (COMP__check__error_occured(error)) {
                    return output;
                }
            // statement is an offset
            } else if (parsling_statement.type == COMP__st__offset) {
                // set type
                accountling_statement.type = parsling_statement.type;

                // get offset index
                accountling_statement.offset_ID = offset_ID;

                // next offset
                offset_ID++;
            }

            // append statement
            COMP__append__accountling_statement(&output.statements, accountling_statement, error);

            // next statement
            current_statement.start += sizeof(COMP__parsling_statement);
        }
    }

    return output;
}

// print call header
void COMP__print__accounted_call_header(COMP__accountling_abstraction_header header) {
    // print ID
    printf("\t\tCall Index [%lu](user_defined=%lu) ", (ANVIL__u64)header.call_index, (ANVIL__u64)(header.call_index >= COMP__act__user_defined));

    // print statement
    COMP__print__parsed_statement(header.header);

    // new line
    printf("\n");

    return;
}

// print call blueprint
void COMP__print__call_blueprint(ANVIL__list blueprint) {
    // setup current
    COMP__current current = COMP__calculate__current_from_list_filled_index(&blueprint);

    // print all headers
    while (COMP__check__current_within_range(current)) {
        // print individual header
        COMP__print__accounted_call_header(*(COMP__accountling_abstraction_header*)current.start);

        // next header
        current.start += sizeof(COMP__accountling_abstraction_header);
    }

    return;
}

// append predefined variable
void COMP__generate_and_append__predefined_variable(ANVIL__list* list, const char* name, COMP__error* error) {
    // append
    COMP__append__parsling_argument(list, COMP__create__parsling_argument(COMP__pat__variable__predefined, COMP__create__name(COMP__create__lexling(COMP__lt__end_of_file, ANVIL__open__buffer_from_string((ANVIL__u8*)name, ANVIL__bt__false, ANVIL__bt__false), COMP__create__character_location(-1, -1, -1))), 0), error);

    return;
}

// generate predefined variables
ANVIL__list COMP__generate__predefined_variables(COMP__error* error) {
    ANVIL__list output;

    // open output
    output = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // append variables
    COMP__generate_and_append__predefined_variable(&output, COMP__global__predefined_cell_name_strings[COMP__pvt__error_code], error);

    return output;
}

// append predefined flag
void COMP__generate_and_append__predefined_flag(ANVIL__list* list, const char* name, COMP__error* error) {
    // append
    COMP__append__parsling_argument(list, COMP__create__parsling_argument(COMP__pat__flag__predefined, COMP__create__name(COMP__create__lexling(COMP__lt__end_of_file, ANVIL__open__buffer_from_string((ANVIL__u8*)name, ANVIL__bt__false, ANVIL__bt__false), COMP__create__character_location(-1, -1, -1))), 0), error);

    return;
}

// generate predefined flags
ANVIL__list COMP__generate__predefined_flags(COMP__error* error) {
    ANVIL__list output;

    // open output
    output = COMP__open__list(sizeof(COMP__parsling_argument) * 16, error);
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // append flags
    COMP__generate_and_append__predefined_flag(&output, COMP__global__predefined_flag_name_strings[COMP__pft__always_run], error);
    COMP__generate_and_append__predefined_flag(&output, COMP__global__predefined_flag_name_strings[COMP__pft__never_run], error);
    COMP__generate_and_append__predefined_flag(&output, COMP__global__predefined_flag_name_strings[COMP__pft__temporary], error);

    return output;
}

// account program
COMP__accountling_program COMP__account__program(ANVIL__list parsling_programs, COMP__error* error) {
    COMP__accountling_program output = COMP__create_null__accountling_program();

    // create headers
    output.call_blueprint = COMP__generate__call_blueprint(parsling_programs, error);
    if (COMP__check__error_occured(error)) {
        goto quit;
    }

    // verify statments in all user defined abstractions
    COMP__account__verify_all_calls(parsling_programs, output.call_blueprint, error);
    if (COMP__check__error_occured(error)) {
        goto quit;
    }

    // generate predefined variables
    output.predefined_variables = COMP__generate__predefined_variables(error);
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // generate predefined flags
    output.predefined_flags = COMP__generate__predefined_flags(error);
    if (COMP__check__error_occured(error)) {
        return output;
    }

    // allocate accountling abstraction list
    output.abstractions = COMP__open__list(sizeof(COMP__accountling_abstraction) * 16, error);
    if (COMP__check__error_occured(error)) {
        goto quit;
    }

    // setup current
    COMP__current current_parsling_program = COMP__calculate__current_from_list_filled_index(&parsling_programs);

    // account all programs
    while (COMP__check__current_within_range(current_parsling_program)) {
        // get program
        COMP__parsling_program parsling_program = *(COMP__parsling_program*)current_parsling_program.start;

        // setup current
        COMP__current current_parsling_abstraction = COMP__calculate__current_from_list_filled_index(&parsling_program.abstractions);

        // account all abstractions in program
        while (COMP__check__current_within_range(current_parsling_abstraction)) {
            // get parsling abstraction
            COMP__parsling_abstraction parsling_abstraction = *(COMP__parsling_abstraction*)current_parsling_abstraction.start;

            // account the parsling abstraction
            COMP__accountling_abstraction accountling_abstraction = COMP__account__abstraction(output.call_blueprint, &output.predefined_variables, &output.predefined_flags, parsling_abstraction, error);
            if (COMP__check__error_occured(error)) {
                // close abstraction
                COMP__close__accountling_abstraction(accountling_abstraction);

                // quit
                goto quit;
            }

            // append abstraction
            COMP__append__accountling_abstraction(&output.abstractions, accountling_abstraction, error);
            if (COMP__check__error_occured(error)) {
                goto quit;
            }

            // next abstraction
            current_parsling_abstraction.start += sizeof(COMP__parsling_abstraction);
        }

        // next program
        current_parsling_program.start += sizeof(COMP__parsling_program);
    }

    // search for main
    // setup current
    COMP__current current_header = COMP__calculate__current_from_list_filled_index(&output.call_blueprint);
    output.main_abstraction_ID = 0;

    // for each header
    while (COMP__check__current_within_range(current_header)) {
        // get header
        COMP__accountling_abstraction_header header = *(COMP__accountling_abstraction_header*)current_header.start;

        // check for correct title & io counts
        if (ANVIL__calculate__buffer_contents_equal(header.header.name.text.lexling.value, ANVIL__open__buffer_from_string((u8*)"main", ANVIL__bt__false, ANVIL__bt__false)) && header.header.input_count == 0 && header.header.output_count == 0) {
            break;
        }

        // next header
        output.main_abstraction_ID++;
        current_header.start += sizeof(COMP__accountling_abstraction_header);
    }

    // check to see if main was not found
    if (COMP__check__current_within_range(current_header) == ANVIL__bt__false) {
        // set error
        *error = COMP__open__error("Accounting Error: The entry point function main()() was not found.", COMP__create_null__character_location());

        goto quit;
    }

    // return
    quit:

    return output;
}

// print variable list
void COMP__print__accountling_variable_list(ANVIL__list* variables, COMP__tab_count tabs) {
    // setup current
    COMP__current current_variable = COMP__calculate__current_from_list_filled_index(variables);

    // print each argument
    while (COMP__check__current_within_range(current_variable)) {
        // print variable
        COMP__print__tabs(tabs);
        COMP__print__parsling_argument(*(COMP__parsling_argument*)current_variable.start);
        printf("\n");

        // next variable
        current_variable.start += sizeof(COMP__parsling_argument);
    }
    
    return;
}

// print accountling argument list
void COMP__print__accountling_arguments(ANVIL__list arguments) {
    // print opener
    printf("(");

    // setup current
    COMP__current current_argument = COMP__calculate__current_from_list_filled_index(&arguments);

    // for each argument
    while (COMP__check__current_within_range(current_argument)) {
        // get argument
        COMP__accountling_argument argument = *(COMP__accountling_argument*)current_argument.start;

        // print argument
        printf("[");
        ANVIL__print__buffer(COMP__convert__parsed_argument_type_to_string_buffer(argument.type));
        printf(":%lu]", argument.index);


        // next argument
        current_argument.start += sizeof(COMP__accountling_argument);
    }

    // print closer
    printf(")");

    return;
}

// print an accountling program
void COMP__print__accountling_program(COMP__accountling_program program) {
    // print section start
    printf("Accountlings:\n\tHeaders:\n");

    // print headers
    COMP__print__call_blueprint(program.call_blueprint);

    // print predefined variables
    printf("\tPredefined Variables:\n");

    // print variables
    COMP__print__accountling_variable_list(&program.predefined_variables, 2);

    // print predefined flags
    printf("\tPredefined Flags:\n");

    // print flags
    COMP__print__accountling_variable_list(&program.predefined_flags, 2);

    // print abstractions
    printf("\tAbstractions:\n");

    // setup current
    COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&program.abstractions);

    // print each abstraction
    while (COMP__check__current_within_range(current_abstraction)) {
        // get abstraction
        COMP__accountling_abstraction abstraction = *(COMP__accountling_abstraction*)current_abstraction.start;

        // print abstraction
        {
            // get call index
            COMP__call_index call_index = COMP__find__accountling_header_index(program.call_blueprint, abstraction.header);
            if (call_index >= COMP__calculate__call_blueprint_entry_count(program.call_blueprint)) {
                // call header not found, exit loop
                return;
            }

            // print header with call index
            printf("\t\t(%lu)", call_index);
            COMP__print__parsed_statement(abstraction.header);
            printf("\n");

            // if there are inputs
            if (abstraction.inputs.filled_index > 0) {
                // print inputs
                printf("\t\t\tInputs:\n");
                COMP__print__accountling_variable_list(&abstraction.inputs, 4);
            }

            // if there are outputs
            if (abstraction.outputs.filled_index > 0) {
                // print outputs
                printf("\t\t\tOutputs:\n");
                COMP__print__accountling_variable_list(&abstraction.outputs, 4);
            }

            // if there are doubles
            if (abstraction.doubles.filled_index > 0) {
                // print doubles
                printf("\t\t\tDoubles:\n");
                COMP__print__accountling_variable_list(&abstraction.doubles, 4);
            }

            // if there are variables
            if (abstraction.variables.filled_index > 0) {
                // print variables
                printf("\t\t\tVariables:\n");
                COMP__print__accountling_variable_list(&abstraction.variables, 4);
            }

            // if there are offsets
            if (abstraction.offsets.filled_index > 0) {
                // print offsets
                printf("\t\t\tOffsets:\n");
                COMP__print__accountling_variable_list(&abstraction.offsets, 4);
            }

            // if there are flags
            if (abstraction.flags.filled_index > 0) {
                // print flags
                printf("\t\t\tFlags:\n");
                COMP__print__accountling_variable_list(&abstraction.flags, 4);
            }

            // if there are statements
            if (abstraction.statements.filled_index > 0) {
                // print statements
                // print header
                printf("\t\t\tStatements:\n");

                // setup current
                COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&abstraction.statements);
                
                // for each statement
                while (COMP__check__current_within_range(current_statement)) {
                    // get statement
                    COMP__accountling_statement statement = *(COMP__accountling_statement*)current_statement.start;
                    
                    // determine statement type
                    // if abstraction call
                    if (statement.type == COMP__st__abstraction_call) {
                        // print statement call index
                        printf("\t\t\t\t%lu = ", statement.header.call_index);

                        // print io
                        COMP__print__accountling_arguments(statement.inputs);
                        COMP__print__accountling_arguments(statement.outputs);

                        // print new line
                        printf("\n");
                    // if offset declaration
                    } else if (statement.type == COMP__st__offset) {
                        // print index
                        printf("\t\t\t\t@%lu\n", statement.offset_ID);
                    }

                    // next statement
                    current_statement.start += sizeof(COMP__accountling_statement);
                }
            }
        }

        // next abstraction
        current_abstraction.start += sizeof(COMP__accountling_abstraction);
    }

    return;
}

/* Generation */
// add an offset to a list
void COMP__append__offset(ANVIL__list* list, ANVIL__offset data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(ANVIL__offset), &(*error).memory_error_occured);

    // append data
    (*(ANVIL__offset*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(ANVIL__offset);

    return;
}

// generation offsets
typedef struct COMP__generation_offsets {
    ANVIL__offset function_start; // the first instruction in the function
    ANVIL__offset function_return; // the first instruction passing output
    ANVIL__offset function_data; // after the code of the function, the embedded data's place
    ANVIL__list statement_offsets; // ANVIL__offset the function specific statement offsets
    ANVIL__list strings_offsets; // ANVIL__offset the function specific string offsets
} COMP__generation_offsets;

// generation cell range
typedef struct COMP__generation_cell_range {
    ANVIL__cell_ID start;
    ANVIL__cell_ID end;
} COMP__generation_cell_range;

// create a cell range
COMP__generation_cell_range COMP__create__generation_cell_range(ANVIL__cell_ID start, ANVIL__cell_ID end) {
    COMP__generation_cell_range output;

    output.start = start;
    output.end = end;

    return output;
}

// calculate cell range length
ANVIL__cell_count COMP__calculate__generation_cell_length(COMP__generation_cell_range range) {
    return range.end - range.start + 1;
}

// calculate cell range from start and length
COMP__generation_cell_range COMP__calculate__generation_cell_range(ANVIL__cell_ID start, ANVIL__cell_count length) {
    return COMP__create__generation_cell_range(start, start + length - 1);
}

// generation cells
typedef struct COMP__generation_cells {
    COMP__generation_cell_range workspace_total_range;
    COMP__generation_cell_range workspace_input_range;
    COMP__generation_cell_range workspace_output_range;
    COMP__generation_cell_range workspace_body_range;
    COMP__generation_cell_range function_input_range;
    COMP__generation_cell_range function_output_range;
    ANVIL__cell_count input_count;
    ANVIL__cell_count output_count;
    ANVIL__cell_count variable_count;
    ANVIL__cell_count workspace_total_count;
} COMP__generation_cells;

// setup generation cells from io & variable counts
COMP__generation_cells COMP__setup__generation_cells(ANVIL__cell_count input_count, ANVIL__cell_count output_count, ANVIL__cell_count variable_count) {
    COMP__generation_cells output;

    // setup counts
    output.input_count = input_count;
    output.output_count = output_count;
    output.variable_count = variable_count;
    output.workspace_total_count = input_count + output_count + variable_count;

    // calculate ranges
    output.function_input_range = COMP__calculate__generation_cell_range(ANVIL__srt__start__function_io, input_count);
    output.function_output_range = COMP__calculate__generation_cell_range(ANVIL__srt__start__function_io, output_count);
    output.workspace_input_range = COMP__calculate__generation_cell_range(ANVIL__srt__start__workspace, input_count);
    output.workspace_output_range = COMP__calculate__generation_cell_range(output.workspace_input_range.end + 1, output_count);
    output.workspace_body_range = COMP__calculate__generation_cell_range(output.workspace_output_range.end + 1, variable_count);
    output.workspace_total_range = COMP__create__generation_cell_range(output.workspace_input_range.start, output.workspace_body_range.end);

    return output;
}

// generation abstraction
typedef struct COMP__generation_abstraction {
    COMP__generation_cells cells;
    COMP__generation_offsets offsets;
    ANVIL__list converted_strings; // ANVIL__buffer (copied from accountlings! do not free!)
} COMP__generation_abstraction;

// translate accountling abstraction to generation abstraction
COMP__generation_abstraction COMP__open__generation_abstraction(COMP__accountling_abstraction accountlings, COMP__error* error) {
    COMP__generation_abstraction output;

    // translate registers
    output.cells = COMP__setup__generation_cells(COMP__calculate__list_content_count(accountlings.inputs, sizeof(COMP__parsling_argument)), COMP__calculate__list_content_count(accountlings.outputs, sizeof(COMP__parsling_argument)), COMP__calculate__list_content_count(accountlings.variables, sizeof(COMP__parsling_argument)));

    // open offsets
    output.offsets.function_start = COMP__define__null_offset_ID;
    output.offsets.function_return = COMP__define__null_offset_ID;
    output.offsets.function_data = COMP__define__null_offset_ID;
    output.offsets.statement_offsets = COMP__open__list(sizeof(ANVIL__offset) * 16, error);
    output.offsets.strings_offsets = COMP__open__list(sizeof(ANVIL__offset) * 16, error);

    // append statement offsets
    for (COMP__offset_index i = 0; i < COMP__calculate__list_content_count(accountlings.offsets, sizeof(COMP__parsling_argument)); i++) {
        // append blank offset
        COMP__append__offset(&output.offsets.statement_offsets, COMP__define__null_offset_ID, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }
    }

    // append string offsets
    for (COMP__offset_index i = 0; i < COMP__calculate__list_content_count(accountlings.strings, sizeof(COMP__parsling_argument)); i++) {
        // append blank offset
        COMP__append__offset(&output.offsets.strings_offsets, COMP__define__null_offset_ID, error);
        if (COMP__check__error_occured(error)) {
            return output;
        }
    }

    // open strings
    output.converted_strings = accountlings.converted_strings;

    return output;
}

// close generation abstraction
void COMP__close__generation_abstraction(COMP__generation_abstraction abstraction) {
    // close offsets
    ANVIL__close__list(abstraction.offsets.statement_offsets);
    ANVIL__close__list(abstraction.offsets.strings_offsets);

    return;
}

// append generation abstraction
void COMP__append__generation_abstraction(ANVIL__list* list, COMP__generation_abstraction data, COMP__error* error) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__generation_abstraction), &(*error).memory_error_occured);

    // append data
    (*(COMP__generation_abstraction*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__generation_abstraction);

    return;
}

// generation workspace
typedef struct COMP__generation_workspace {
    ANVIL__workspace anvil;
    ANVIL__workspace* workspace;
    ANVIL__list abstractions; // COMP__generation_abstraction
    COMP__abstraction_index entry_point;
    COMP__abstraction_index function_count;
    STD__offsets standard_offsets;
} COMP__generation_workspace;

// open workspace
void COMP__open__generation_workspace(ANVIL__buffer* program_buffer, COMP__accountling_program accountlings, COMP__generation_workspace* output, COMP__error* error) {
    // setup anvil workspace
    output->anvil = ANVIL__setup__workspace(program_buffer);
    output->workspace = &(output->anvil);

    // setup entry point
    output->entry_point = COMP__calculate__abstraction_index_from_call_index(accountlings.main_abstraction_ID);

    // setup generation abstractions from accountling abstractions
    // open generation abstractions
    output->abstractions = COMP__open__list(sizeof(COMP__generation_abstraction) * 16, error);
    output->function_count = 0;

    // setup current
    COMP__current current_accountling_abstraction = COMP__calculate__current_from_list_filled_index(&accountlings.abstractions);
    
    // for each accountling abstraction
    while (COMP__check__current_within_range(current_accountling_abstraction)) {
        // get accountling abstraction
        COMP__accountling_abstraction accountling_abstraction = *(COMP__accountling_abstraction*)current_accountling_abstraction.start;

        // setup generation abstraction
        COMP__generation_abstraction generation_abstraction = COMP__open__generation_abstraction(accountling_abstraction, error);
        if (COMP__check__error_occured(error)) {
            return;
        }

        // append abstraction
        COMP__append__generation_abstraction(&output->abstractions, generation_abstraction, error);
        if (COMP__check__error_occured(error)) {
            return;
        }

        // next abstraction
        output->function_count++;
        current_accountling_abstraction.start += sizeof(COMP__accountling_abstraction);
    }

    return;
}

// close workspace
void COMP__close__generation_workspace(COMP__generation_workspace workspace) {
    COMP__current current = COMP__calculate__current_from_list_filled_index(&workspace.abstractions);

    // close all abstractions
    while (COMP__check__current_within_range(current)) {
        // close abstraction
        COMP__close__generation_abstraction(*(COMP__generation_abstraction*)current.start);

        // next abstraction
        current.start += sizeof(COMP__generation_abstraction);
    }

    // close abstractions list
    ANVIL__close__list(workspace.abstractions);

    return;
}

// get abstraction from workspace
COMP__generation_abstraction COMP__find__generation_abstraction_by_index(COMP__generation_workspace* workspace, COMP__abstraction_index index) {
    return ((COMP__generation_abstraction*)(*workspace).abstractions.buffer.start)[index];
}

// get abstraction from abstraction list
COMP__accountling_abstraction COMP__find__accountling_abstraction_by_index(COMP__accountling_program program, COMP__abstraction_index index) {
    return ((COMP__accountling_abstraction*)program.abstractions.buffer.start)[index];
}

// convert variable index to cell ID
ANVIL__cell_ID COMP__translate__accountling_variable_index_to_cell_ID(COMP__generation_abstraction* generation_abstraction, COMP__accountling_argument argument, COMP__error* error) {
    // convert based on type
    if (argument.type == COMP__pat__variable || argument.type == COMP__pat__variable__body) {
        // return cell ID
        return generation_abstraction->cells.workspace_body_range.start + argument.index;
    } else if (argument.type == COMP__pat__variable__input) {
        // return cell ID
        return generation_abstraction->cells.workspace_input_range.start + argument.index;
    } else if (argument.type == COMP__pat__variable__output) {
        // return cell ID
        return generation_abstraction->cells.workspace_output_range.start + argument.index;
    } else if (argument.type == COMP__pat__variable__predefined) {
        // determine variable type
        if (argument.index == COMP__pvt__error_code) {
            return ANVIL__rt__error_code;
        }
    }
    
    // error
    *error = COMP__open__error("Internal Error: Unsupported variable index type.", COMP__create_null__character_location());

    return COMP__define__null_offset_ID;
}

// convert flag index to flag ID
ANVIL__flag_ID COMP__translate__accountling_flag_index_to_flag_ID(COMP__accountling_argument argument, COMP__error* error) {
    if (argument.type == COMP__pat__flag__predefined) {
        return argument.index;
    } else if (argument.type == COMP__pat__flag__user_defined) {
        return COMP__pft__COUNT + argument.index;
    }

    // error
    *error = COMP__open__error("Internal Error: Flag is not a usable type (aka, not a predefined flag or user defined flag), oops.", COMP__create_null__character_location());

    return ANVIL__define__null_flag;
}

// generate function
void COMP__forge__anvil_abstraction(COMP__generation_workspace* workspace, COMP__generation_abstraction* generation_abstraction, COMP__accountling_abstraction accountling_abstraction, COMP__error* error) {
    COMP__string_index current_string_ID = 0;

    // setup offset
    (*generation_abstraction).offsets.function_start = ANVIL__get__offset((*workspace).workspace);

    // setup function prologue
    ANVIL__code__preserve_workspace((*workspace).workspace, ANVIL__sft__always_run, generation_abstraction->cells.workspace_total_range.start, generation_abstraction->cells.workspace_total_range.end);

    // get inputs
    for (COMP__input_count i = 0; i < generation_abstraction->cells.input_count; i++) {
        // pass input
        ANVIL__code__cell_to_cell(workspace->workspace, ANVIL__sft__always_run, generation_abstraction->cells.function_input_range.start + i, generation_abstraction->cells.workspace_input_range.start + i);
    }

    // translate statements
    // setup current
    COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&accountling_abstraction.statements);

    // for each statement
    while (COMP__check__current_within_range(current_statement)) {
        // get statement
        COMP__accountling_statement statement = *(COMP__accountling_statement*)current_statement.start;

        // build statement
        // if statement is call
        if (statement.type == COMP__st__abstraction_call) {
            // if statement is instruction
            if (statement.header.call_index < COMP__act__user_defined) {
                // determine instruction type & write instruction
                switch ((COMP__act)statement.header.call_index) {
                case COMP__act__set__boolean:
                case COMP__act__set__binary:
                case COMP__act__set__integer:
                case COMP__act__set__hexadecimal:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0).index, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__set__offset:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(((ANVIL__offset*)generation_abstraction->offsets.statement_offsets.buffer.start)[COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0).index]), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__set__flag_ID:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__set__string:
                    // write string data
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)((ANVIL__offset*)generation_abstraction->offsets.strings_offsets.buffer.start)[current_string_ID], ANVIL__srt__temp__offset);
                    ANVIL__code__retrieve_embedded_buffer(workspace->workspace, ANVIL__sft__always_run, ANVIL__srt__temp__offset, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 1), error));

                    // next string ID
                    current_string_ID++;

                    break;
                case COMP__act__io__cell_to_address:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__cell_to_address(workspace->workspace, ANVIL__srt__temp__flag_ID_0, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 2), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__io__address_to_cell:
                    ANVIL__code__address_to_cell(workspace->workspace, COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 2), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__io__file_to_buffer:
                    ANVIL__code__file_to_buffer(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 1), error));

                    break;
                case COMP__act__io__buffer_to_file:
                    ANVIL__code__buffer_to_file(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 1), error));

                    break;
                case COMP__act__copy__cell:
                    ANVIL__code__cell_to_cell(workspace->workspace, ANVIL__sft__always_run, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__copy__buffer:
                    ANVIL__code__buffer_to_buffer(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 1), error));

                    break;
                case COMP__act__memory__request_memory:
                    ANVIL__code__request_memory(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 1), error));

                    break;
                case COMP__act__memory__return_memory:
                    ANVIL__code__return_memory(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error));

                    break;
                case COMP__act__buffer__calculate_length:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_subtract, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__constant__1, ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__cast__cell_to_unsigned_integer_string:
                    STD__code__call__cell_to_unsigned_integer_string(workspace->workspace, &workspace->standard_offsets, ANVIL__sft__always_run, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 1), error));

                    break;
                case COMP__act__print__unsigned_integer:
                    ANVIL__code__debug__print_cell_as_decimal(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error));

                    break;
                case COMP__act__print__character:
                    ANVIL__code__debug__putchar(workspace->workspace, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error));

                    break;
                case COMP__act__print__buffer_as_string:
                    STD__code__call__print_buffer_as_string(workspace->workspace, &workspace->standard_offsets, ANVIL__sft__always_run, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error));

                    break;
                case COMP__act__print__binary:
                    STD__code__call__print_binary(workspace->workspace, &workspace->standard_offsets, ANVIL__sft__always_run, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error));

                    break;
                case COMP__act__integer__add:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__integer__subtract:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_subtract, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__integer__multiply:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_multiply, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__integer__divide:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_division, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__integer__modulous:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__integer_modulous, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__integer__within_range:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__operate__flag(workspace->workspace, ANVIL__sft__always_run, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 2), error), COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 3), error), ANVIL__srt__temp__flag_ID_0);

                    break;
                case COMP__act__binary__or:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_or, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__binary__invert:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_invert, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__binary__and:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_and, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__binary__xor:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_xor, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__binary__shift_higher:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_shift_higher, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__binary__shift_lower:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_shift_lower, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__binary__overwrite:
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__bits_overwrite, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 2), error), COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__flag__get:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__flag_get, ANVIL__srt__temp__flag_ID_0, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error));

                    break;
                case COMP__act__flag__set:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__flag_set, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__temp__flag_ID_0);

                    break;
                case COMP__act__flag__invert:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__temp__flag_ID_1);
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__flag_invert, ANVIL__srt__temp__flag_ID_0, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__temp__flag_ID_1);

                    break;
                case COMP__act__flag__or:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__srt__temp__flag_ID_1);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__temp__flag_ID_2);
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__flag_or, ANVIL__srt__temp__flag_ID_0, ANVIL__srt__temp__flag_ID_1, ANVIL__unused_cell_ID, ANVIL__srt__temp__flag_ID_2);

                    break;
                case COMP__act__flag__and:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__srt__temp__flag_ID_1);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__temp__flag_ID_2);
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__flag_and, ANVIL__srt__temp__flag_ID_0, ANVIL__srt__temp__flag_ID_1, ANVIL__unused_cell_ID, ANVIL__srt__temp__flag_ID_2);

                    break;
                case COMP__act__flag__xor:
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), ANVIL__srt__temp__flag_ID_0);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1), error), ANVIL__srt__temp__flag_ID_1);
                    ANVIL__code__write_cell(workspace->workspace, (ANVIL__cell)(ANVIL__cell_integer_value)COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.outputs, 0), error), ANVIL__srt__temp__flag_ID_2);
                    ANVIL__code__operate(workspace->workspace, ANVIL__sft__always_run, ANVIL__ot__flag_xor, ANVIL__srt__temp__flag_ID_0, ANVIL__srt__temp__flag_ID_1, ANVIL__unused_cell_ID, ANVIL__srt__temp__flag_ID_2);

                    break;
                case COMP__act__jump:
                    ANVIL__code__jump__static(workspace->workspace, COMP__translate__accountling_flag_index_to_flag_ID(COMP__get__abstractling_statement_argument_by_index(statement.inputs, 0), error), (((ANVIL__offset*)generation_abstraction->offsets.statement_offsets.buffer.start)[COMP__get__abstractling_statement_argument_by_index(statement.inputs, 1).index]));

                    break;
                case COMP__act__reset_error_code_cell:
                    ANVIL__code__write_cell(workspace->workspace, ANVIL__et__no_error, ANVIL__rt__error_code);

                    break;
                default:
                    // should not be reachable
                    break;
                }
            // if statement is abstraction call
            } else {
                // pass inputs
                for (COMP__input_count i = 0; i < COMP__calculate__list_content_count(statement.inputs, sizeof(COMP__accountling_argument)); i++) {
                    // pass input
                    ANVIL__code__cell_to_cell(workspace->workspace, ANVIL__sft__always_run, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.inputs, i), error), generation_abstraction->cells.function_input_range.start + i);
                }

                // call function
                ANVIL__code__call__static(workspace->workspace, ANVIL__sft__always_run, ((COMP__generation_abstraction*)workspace->abstractions.buffer.start)[COMP__calculate__abstraction_index_from_call_index(statement.header.call_index)].offsets.function_start);

                // pass outputs
                for (COMP__output_count i = 0; i < COMP__calculate__list_content_count(statement.outputs, sizeof(COMP__accountling_argument)); i++) {
                    // pass output
                    ANVIL__code__cell_to_cell(workspace->workspace, ANVIL__sft__always_run, generation_abstraction->cells.function_output_range.start + i, COMP__translate__accountling_variable_index_to_cell_ID(generation_abstraction, COMP__get__abstractling_statement_argument_by_index(statement.outputs, i), error));
                }
            }
        // statement is offset
        } else if (statement.type == COMP__st__offset) {
            // declare offset
            ((ANVIL__offset*)generation_abstraction->offsets.statement_offsets.buffer.start)[statement.offset_ID] = ANVIL__get__offset(workspace->workspace);
        // unrecognized statement type, error
        } else {
            *error = COMP__open__error("Internal Error: In code generator, invalid statement type, oops.", statement.header.header.name.text.lexling.location);

            return;
        }

        // check for error
        if (COMP__check__error_occured(error)) {
            return;
        }

        // next statement
        current_statement.start += sizeof(COMP__accountling_statement);
    }

    // setup function return offset
    generation_abstraction->offsets.function_return = ANVIL__get__offset(workspace->workspace);

    // pass outputs
    for (COMP__output_count i = 0; i < generation_abstraction->cells.output_count; i++) {
        // pass input
        ANVIL__code__cell_to_cell(workspace->workspace, ANVIL__sft__always_run, generation_abstraction->cells.workspace_output_range.start + i, generation_abstraction->cells.function_output_range.start + i);
    }

    // setup function epilogue
    ANVIL__code__restore_workspace(workspace->workspace, ANVIL__sft__always_run, generation_abstraction->cells.workspace_total_range.start, generation_abstraction->cells.workspace_total_range.end);

    // return to caller
    ANVIL__code__jump__explicit(workspace->workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

    // setup data offset
    generation_abstraction->offsets.function_data = ANVIL__get__offset(workspace->workspace);

    // setup strings
    for (COMP__string_index i = 0; i < COMP__calculate__list_content_count(generation_abstraction->converted_strings, sizeof(ANVIL__buffer)); i++) {
        // setup offset
        ((ANVIL__offset*)generation_abstraction->offsets.strings_offsets.buffer.start)[i] = ANVIL__get__offset(workspace->workspace);

        // embed string
        ANVIL__code__buffer(workspace->workspace, ((ANVIL__buffer*)generation_abstraction->converted_strings.buffer.start)[i]);
    }

    return;
}

// generate program
void COMP__forge__anvil_program(ANVIL__buffer* final_program, COMP__accountling_program accountlings, ANVIL__stack_size stack_size, COMP__error* error) {
    COMP__generation_workspace workspace;
    
    // setup generation workspace
    COMP__open__generation_workspace(final_program, accountlings, &workspace, error);
    if (COMP__check__error_occured(error)) {
        return;
    }

    // forge program
    for (ANVIL__pt pass = ANVIL__pt__get_offsets; pass < ANVIL__pt__COUNT; pass++) {
        // setup pass
        ANVIL__setup__pass(workspace.workspace, pass);

        // build program
        // setup kickstarter
        ANVIL__code__start(&workspace.anvil, stack_size, COMP__find__generation_abstraction_by_index(&workspace, workspace.entry_point).offsets.function_start);

        // weave user defined abstractions
        // setup current
        COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&workspace.abstractions);
        COMP__abstraction_index index = 0;

        // for each abstraction
        while (COMP__check__current_within_range(current_abstraction)) {
            // weave abstraction
            COMP__forge__anvil_abstraction(&workspace, (COMP__generation_abstraction*)current_abstraction.start, COMP__find__accountling_abstraction_by_index(accountlings, index), error);

            // next abstraction
            current_abstraction.start += sizeof(COMP__generation_abstraction);
            index++;
        }

        // forge built in functions
        STD__code__package(workspace.workspace, &workspace.standard_offsets);
    }

    // close workspace
    COMP__close__generation_workspace(workspace);

    return;
}

/* Compile */
// one compiled object across multiple stages
typedef struct COMP__compilation_unit {
    ANVIL__buffer user_codes;
    ANVIL__list lexling_buffers;
    ANVIL__list parsling_buffers;
    COMP__accountling_program accountlings;
    COMP__pst stages_completed;
} COMP__compilation_unit;

// close a compilation unit
void COMP__close__compilation_unit(COMP__compilation_unit unit) {
    // close lexling buffers
    if (unit.stages_completed > COMP__pst__invalid) {
        // setup current
        COMP__current current = COMP__calculate__current_from_list_filled_index(&unit.lexling_buffers);

        // close each buffer
        while (COMP__check__current_within_range(current)) {
            // close parsling buffer
            COMP__close__lexlings(*(COMP__lexlings*)current.start);

            // next program
            current.start += sizeof(COMP__lexlings);
        }

        // close parslings buffer
        ANVIL__close__list(unit.lexling_buffers);
    }

    // close parsling buffers
    if (unit.stages_completed > COMP__pst__lexing) {
        // setup current
        COMP__current current = COMP__calculate__current_from_list_filled_index(&unit.parsling_buffers);

        // close each buffer
        while (COMP__check__current_within_range(current)) {
            // close parsling buffer
            COMP__close__parsling_program(*(COMP__parsling_program*)current.start);

            // next program
            current.start += sizeof(COMP__parsling_program);
        }

        // close parslings buffer
        ANVIL__close__list(unit.parsling_buffers);
    }

    // close accountling data
    if (unit.stages_completed > COMP__pst__parsing) {
        COMP__close__accountling_program(unit.accountlings);
    }

    return;
}

// compile a program
void COMP__compile__files(ANVIL__buffer user_codes, ANVIL__bt print_debug, ANVIL__buffer* final_program, COMP__error* error) {
    COMP__compilation_unit compilation_unit;
    COMP__current current;
    COMP__file_index current_file_index = 0;

    // setup blank error
    *error = COMP__create_null__error();

    // setup compilation unit user code list and lexling list
    compilation_unit.user_codes = user_codes;
    compilation_unit.lexling_buffers = COMP__open__list(sizeof(COMP__lexlings) * 5, error);
    compilation_unit.parsling_buffers = COMP__open__list(sizeof(COMP__parsling_program) * 5, error);
    compilation_unit.stages_completed = COMP__pst__invalid;

    // check for error
    if (COMP__check__error_occured(error)) {
        goto quit;
    }

    // setup current
    current = compilation_unit.user_codes;

    // print compilation message
    if (print_debug) {
        printf("Compiling Files.\n");
    }

    // lex and parse each file
    while (COMP__check__current_within_range(current)) {
        // get file
        ANVIL__buffer user_code = *(ANVIL__buffer*)current.start;

        // inform user of compilation start
        if (print_debug) {
            printf("Compiling User Code:\n");
            ANVIL__print__buffer(user_code);
            printf("\n");
        }

        // lex file
        COMP__lexlings lexlings = COMP__compile__lex(user_code, current_file_index, error);

        // append lexlings
        COMP__append__lexlings(&compilation_unit.lexling_buffers, lexlings, error);

        // print lexlings
        if (print_debug) {
            COMP__debug__print_lexlings(lexlings);
        }

        // mark as done (unfinished input acceptable)
        if (compilation_unit.stages_completed < COMP__pst__lexing) {
            compilation_unit.stages_completed = COMP__pst__lexing;
        }

        // check for error
        if (COMP__check__error_occured(error)) {
            goto quit;
        }

        // parse file
        COMP__parsling_program parslings = COMP__parse__program(lexlings, error);

        // append program
        COMP__append__parsling_program(&compilation_unit.parsling_buffers, parslings, error);

        // print parslings
        if (print_debug) {
            COMP__print__parsed_program(parslings);
        }

        // mark as done (unfinished input acceptable)
        if (compilation_unit.stages_completed < COMP__pst__parsing) {
            compilation_unit.stages_completed = COMP__pst__parsing;
        }

        // check for error
        if (COMP__check__error_occured(error)) {
            goto quit;
        }

        // advance current
        current.start += sizeof(ANVIL__buffer);
        current_file_index++;
    }

    // account
    compilation_unit.accountlings = COMP__account__program(compilation_unit.parsling_buffers, error);

    // mark as done
    compilation_unit.stages_completed = COMP__pst__accounting;

    // check for errors
    if (COMP__check__error_occured(error)) {
        goto quit;
    }

    // print accountlings
    if (print_debug) {
        COMP__print__accountling_program(compilation_unit.accountlings);
    }

    // generate program
    COMP__forge__anvil_program(final_program, compilation_unit.accountlings, 65536, error);

    // quit label
    quit:

    // clean up
    COMP__close__compilation_unit(compilation_unit);

    return;
}

#endif
