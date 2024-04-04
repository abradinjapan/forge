#ifndef FROST__compiler
#define FROST__compiler

/* Include */
// anvil / basic
#include "../anvil/anvil.h"

/* Define */
// aliases
typedef ANVIL__buffer COMP__current;

// error types
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

// accounting types
typedef ANVIL__u64 COMP__io_count;
typedef COMP__io_count COMP__input_count;
typedef COMP__io_count COMP__output_count;
typedef ANVIL__u64 COMP__variable_index;
typedef ANVIL__u64 COMP__call_index;
typedef ANVIL__u64 COMP__offset_index;
typedef ANVIL__u64 COMP__instruction_index;
typedef ANVIL__u64 COMP__statement_index;

// generation types
typedef ANVIL__u64 COMP__abstraction_index;

// defines
#define COMP__define__null_offset_ID -1
#define COMP__define__null_user_defined_call_ID -1
#define COMP__define__u64_max_base_10_conversion_character_limit 20

// program stage type
typedef enum COMP__pst {
    // stages
    COMP__pst__loading, // getting all files into memory
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
    COMP__file_index file_index;
    COMP__line_number line_number;
    COMP__character_index character_index;
} COMP__error;

// create custom error
COMP__error COMP__create__error(ANVIL__bt occured, ANVIL__buffer message, COMP__file_index file_index, COMP__line_number line_number, COMP__character_index character_index) {
    COMP__error output;

    // setup output
    output.occured = occured;
    output.message = message;
    output.file_index = file_index;
    output.line_number = line_number;
    output.character_index = character_index;

    return output;
}

// create null error
COMP__error COMP__create_null__error() {
    // return empty
    return COMP__create__error(ANVIL__bt__false, ANVIL__create_null__buffer(), 0, 0, 0);
}

// create a generic memory allocation error
COMP__error COMP__create__internal_memory_error() {
    return COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Internal memory error.", ANVIL__bt__true, ANVIL__bt__false), 0, 0, 0);
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
    temp_buffer = COMP__translate__integer_value_to_string(error.file_index);
    ANVIL__list__append__buffer_data(&json, temp_buffer, error_occured);
    ANVIL__close__buffer(temp_buffer);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\",\n\t\"line_number\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    temp_buffer = COMP__translate__integer_value_to_string(error.line_number);
    ANVIL__list__append__buffer_data(&json, temp_buffer, error_occured);
    ANVIL__close__buffer(temp_buffer);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\",\n\t\"character_index\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    temp_buffer = COMP__translate__integer_value_to_string(error.character_index);
    ANVIL__list__append__buffer_data(&json, temp_buffer, error_occured);
    ANVIL__close__buffer(temp_buffer);
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\"\n}\n", ANVIL__bt__false, ANVIL__bt__false), error_occured);

    // create buffer from list
    output = ANVIL__list__create_buffer_from_list(&json, error_occured);

    // clean up list
    ANVIL__close__list(json);

    return output;
}

// check to see if an error occured
ANVIL__bt COMP__check__error_occured(COMP__error* error) {
    return (*error).occured;
}

// close an error
void COMP__close__error(COMP__error error) {
    // clean up buffers
    ANVIL__close__buffer(error.message);
    
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
    COMP__file_index file_index;
    COMP__line_number line_number;
    COMP__character_index character_index;
} COMP__lexling;

// create custom lexling
COMP__lexling COMP__create__lexling(COMP__lexling_type type, ANVIL__buffer value, COMP__file_index file_index, COMP__line_number line_number, COMP__character_index character_index) {
    COMP__lexling output;

    // setup output
    output.type = type;
    output.value = value;
    output.file_index = file_index;
    output.line_number = line_number;
    output.character_index = character_index;

    return output;
}

// create null lexling
COMP__lexling COMP__create_null__lexling() {
    // return empty
    return COMP__create__lexling(COMP__lt__invalid, ANVIL__create_null__buffer(), 0, 0, 0);
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
void COMP__append__lexling(COMP__lexlings* lexlings, COMP__lexling lexling, ANVIL__bt* error_occured) {
    // request space
    ANVIL__list__request__space(&((*lexlings).data), sizeof(COMP__lexling), error_occured);

    // append data
    (*(COMP__lexling*)ANVIL__calculate__list_current_address(&((*lexlings).data))) = lexling;

    // increase fill
    (*lexlings).data.filled_index += sizeof(COMP__lexling);

    return;
}

// append a lexlings to the list
void COMP__append__lexlings(ANVIL__list* list, COMP__lexlings lexlings, ANVIL__bt* error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__lexlings), error_occured);

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
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // setup output
    output.data = ANVIL__open__list(sizeof(COMP__lexling) * 64, &memory_error_occured);

    // check for error
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();

        // return empty
        return COMP__create_null__lexlings();
    }

    // setup current & trackers
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
                    *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Lexing Error: Comment ended with end of file instead of proper closing.", ANVIL__bt__true, ANVIL__bt__false), file_index, current_line_number, COMP__calculate__character_index(user_code, current));

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
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_parenthesis, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, ')', ')')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_parenthesis, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '{', '{')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_curly_bracket, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '}', '}')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_curly_bracket, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

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
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__name, ANVIL__create__buffer(temp_start, temp_end), file_index, current_line_number, COMP__calculate__character_index(user_code, ANVIL__create__buffer(temp_start, current.end))), &memory_error_occured);
        } else if (COMP__calculate__valid_character_range(current, '@', '@')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__at, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '#', '#')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__hashtag, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '=', '=')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__equals, ANVIL__create__buffer(current.start, current.start), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

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
                *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Lexical Error: String ended at the end of a file and not with a (\").", ANVIL__bt__true, ANVIL__bt__false), file_index, current_line_number, COMP__calculate__character_index(user_code, current));

                goto quit;
            }

            // finish string data
            data.end = current.start;

            // append lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__string_literal, data, file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        // no lexling found
        } else {
            // open error
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Lexical Error: Invalid character.", ANVIL__bt__true, ANVIL__bt__false), file_index, current_line_number, COMP__calculate__character_index(user_code, current));

            // quit
            goto quit;
        }

        // check for error
        if (memory_error_occured == ANVIL__bt__true) {
            // set error
            *error = COMP__create__internal_memory_error();

            // return lexlings as they are
            goto quit;
        }
    }

    // quit
    quit:

    // append eof lexling
    COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__end_of_file, ANVIL__open__buffer_from_string((u8*)"[EOF]", ANVIL__bt__false, ANVIL__bt__false), file_index, current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();
    }

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
        printf("\t%lu [ %lu, %lu ] [ file_index: %lu, line_number: %lu, character_index: %lu ] : ", (ANVIL__u64)temp.type, (ANVIL__u64)temp.value.start, (ANVIL__u64)temp.value.end, temp.file_index, (ANVIL__u64)temp.line_number, (ANVIL__u64)temp.character_index);
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

// check if two names are exactly the same (not including line information)
ANVIL__bt COMP__check__name_data_is_identical(COMP__name a, COMP__name b) {
    return ANVIL__calculate__buffer_contents_equal(a.lexling.value, b.lexling.value);
}

// append name
void COMP__append__name(ANVIL__list* list, COMP__name data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__name), memory_error_occured);

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
    COMP__pat__variable__body,
    COMP__pat__offset,
    COMP__pat__flag,
    COMP__pat__literal__boolean,
    COMP__pat__literal__binary,
    COMP__pat__literal__integer,
    COMP__pat__literal__hexadecimal,
    COMP__pat__literal__string,
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

    // reset current
    current = ANVIL__create__buffer(string.start + ANVIL__calculate__buffer_length(prefix), string.end);

    // convert binary string to binary number
    while (COMP__check__current_within_range(current)) {
        if (COMP__check__character_range_at_current(current, '_', '_') == ANVIL__bt__false) {
            // next value
            *value <<= 1;

            // append value
            *value += ((*(ANVIL__character*)current.start) - '0');
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
    COMP__name name;

    // abstraction call data
    ANVIL__list inputs; // COMP__parsling_argument
    ANVIL__list outputs; // COMP__parsling_argument

    // metadata
    COMP__input_count input_count;
    COMP__output_count output_count;
} COMP__parsling_statement;

// create a custom statement
COMP__parsling_statement COMP__create__parsling_statement(COMP__st type, COMP__name name, ANVIL__list inputs, ANVIL__list outputs, COMP__input_count input_count, COMP__output_count output_count) {
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
    return COMP__create__parsling_statement(COMP__st__COUNT, COMP__create_null__name(), ANVIL__create_null__list(), ANVIL__create_null__list(), 0, 0);
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
void COMP__append__parsling_argument(ANVIL__list* list, COMP__parsling_argument data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_argument), memory_error_occured);

    // append data
    (*(COMP__parsling_argument*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_argument);

    return;
}

// append parsling statement
void COMP__append__parsling_statement(ANVIL__list* list, COMP__parsling_statement data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_statement), memory_error_occured);

    // append data
    (*(COMP__parsling_statement*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_statement);

    return;
}

// append parsling abstraction
void COMP__append__parsling_abstraction(ANVIL__list* list, COMP__parsling_abstraction data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_abstraction), memory_error_occured);

    // append data
    (*(COMP__parsling_abstraction*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_abstraction);

    return;
}

// append parsling program
void COMP__append__parsling_program(ANVIL__list* list, COMP__parsling_program data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_program), memory_error_occured);

    // append data
    (*(COMP__parsling_program*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_program);

    return;
}

// close statement
void COMP__close__parsling_statement(COMP__parsling_statement statement) {
    // close io
    ANVIL__close__list(statement.inputs);
    ANVIL__close__list(statement.outputs);

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
ANVIL__list COMP__parse__arguments(COMP__current* current, COMP__io_count* count, COMP__error* error) {
    ANVIL__list output;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;
    ANVIL__cell_integer_value value = 0;

    // init count
    *count = 0;

    // open names list
    output = ANVIL__open__list(sizeof(COMP__parsling_argument) * 8, &memory_error_occured);

    // check for error
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();

        return output;
    }
    
    // check for opening parenthesis
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__left_parenthesis) {
        // next lexling
        COMP__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Arguments is missing opening parenthesis.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

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
                *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Offset is missing name.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

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
                *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Flag is missing name.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

                return output;
            }
        // string literal
        } else if (COMP__read__lexling_from_current(*current).type == COMP__lt__string_literal) {
            // get argument
            argument = COMP__create__parsling_argument(COMP__pat__literal__string, COMP__create__name_from_lexling_current(*current), 0);
        // error
        } else {
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Unrecognized argument type.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

            return output;
        }

        // append argument
        COMP__append__parsling_argument(&output, argument, &memory_error_occured);
        
        // increment count
        *count = *count + 1;

        // check for error
        if (memory_error_occured == ANVIL__bt__true) {
            // set error
            *error = COMP__create__internal_memory_error();

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
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Arguments is missing closing parenthesis.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

        return output;
    }

    return output;
}

// parse one statement
COMP__parsling_statement COMP__parse__statement(COMP__current* current, COMP__error* error) {
    COMP__parsling_statement output = COMP__create_null__parsling_statement();
    COMP__input_count input_count;
    COMP__output_count output_count;

    // check for offset
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__at) {
        // setup temp
        output.type = COMP__st__offset;

        // advance current
        COMP__advance__lexling_current(current, 1);

        // check for offset name
        if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
            // set name
            output.name = COMP__create__name_from_lexling_current(*current);

            // advance current
            COMP__advance__lexling_current(current, 1);
        // invalid syntax
        } else {
            // set error
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Offset statement name is an invalid lexling.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

            return output;
        }

        // null initialize unused data
        output.inputs = ANVIL__create_null__list();
        output.outputs = ANVIL__create_null__list();
    // is an abstraction call
    } else if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
        // set type
        output.type = COMP__st__abstraction_call;

        // get name
        output.name = COMP__create__name_from_lexling_current(*current);

        // advance current
        COMP__advance__lexling_current(current, 1);

        // get inputs
        output.inputs = COMP__parse__arguments(current, &input_count, error);
        output.input_count = input_count;

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }

        // get outputs
        output.outputs = COMP__parse__arguments(current, &output_count, error);
        output.output_count = output_count;

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }
    // error
    } else {
        // set error
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Unrecognized statement type.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

        return output;
    }

    return output;
}

// parse an abstraction
COMP__parsling_abstraction COMP__parse__abstraction(COMP__current* current, COMP__error* error) {
    COMP__parsling_abstraction output = COMP__create_null__parsling_abstraction();
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // check for eof
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__end_of_file) {
        return output;
    }

    // parse header
    output.header = COMP__parse__statement(current, error);

    // check for equals sign
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__equals) {
        // no saving data necessary, next lexling
        COMP__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: An abstraction definition has an equals sign missing.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

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
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Scope is missing left curly bracket.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);
        
        return output;
    }

    // parse statements
    // open statements list
    output.statements = ANVIL__open__list(sizeof(COMP__parsling_statement) * 16, &memory_error_occured);
    
    // get statements
    while (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type != COMP__lt__right_curly_bracket) {
        // parse statement
        COMP__parsling_statement statement = COMP__parse__statement(current, error);

        // add statement
        COMP__append__parsling_statement(&output.statements, statement, &memory_error_occured);
        if (memory_error_occured == ANVIL__bt__true) {
            *error = COMP__create__internal_memory_error();

            return output;
        }

        // check for error
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
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Scope is missing right curly bracket.", ANVIL__bt__true, ANVIL__bt__false), COMP__read__lexling_from_current(*current).file_index, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);
        
        return output;
    }

    return output;
}

// parse a file (program)
COMP__parsling_program COMP__parse__program(COMP__lexlings lexlings, COMP__error* error) {
    COMP__parsling_program output;
    COMP__parsling_abstraction temp;
    COMP__current current;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // setup error
    *error = COMP__create_null__error();

    // setup current
    current = COMP__calculate__current_from_list_filled_index(&lexlings.data);

    // open the abstraction list
    output.abstractions = ANVIL__open__list(sizeof(COMP__parsling_abstraction) * 64, &memory_error_occured);

    // check for memory error
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();

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
        COMP__append__parsling_abstraction(&(output.abstractions), temp, &memory_error_occured);

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            goto quit;
        }
    }

    // quit
    quit:

    return output;
}

// print argument
void COMP__print__argument(COMP__parsling_argument argument) {
    if (argument.type == COMP__pat__variable) {
        printf("[variable]");
        ANVIL__print__buffer(argument.text.lexling.value);
    } else if (argument.type == COMP__pat__offset) {
        printf("@");
        ANVIL__print__buffer(argument.text.lexling.value);
    } else if (argument.type == COMP__pat__flag) {
        printf("#");
        ANVIL__print__buffer(argument.text.lexling.value);
    } else if (argument.type == COMP__pat__literal__string) {
        ANVIL__print__buffer(argument.text.lexling.value);
    } else if (argument.type == COMP__pat__literal__boolean) {
        printf("[boolean]");
        ANVIL__print__buffer(argument.text.lexling.value);
        printf("[%lu]", argument.value);
    } else if (argument.type == COMP__pat__literal__binary) {
        printf("[binary]");
        ANVIL__print__buffer(argument.text.lexling.value);
        printf("[%lu]", argument.value);
    } else if (argument.type == COMP__pat__literal__integer) {
        printf("[integer]");
        ANVIL__print__buffer(argument.text.lexling.value);
        printf("[%lu, %li]", argument.value, argument.value);
    } else if (argument.type == COMP__pat__literal__hexadecimal) {
        printf("[hexadecimal]");
        ANVIL__print__buffer(argument.text.lexling.value);
        printf("[%lu]", argument.value);
    } else {
        printf("[invalid]");
    }

    return;
}

// print arguments
void COMP__print__arguments(ANVIL__list* arguments) {
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
        COMP__print__argument(*(COMP__parsling_argument*)current.start);

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
        ANVIL__print__buffer(statement.name.lexling.value);
    } else if (statement.type == COMP__st__abstraction_call) {
        // print statement
        ANVIL__print__buffer(statement.name.lexling.value);

        // print inputs
        COMP__print__arguments(&statement.inputs);

        // print outputs
        COMP__print__arguments(&statement.outputs);
    }

    return;
}

// print an abstraction
void COMP__print__parsed_abstraction(COMP__parsling_abstraction abstraction) {
    COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&abstraction.statements);

    // print header
    printf("Abstraction: ");
    COMP__print__parsed_statement(abstraction.header);

    // new line for statements
    printf("\n");

    // print statements
    if (ANVIL__check__empty_list(abstraction.statements) == ANVIL__bt__false) {
        // print each statement
        while (COMP__check__current_within_range(current_statement)) {
            // print formatting
            printf("\t");

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
/*
    First, get all possible calls and get all data from each function that cannot be verified in statement order.
    Second, Go though each abstraction and verify / translate each statement in top down order one by one.
*/
/*// accountling variable declaration index
typedef ANVIL__u64 COMP__accountling_variable_declaration_index;

// blueprintling
typedef ANVIL__u64 COMP__blueprintling;

// accountling call type
typedef enum COMP__act {
    // start
    COMP__act__START = 0,

    // acts
    COMP__act__set = COMP__act__START,
    COMP__act__debug_print_integer,

    // end
    COMP__act__END,

    // user defined
    COMP__act__user_defined = COMP__act__END,

    // count
    COMP__act__COUNT = COMP__act__END - COMP__act__START,
} COMP__act;

// accountling header
typedef struct COMP__accountling_abstraction_header {
    COMP__name name;
    COMP__act call;
    COMP__input_count input_count;
    COMP__output_count output_count;
    ANVIL__list inputs; // COMP__parsling_argument
    ANVIL__list outputs; // COMP__parsling_argument
} COMP__accountling_abstraction_header;

// close accountling abstraction header
void COMP__close__accountling_abstraction_header(COMP__accountling_abstraction_header header) {
    // close io
    ANVIL__close__list(header.inputs);
    ANVIL__close__list(header.outputs);

    return;
}

// accountling abstraction
typedef struct COMP__accountling_abstraction {
    ANVIL__list variables;
} COMP__accountling_abstraction;

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
    COMP__bnit__debug_print_integer,

    // stats
    COMP__bnit__COUNT,
} COMP__bnit;

// append accountling abstraction header
void COMP__append__accountling_abstraction_header(ANVIL__list* list, COMP__accountling_abstraction_header data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_abstraction_header), memory_error_occured);

    // append data
    (*(COMP__accountling_abstraction_header*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_abstraction_header);

    return;
}

// get and validate one header
COMP__accountling_abstraction_header COMP__account__get_abstraction_header(COMP__parsling_abstraction abstraction, COMP__error* error) {
    COMP__accountling_abstraction_header output;

    output.name = abstraction.name;
    output.call = COMP__act__user_defined;

    // setup io
    output.inputs = abstraction.inputs;
    output.outputs = abstraction.outputs;

    // calculate counts
    output.input_count = output.inputs.filled_index / sizeof(COMP__parsling_argument);
    output.output_count = output.outputs.filled_index / sizeof(COMP__parsling_argument);

    // validate inputs
    for (ANVIL__length i = 0; i < output.input_count; i++) {
        // get argument
        COMP__parsling_argument argument = ((COMP__parsling_argument*)output.inputs.buffer.start)[i];

        // check argument validity
        if (argument.type != COMP__pat__variable) {
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Accounting Error: An abstraction input header argument contains illegal argument type.", ANVIL__bt__true, ANVIL__bt__false), argument.text.lexling.file_index, argument.text.lexling.line_number, argument.text.lexling.character_index);

            return output;
        }
    }

    // validate outputs
    for (ANVIL__length i = 0; i < output.output_count; i++) {
        // get argument
        COMP__parsling_argument argument = ((COMP__parsling_argument*)output.outputs.buffer.start)[i];

        // check argument validity
        if (argument.type != COMP__pat__variable) {
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Accounting Error: An abstraction output header argument contains illegal argument type.", ANVIL__bt__true, ANVIL__bt__false), argument.text.lexling.file_index, argument.text.lexling.line_number, argument.text.lexling.character_index);

            return output;
        }
    }

    return output;
}

// create call blueprint
ANVIL__list COMP__generate__call_blueprint(ANVIL__list parsling_programs, COMP__error* error) {
    const char* names[] = {
        "frost.set",
        "frost.debug.print.integer",
    };
    const COMP__blueprintling blueprint[] = {
        COMP__abt__define_call,
            COMP__act__set,
            COMP__bnit__set,
            1,
            COMP__pat__literal__boolean,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__set,
            COMP__bnit__set,
            1,
            COMP__pat__literal__integer,
            1,
            COMP__pat__variable,
        COMP__abt__define_call,
            COMP__act__debug_print_integer,
            COMP__bnit__debug_print_integer,
            1,
            COMP__pat__variable,
            0,
        COMP__abt__end_of_blueprint,
    };

    // open output
    ANVIL__bt memory_error_occured = ANVIL__bt__false;
    ANVIL__list output = ANVIL__open__list(sizeof(COMP__accountling_abstraction_header) * 256, &memory_error_occured);
    if (memory_error_occured == ANVIL__bt__true) {
        *error = COMP__create__internal_memory_error();

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
            *error = COMP__create__internal_memory_error();

            return output;
        }

        // open new header
        COMP__accountling_abstraction_header header;

        // set call type
        header.call = (COMP__act)(*(COMP__blueprintling*)current_blueprintling.start);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // set name
        header.name = COMP__create__name(COMP__create__lexling(COMP__lt__name, ANVIL__open__buffer_from_string((ANVIL__u8*)names[*(COMP__blueprintling*)current_blueprintling.start], ANVIL__bt__false, ANVIL__bt__false), -1, -1, -1));
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // get input count
        header.input_count = (*(COMP__blueprintling*)current_blueprintling.start);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // open inputs
        header.inputs = ANVIL__open__list(sizeof(COMP__parsling_argument) * 8, &memory_error_occured);

        // get inputs
        for (COMP__blueprintling i = 0; i < header.input_count; i++) {
            // add argument
            COMP__append__parsling_argument(&header.inputs, COMP__create__parsling_argument((COMP__pat)(*(COMP__blueprintling*)current_blueprintling.start), COMP__create_null__name(), 0), &memory_error_occured);

            // next blueprintling
            current_blueprintling.start += sizeof(COMP__blueprintling);
        }

        // get output count
        header.output_count = (*(COMP__blueprintling*)current_blueprintling.start);
        current_blueprintling.start += sizeof(COMP__blueprintling);

        // open outputs
        header.outputs = ANVIL__open__list(sizeof(COMP__parsling_argument) * 8, &memory_error_occured);

        // get outputs
        for (COMP__blueprintling i = 0; i < header.output_count; i++) {
            // add argument
            COMP__append__parsling_argument(&header.outputs, COMP__create__parsling_argument((COMP__pat)(*(COMP__blueprintling*)current_blueprintling.start), COMP__create_null__name(), 0), &memory_error_occured);

            // next blueprintling
            current_blueprintling.start += sizeof(COMP__blueprintling);
        }

        // append header
        COMP__append__accountling_abstraction_header(&output, header, &memory_error_occured);
        if (memory_error_occured == ANVIL__bt__true) {
            *error = COMP__create__internal_memory_error();

            return output;
        }
    }

    // get all user defined headers
    // setup current
    COMP__current current_file = COMP__calculate__current_from_list_filled_index(&parsling_programs);

    // across all files
    while (COMP__check__current_within_range(current_file)) {
        // setup current
        COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&(*(COMP__parsling_program*)current_file.start).abstractions);

        // get each abstraction header
        while (COMP__check__current_within_range(current_abstraction)) {
            // get abstraction header
            COMP__accountling_abstraction_header header = COMP__account__get_abstraction_header(*(COMP__parsling_abstraction*)current_abstraction.start, error);

            // check for error
            if (COMP__check__error_occured(error)) {
                return output;
            }

            // append header
            COMP__append__accountling_abstraction_header(&output, header, &memory_error_occured);
            if (memory_error_occured == ANVIL__bt__true) {
                *error = COMP__create__internal_memory_error();

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

ANVIL__bt COMP__check__accountling_header_exists(ANVIL__list call_blueprint, COMP__parsling_statement searching_for) {
    // search the blueprint for the correct header
    // setup current
    COMP__current current_header = COMP__calculate__current_from_list_filled_index(&call_blueprint);

    // search for match
    while (COMP__check__current_within_range(current_header)) {
        // get current header
        COMP__accountling_abstraction_header header = *(COMP__accountling_abstraction_header*)current_header.start;

        // check statement type and names
        if (searching_for.type == COMP__st__abstraction_call) {
            if(COMP__check__name_data_is_identical(header.name, searching_for.name)) {
                // DEBUG
                printf("\t\tNames were identical: ");
                ANVIL__print__buffer(header.name.lexling.value);
                printf(" == ");
                ANVIL__print__buffer(searching_for.name.lexling.value);
                printf("\n");

                // check if io is same size
                if ((ANVIL__calculate__lists_have_same_fill_size(&searching_for.inputs, &header.inputs) && ANVIL__calculate__lists_have_same_fill_size(&searching_for.outputs, &header.outputs)) == ANVIL__bt__false) {
                    // DEBUG
                    printf("\t\t\tIO lengths do not match. [ %lu, %lu ][ %lu, %lu ]\n", searching_for.inputs.filled_index, searching_for.outputs.filled_index, header.inputs.filled_index, header.outputs.filled_index);

                    goto next_header;
                }

                // setup currents for input
                COMP__current current_statement_io = COMP__calculate__current_from_list_filled_index(&searching_for.inputs);
                COMP__current current_header_io = COMP__calculate__current_from_list_filled_index(&header.inputs);

                // if inputs are not empty
                if (ANVIL__check__empty_buffer(current_statement_io) == ANVIL__bt__false) {
                    // check inputs
                    while (COMP__check__current_within_range(current_statement_io) && COMP__check__current_within_range(current_header_io)) {
                        // check types
                        if ((*(COMP__parsling_argument*)current_statement_io.start).type != (*(COMP__parsling_argument*)current_header_io.start).type) {
                            // DEBUG
                            printf("\t\tInputs did not match.\n");

                            // not a match
                            goto next_header;
                        }

                        // next input
                        current_statement_io.start += sizeof(COMP__parsling_argument);
                        current_header_io.start += sizeof(COMP__parsling_argument);
                    }
                }

                // re-setup currents
                current_statement_io = COMP__calculate__current_from_list_filled_index(&searching_for.outputs);
                current_header_io = COMP__calculate__current_from_list_filled_index(&header.outputs);

                // if outputs are not empty
                if (ANVIL__check__empty_buffer(current_statement_io) == ANVIL__bt__false) {
                    // check outputs
                    while (COMP__check__current_within_range(current_statement_io) && COMP__check__current_within_range(current_header_io)) {
                        // check types
                        if ((*(COMP__parsling_argument*)current_statement_io.start).type != (*(COMP__parsling_argument*)current_header_io.start).type) {
                            // DEBUG
                            printf("\t\tOutputs did not match.\n");

                            // not a match
                            goto next_header;
                        }

                        // next input
                        current_statement_io.start += sizeof(COMP__parsling_argument);
                        current_header_io.start += sizeof(COMP__parsling_argument);
                    }
                }

                // DEBUG
                printf("\t\t\tMatch Found: ");
                ANVIL__print__buffer(searching_for.name.lexling.value);
                printf("\n");

                // match!
                return ANVIL__bt__true;
            // DEBUG
            } else {
                printf("\t\tNames were not identical: ");
                ANVIL__print__buffer(searching_for.name.lexling.value);
                printf(" != ");
                ANVIL__print__buffer(header.name.lexling.value);
                printf("\n");
            }
        // DEBUG
        } else {
            printf("\t\tStatement was not a call.\n");
        }

        // next header
        next_header:
        current_header.start += sizeof(COMP__accountling_abstraction_header);
    }

    // match not found
    return ANVIL__bt__false;
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
                    if (COMP__check__accountling_header_exists(call_blueprint, searching_for) == ANVIL__bt__false) {
                        // does not exist
                        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Accounting Error: A statement calls a non-existent abstraction / instruction.", ANVIL__bt__true, ANVIL__bt__false), searching_for.name.lexling.file_index, searching_for.name.lexling.line_number, searching_for.name.lexling.character_index);

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

// close blueprint
void COMP__close__accountling_call_blueprint(ANVIL__list call_blueprint) {
    COMP__current current = COMP__calculate__current_from_list_filled_index(&call_blueprint);

    // clean up non-predefined headers
    while (COMP__check__current_within_range(current)) {
        // get header
        COMP__accountling_abstraction_header header = *(COMP__accountling_abstraction_header*)current.start;

        // if header is compiler defined
        if (header.call < COMP__act__user_defined) {
            // header must have it's arguments closed
            COMP__close__accountling_abstraction_header(header);
        }

        // next header
        current.start += sizeof(COMP__accountling_abstraction_header);
    }

    // clean up list
    ANVIL__close__list(call_blueprint);

    return;
}

// print call header
void COMP__print__call_header(COMP__accountling_abstraction_header header) {
    // print tabs
    printf("\t");

    // print name
    ANVIL__print__buffer(header.name.lexling.value);

    // print input types
    printf("(");
    for (COMP__input_count i = 0; i < header.input_count; i++) {
        // print separator
        if (i != 0) {
            printf(" ");
        }

        // get type
        COMP__pat type = ((COMP__pat*)header.inputs.buffer.start)[i];

        // print type
        if (type == COMP__pat__variable) {
            printf("[variable]");
        } else if (type == COMP__pat__offset) {
            printf("[offset]");
        } else if (type == COMP__pat__flag) {
            printf("[flag]");
        } else if (type == COMP__pat__literal__binary) {
            printf("[binary_literal]");
        } else if (type == COMP__pat__literal__boolean) {
            printf("[boolean_literal]");
        } else if (type == COMP__pat__literal__integer) {
            printf("[integer_literal]");
        } else if (type == COMP__pat__literal__hexadecimal) {
            printf("[hexadecimal_literal]");
        } else if (type == COMP__pat__literal__string) {
            printf("[string_literal]");
        } else {
            printf("[invalid]");
        }
    }
    printf(")");

    // print output types
    printf("(");
    for (COMP__output_count i = 0; i < header.output_count; i++) {
        // print separator
        if (i != 0) {
            printf(" ");
        }

        // get type
        COMP__pat type = ((COMP__pat*)header.outputs.buffer.start)[i];

        // print type
        if (type == COMP__pat__variable) {
            printf("[variable]");
        } else if (type == COMP__pat__offset) {
            printf("[offset]");
        } else if (type == COMP__pat__flag) {
            printf("[flag]");
        } else if (type == COMP__pat__literal__binary) {
            printf("[binary_literal]");
        } else if (type == COMP__pat__literal__boolean) {
            printf("[boolean_literal]");
        } else if (type == COMP__pat__literal__integer) {
            printf("[integer_literal]");
        } else if (type == COMP__pat__literal__hexadecimal) {
            printf("[hexadecimal_literal]");
        } else if (type == COMP__pat__literal__string) {
            printf("[string_literal]");
        } else {
            printf("[invalid]");
        }
    }
    printf(")");

    // print next line
    printf("\n");

    return;
}

// print call blueprint
void COMP__print__call_blueprint(ANVIL__list blueprint) {
    // print starting message
    printf("All Abstraction Call Headers:\n");

    // setup current
    COMP__current current = COMP__calculate__current_from_list_filled_index(&blueprint);

    // print all headers
    while (COMP__check__current_within_range(current)) {
        // print individual header
        COMP__print__call_header(*(COMP__accountling_abstraction_header*)current.start);

        // next header
        current.start += sizeof(COMP__accountling_abstraction_header);
    }

    return;
}

// account program
void COMP__account__program(ANVIL__list parsling_programs, COMP__error* error) {
    // create headers
    ANVIL__list call_blueprint = COMP__generate__call_blueprint(parsling_programs, error);

    // print headers
    COMP__print__call_blueprint(call_blueprint);

    // DEBUG
    goto clean_up;

    // setup current
    COMP__current current_parsling_program = COMP__calculate__current_from_list_filled_index(&parsling_programs);

    // get program
    COMP__parsling_program program = *(COMP__parsling_program*)current_parsling_program.start;

    // account all programs
    while (COMP__check__current_within_range(current_parsling_program)) {
        // setup current
        COMP__current current_parsling_abstraction = COMP__calculate__current_from_list_filled_index(&program.abstractions);

        // account all abstractions in program
        while (COMP__check__current_within_range(current_parsling_abstraction)) {
            // get parsling abstraction
            COMP__parsling_abstraction parsling_abstraction = *(COMP__parsling_abstraction*)current_parsling_abstraction.start;

            // account one abstraction
            //COMP__accountling_abstraction accountling_abstraction = COMP__account__abstraction();

            // check error
            if (COMP__check__error_occured(error) == ANVIL__bt__true) {
                goto clean_up;
            }

            // next abstraction
            current_parsling_abstraction.start += sizeof(COMP__parsling_abstraction);
        }

        // next program
        current_parsling_program.start += sizeof(COMP__parsling_program);
    }

    // verify statments in all user defined abstractions
    //COMP__account__verify_all_calls(parsling_programs, call_blueprint, error);
    //if (COMP__check__error_occured(error)) {
    //    goto clean_up;
    //}

    // clean up
    clean_up:
    COMP__close__accountling_call_blueprint(call_blueprint);

    return;
}*/

/* Generation */
// generation offsets
typedef struct COMP__generation_offsets {
    ANVIL__offset function_start; // the first instruction in the function
    ANVIL__offset function_return; // the first instruction passing output
    ANVIL__offset function_data; // after the code of the function, the embedded data's place
    ANVIL__buffer internal_offsets; // the function specific offsets
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
    output.function_input_range = COMP__calculate__generation_cell_range(ANVIL__srt__start__function_io, input_count - 1);
    output.function_output_range = COMP__calculate__generation_cell_range(ANVIL__srt__start__function_io, output_count - 1);
    output.workspace_input_range = COMP__calculate__generation_cell_range(ANVIL__srt__start__workspace, input_count - 1);
    output.workspace_output_range = COMP__calculate__generation_cell_range(output.workspace_input_range.end + 1, output_count - 1);
    output.workspace_body_range = COMP__calculate__generation_cell_range(output.workspace_output_range.end + 1, variable_count - 1);
    output.workspace_total_range = COMP__create__generation_cell_range(output.workspace_input_range.start, output.workspace_body_range.end);

    return output;
}

// generation argument
typedef struct COMP__generation_argument {
    ANVIL__cell_ID cell_ID;
} COMP__generation_argument;

// create generation argument
COMP__generation_argument COMP__create__generation_argument(ANVIL__cell_ID cell_ID) {
    COMP__generation_argument output;

    output.cell_ID = cell_ID;

    return output;
}

// create null generation argument
COMP__generation_argument COMP__create_null__generation_argument() {
    return COMP__create__generation_argument(ANVIL__unused_cell_ID);
}

// generation statement type
typedef enum COMP__gst {
    // invalid
    COMP__gst__invalid,

    // types
    COMP__gst__instruction_call,
    COMP__gst__abstraction_call,
    COMP__gst__offset_declaration,

    // COUNT
    COMP__gst__COUNT,
} COMP__gst;

// generation statement
typedef struct COMP__generation_statement {
    COMP__gst type;

    // instruction calls
    COMP__instruction_index instruction_ID; // ANVIL__it / ANVIL__sit
    ANVIL__cell write_register_value;
    ANVIL__flag_ID do_flag;
    ANVIL__operation_ID operation;
    ANVIL__cell_ID input_0;
    ANVIL__cell_ID input_1;
    ANVIL__cell_ID input_2;
    ANVIL__cell_ID input_3;
    ANVIL__cell_ID output_0;
    ANVIL__cell_ID output_1;
    ANVIL__cell section_length;
    ANVIL__flag_ID invert_flag;

    // abstraction calls
    COMP__call_index user_defined_call_ID;
    ANVIL__list inputs;
    ANVIL__list outputs;

    // offset declarations
    COMP__offset_index offset_ID;
} COMP__generation_statement;

// create instruction call statement
COMP__generation_statement COMP__create__generation_statement__instruction_call(COMP__instruction_index instruction_ID, ANVIL__cell write_register_value, ANVIL__flag_ID do_flag, ANVIL__operation_ID operation, ANVIL__cell_ID input_0, ANVIL__cell_ID input_1, ANVIL__cell_ID input_2, ANVIL__cell_ID input_3, ANVIL__cell_ID output_0, ANVIL__cell_ID output_1, ANVIL__cell section_length, ANVIL__flag_ID invert_flag) {
    COMP__generation_statement output;

    output.type = COMP__gst__instruction_call;
    output.instruction_ID = instruction_ID;
    output.write_register_value = write_register_value;
    output.do_flag = do_flag;
    output.operation = operation;
    output.input_0 = input_0;
    output.input_1 = input_1;
    output.input_2 = input_2;
    output.input_3 = input_3;
    output.output_0 = output_0;
    output.output_1 = output_1;
    output.section_length = section_length;
    output.invert_flag = invert_flag;

    return output;
}

// create null statement
COMP__generation_statement COMP__create__generation_statement__invalid() {
    COMP__generation_statement output;

    output.type = COMP__gst__invalid;

    return output;
}

// append generation statement
void COMP__append__generation_argument(ANVIL__list* list, COMP__generation_argument data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__generation_argument), memory_error_occured);

    // append data
    (*(COMP__generation_argument*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__generation_argument);

    return;
}

// append generation statement
void COMP__append__generation_statement(ANVIL__list* list, COMP__generation_statement data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__generation_statement), memory_error_occured);

    // append data
    (*(COMP__generation_statement*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__generation_statement);

    return;
}

// generation abstraction
typedef struct COMP__generation_abstraction {
    COMP__generation_cells cells;
    COMP__generation_offsets offsets;
    ANVIL__list statements;
} COMP__generation_abstraction;

// setup test abstraction
COMP__generation_abstraction COMP__setup__test_abstraction() {
    COMP__generation_abstraction output;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // setup
    output.cells = COMP__setup__generation_cells(5, 3, 5);
    output.statements = ANVIL__open__list(sizeof(COMP__generation_statement) * 32, &memory_error_occured);

    // create statements
    COMP__append__generation_statement(&output.statements, COMP__create__generation_statement__instruction_call(ANVIL__it__write_cell, (ANVIL__cell)543210, ANVIL__sft__always_run, ANVIL__ot__COUNT, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__start__workspace, ANVIL__unused_cell_ID, ANVIL__define__null_address, ANVIL__sft__never_run), &memory_error_occured);
    COMP__append__generation_statement(&output.statements, COMP__create__generation_statement__instruction_call(ANVIL__it__debug__print_cell_as_decimal, ANVIL__define__null_address, ANVIL__sft__always_run, ANVIL__ot__COUNT, ANVIL__srt__start__workspace, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__define__null_address, ANVIL__sft__never_run), &memory_error_occured);
    COMP__append__generation_statement(&output.statements, COMP__create__generation_statement__instruction_call(ANVIL__it__write_cell, (ANVIL__cell)'\n', ANVIL__sft__always_run, ANVIL__ot__COUNT, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__srt__start__workspace + 1, ANVIL__unused_cell_ID, ANVIL__define__null_address, ANVIL__sft__never_run), &memory_error_occured);
    COMP__append__generation_statement(&output.statements, COMP__create__generation_statement__instruction_call(ANVIL__it__debug__putchar, ANVIL__define__null_address, ANVIL__sft__always_run, ANVIL__ot__COUNT, ANVIL__srt__start__workspace + 1, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, ANVIL__define__null_address, ANVIL__sft__never_run), &memory_error_occured);

    return output;
}

// close generation abstraction
void COMP__close__generation_abstraction(COMP__generation_abstraction abstraction) {
    // close statements
    {
        COMP__current current = COMP__calculate__current_from_list_filled_index(&abstraction.statements);

        // for each statement
        while (COMP__check__current_within_range(current)) {
            // get statement
            COMP__generation_statement statement = *(COMP__generation_statement*)current.start;

            // if it is an abstraction call
            if (statement.type == COMP__gst__abstraction_call) {
                // clean up io
                // TODO
            }

            // next statement
            current.start += sizeof(COMP__generation_statement);
        }

        // close statements
        ANVIL__close__list(abstraction.statements);
    }

    return;
}

// generation workspace
typedef struct COMP__generation_workspace {
    ANVIL__workspace anvil;
    ANVIL__buffer abstractions;
} COMP__generation_workspace;

// open workspace
COMP__generation_workspace COMP__open__generation_workspace(ANVIL__buffer* program_buffer, COMP__abstraction_index abstraction_count) {
    COMP__generation_workspace output;

    // setup anvil
    output.anvil = ANVIL__setup__workspace(program_buffer);

    // setup abstractions
    output.abstractions = ANVIL__open__buffer(sizeof(COMP__generation_abstraction) * abstraction_count);

    return output;
}

// close workspace
void COMP__close__generation_workspace(COMP__generation_workspace workspace) {
    COMP__current current = workspace.abstractions;

    // close all abstractions
    while (COMP__check__current_within_range(current)) {
        // close abstraction
        COMP__close__generation_abstraction(*(COMP__generation_abstraction*)current.start);

        // next abstraction
        current.start += sizeof(COMP__generation_abstraction);
    }

    // close abstractions buffer
    ANVIL__close__buffer(workspace.abstractions);

    return;
}

// generate abstraction function
void COMP__generate__anvil_abstraction_body(ANVIL__workspace* workspace, COMP__generation_abstraction* abstraction, COMP__error* error) {
    // setup offset
    (*abstraction).offsets.function_start = ANVIL__get__offset(workspace);

    // preserve workspace
    ANVIL__code__preserve_workspace(workspace, ANVIL__sft__always_run, (*abstraction).cells.workspace_total_range.start, (*abstraction).cells.workspace_total_range.end);

    // build inputs
    for (ANVIL__cell_index input_index = 0; input_index < (*abstraction).cells.input_count; input_index++) {
        // build input
        ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, (*abstraction).cells.function_input_range.start + input_index, (*abstraction).cells.workspace_input_range.start + input_index);
    }

    // build body
    {
        // get current
        COMP__current current = COMP__calculate__current_from_list_filled_index(&((*abstraction).statements));

        // for each statement
        while (COMP__check__current_within_range(current)) {
            // get statement
            COMP__generation_statement statement = *(COMP__generation_statement*)current.start;

            // check type
            if (statement.type == COMP__gst__instruction_call) {
                if (statement.instruction_ID == ANVIL__it__stop) {
                    ANVIL__code__stop(workspace);
                } else if (statement.instruction_ID == ANVIL__it__write_cell) {
                    ANVIL__code__write_cell(workspace, statement.write_register_value, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__it__operate) {
                    ANVIL__code__operate(workspace, statement.do_flag, statement.operation, statement.input_0, statement.input_1, statement.input_2, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__it__request_memory) {
                    ANVIL__code__request_memory(workspace, statement.input_0, statement.output_0, statement.output_1);
                } else if (statement.instruction_ID == ANVIL__it__return_memory) {
                    ANVIL__code__return_memory(workspace, statement.input_0, statement.input_1);
                } else if (statement.instruction_ID == ANVIL__it__address_to_cell) {
                    ANVIL__code__address_to_cell(workspace, statement.do_flag, statement.input_0, statement.input_1, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__it__cell_to_address) {
                    ANVIL__code__cell_to_address(workspace, statement.do_flag, statement.input_0, statement.input_1, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__it__file_to_buffer) {
                    ANVIL__code__file_to_buffer(workspace, statement.input_0, statement.input_1, statement.output_0, statement.output_1);
                } else if (statement.instruction_ID == ANVIL__it__buffer_to_file) {
                    ANVIL__code__buffer_to_file(workspace, statement.input_0, statement.input_1, statement.input_2, statement.input_3);
                } else if (statement.instruction_ID == ANVIL__it__run) {
                    ANVIL__code__run(workspace, statement.input_0, statement.input_1, statement.input_2);
                } else if (statement.instruction_ID == ANVIL__it__debug__putchar) {
                    ANVIL__code__debug__putchar(workspace, statement.input_0);
                } else if (statement.instruction_ID == ANVIL__it__debug__print_cell_as_decimal) {
                    ANVIL__code__debug__print_cell_as_decimal(workspace, statement.input_0);
                } else if (statement.instruction_ID == ANVIL__it__debug__fgets) {
                    ANVIL__code__debug__fgets(workspace, statement.output_0, statement.output_1);
                } else if (statement.instruction_ID == ANVIL__sit__cell_to_cell) {
                    ANVIL__code__cell_to_cell(workspace, statement.do_flag, statement.input_0, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__sit__push_cell) {
                    ANVIL__code__push_cell(workspace, statement.do_flag, statement.input_0);
                } else if (statement.instruction_ID == ANVIL__sit__pop_cell) {
                    ANVIL__code__pop_cell(workspace, statement.do_flag, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__sit__calculate_dynamically__offset_address) {
                    ANVIL__code__calculate_dynamically__offset_address(workspace, statement.do_flag, statement.input_0, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__sit__calculate_statically__offset_address) {
                    // offset needs to be dereferenced! skipping for now
                    // ANVIL__code__calculate_statically__offset_address(workspace, statement.do_flag, );
                } else if (statement.instruction_ID == ANVIL__sit__jump__explicit) {
                    ANVIL__code__jump__explicit(workspace, statement.do_flag, statement.input_0);
                } else if (statement.instruction_ID == ANVIL__sit__jump__static) {
                    //offset needs to be dereferenced! skipping for now
                    //ANVIL__code__jump__static(workspace, statement.do_flag, );
                } else if (statement.instruction_ID == ANVIL__sit__operate__flag) {
                    ANVIL__code__operate__flag(workspace, statement.do_flag, statement.input_0, statement.input_1, statement.input_2, statement.invert_flag, statement.output_0);
                } else if (statement.instruction_ID == ANVIL__sit__operate__jump__explicit) {
                    ANVIL__code__operate__jump__explicit(workspace, statement.do_flag, statement.input_0, statement.input_1, statement.input_2, statement.invert_flag, statement.input_3);
                } else if (statement.instruction_ID == ANVIL__sit__operate__jump__dynamic) {
                    ANVIL__code__operate__jump__dynamic(workspace, statement.do_flag, statement.input_0, statement.input_1, statement.input_2, statement.invert_flag, statement.input_3);
                } else if (statement.instruction_ID == ANVIL__sit__operate__jump__static) {
                    // offset needs to be dereferenced! skipping for now
                    //ANVIL__code__operate__jump__static(workspace, statement.do_flag, );
                } else if (statement.instruction_ID == ANVIL__sit__setup__context) {
                    ANVIL__code__setup__context(workspace, statement.input_0, statement.input_1, statement.output_0, statement.output_1);
                } else {
                    // set error
                    *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Internal Generation Error: Unsupported instruction type.", ANVIL__bt__true, ANVIL__bt__false), 0, 0, 0);

                    return;
                }
            } else if (statement.type == COMP__gst__abstraction_call) {
                // TODO
            } else if (statement.type == COMP__gst__offset_declaration) {
                // TODO
            } else {
                // set error
                *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Internal Generation Error: Unrecognized statement type.", ANVIL__bt__true, ANVIL__bt__false), 0, 0, 0);

                return;
            }

            // next statement
            current.start += sizeof(COMP__generation_statement);
        }
    }

    // setup return offset
    (*abstraction).offsets.function_return = ANVIL__get__offset(workspace);

    // build outputs
    for (ANVIL__cell_index output_index = 0; output_index < (*abstraction).cells.output_count; output_index++) {
        // build input
        ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, (*abstraction).cells.workspace_output_range.start + output_index, (*abstraction).cells.function_output_range.start + output_index);
    }

    // restore workspace
    ANVIL__code__restore_workspace(workspace, ANVIL__sft__always_run, (*abstraction).cells.workspace_total_range.start, (*abstraction).cells.workspace_total_range.end);

    // return to caller
    ANVIL__code__jump__explicit(workspace, ANVIL__sft__always_run, ANVIL__srt__return_address);

    // setup function data start
    (*abstraction).offsets.function_data = ANVIL__get__offset(workspace);

    // append buffers
    // TODO

    return;
}

// generate package
void COMP__generate__anvil_package(ANVIL__workspace* workspace, ANVIL__stack_size stack_size, COMP__generation_abstraction* abstraction, COMP__error* error) {
    // setup kickstarter
    ANVIL__code__start(workspace, stack_size, (*abstraction).offsets.function_start);

    // setup abstraction
    COMP__generate__anvil_abstraction_body(workspace, abstraction, error);

    return;
}

// generate program
ANVIL__buffer COMP__generate_debug__anvil_program(COMP__generation_abstraction* abstraction, COMP__error* error) {
    ANVIL__buffer output;
    ANVIL__workspace workspace;
    
    // create workspace
    workspace = ANVIL__setup__workspace(&output);

    // create program
    for (ANVIL__pt pass = ANVIL__pt__get_offsets; pass < ANVIL__pt__COUNT; pass++) {
        // setup pass
        ANVIL__setup__pass(&workspace, pass);

        // build program
        COMP__generate__anvil_package(&workspace, 1024, abstraction, error);
    }

    return output;
}

/* Compile */
// one compiled object across multiple stages
typedef struct COMP__compilation_unit {
    ANVIL__list user_codes;
    ANVIL__list lexling_buffers;
    ANVIL__list parsling_buffers;
    //COMP__accountling_program accountlings;
} COMP__compilation_unit;

// close a compilation unit
void COMP__close__compilation_unit(COMP__compilation_unit unit) {
    // close user codes (Assumes that user codes are allocated elsewhere!)
    ANVIL__close__list(unit.user_codes);

    // close lexling buffers
    {
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
    {
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

    // close accountlings
    //COMP__close__accountling_program(unit.accountlings);

    return;
}

// compile a program
void COMP__compile__files(ANVIL__list user_codes, ANVIL__bt print_debug, COMP__error* error) {
    COMP__compilation_unit compilation_unit;
    COMP__current current;
    COMP__file_index current_file_index = 0;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // setup blank error
    *error = COMP__create_null__error();

    // setup compilation unit user codes
    compilation_unit.user_codes = ANVIL__open__list(sizeof(ANVIL__buffer) * 5, &memory_error_occured);
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();

        goto quit;
    }

    // setup compilation unit lexling buffers
    compilation_unit.lexling_buffers = ANVIL__open__list(sizeof(COMP__lexlings) * 5, &memory_error_occured);
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();

        goto quit;
    }

    // setup compilation unit parsling buffers
    compilation_unit.parsling_buffers = ANVIL__open__list(sizeof(COMP__parsling_program) * 5, &memory_error_occured);
    if (memory_error_occured == ANVIL__bt__true) {
        // set error
        *error = COMP__create__internal_memory_error();

        goto quit;
    }

    // setup current
    current = COMP__calculate__current_from_list_filled_index(&user_codes);

    // print compilation message
    printf("Compiling Files.\n");

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

        // append file
        ANVIL__list__append__buffer(&compilation_unit.user_codes, user_code, &memory_error_occured);

        // check for error
        if (memory_error_occured == ANVIL__bt__true) {
            // set error
            *error = COMP__create__internal_memory_error();

            goto quit;
        }

        // lex file
        COMP__lexlings lexlings = COMP__compile__lex(user_code, current_file_index, error);

        // append lexlings
        COMP__append__lexlings(&compilation_unit.lexling_buffers, lexlings, &memory_error_occured);

        // print lexlings
        if (print_debug) {
            COMP__debug__print_lexlings(lexlings);
        }

        // check for errors
        if (memory_error_occured == ANVIL__bt__true) {
            // set error
            *error = COMP__create__internal_memory_error();

            goto quit;
        }
        if (COMP__check__error_occured(error)) {
            goto quit;
        }

        // parse file
        COMP__parsling_program parslings = COMP__parse__program(lexlings, error);

        // append program
        COMP__append__parsling_program(&compilation_unit.parsling_buffers, parslings, &memory_error_occured);

        // print parslings
        if (print_debug) {
            COMP__print__parsed_program(parslings);
        }

        // check for errors
        if (memory_error_occured == ANVIL__bt__true) {
            // set error
            *error = COMP__create__internal_memory_error();

            goto quit;
        }
        if (COMP__check__error_occured(error)) {
            goto quit;
        }

        // advance current
        current.start += sizeof(ANVIL__buffer);
        current_file_index++;
    }

    // account
    /*compilation_unit.accountlings = COMP__account__program(compilation_unit.parsling_buffers, error);

    // print accounted program
    if (print_debug) {
        COMP__print__accounted_program(compilation_unit.accountlings);
    }

    // check for errors
    if (COMP__check__error_occured(error)) {
        goto quit;
    }*/

    // account
    //COMP__account__program(compilation_unit.parsling_buffers, error);

    // quit label
    quit:

    // clean up
    COMP__close__compilation_unit(compilation_unit);

    return;
}

#endif
