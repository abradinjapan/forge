#ifndef FROST__compiler
#define FROST__compiler

/* Include */
// anvil / basic
#include "../anvil/anvil.h"

/* Define */
// aliases
typedef ANVIL__buffer COMP__current;

// error types
typedef ANVIL__u64 COMP__file_number;
typedef ANVIL__u64 COMP__line_number;
typedef ANVIL__u64 COMP__character_index;

// lexing types
typedef ANVIL__u8 COMP__lexling_type;
typedef ANVIL__address COMP__lexling_address;
typedef COMP__lexling_address COMP__lexling_start;
typedef COMP__lexling_address COMP__lexling_end;
typedef ANVIL__u64 COMP__lexling_index;

// accounting types
typedef ANVIL__u64 COMP__header_input_count;
typedef ANVIL__u64 COMP__header_output_count;
typedef ANVIL__u64 COMP__variable_index;
typedef ANVIL__u64 COMP__call_index;
typedef ANVIL__u64 COMP__offset_index;

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
    COMP__file_number file_number;
    COMP__line_number line_number;
    COMP__character_index character_index;
} COMP__error;

// create custom error
COMP__error COMP__create__error(ANVIL__bt occured, ANVIL__buffer message, COMP__file_number file_number, COMP__line_number line_number, COMP__character_index character_index) {
    COMP__error output;

    // setup output
    output.occured = occured;
    output.message = message;
    output.file_number = file_number;
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
    ANVIL__list__append__buffer_data(&json, ANVIL__open__buffer_from_string((u8*)"\",\n\t\"file_number\": \"", ANVIL__bt__false, ANVIL__bt__false), error_occured);
    temp_buffer = COMP__translate__integer_value_to_string(error.file_number);
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

// calculate a current buffer from a list
COMP__current COMP__calculate__current_from_list_filled_index(ANVIL__list* list) {
    // return calculation
    return ANVIL__create__buffer((*list).buffer.start, (*list).buffer.start + (*list).filled_index - 1);
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
    COMP__lt__equals,
    COMP__lt__string_literal,
    COMP__lt__COUNT,
} COMP__lt;

// lexling type
typedef struct COMP__lexling {
    COMP__lexling_type type;
    ANVIL__buffer value;
    COMP__line_number line_number;
    COMP__character_index character_index;
} COMP__lexling;

// create custom lexling
COMP__lexling COMP__create__lexling(COMP__lexling_type type, ANVIL__buffer value, COMP__line_number line_number, COMP__character_index character_index) {
    COMP__lexling output;

    // setup output
    output.type = type;
    output.value = value;
    output.line_number = line_number;
    output.character_index = character_index;

    return output;
}

// create null lexling
COMP__lexling COMP__create_null__lexling() {
    // return empty
    return COMP__create__lexling(COMP__lt__invalid, ANVIL__create_null__buffer(), 0, 0);
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
COMP__lexlings COMP__compile__lex(ANVIL__buffer user_code, COMP__file_number file_number, COMP__error* error) {
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
    current_line_number = 0;

    // lex program
    while (current.start < user_code.end) {
        // skip whitespace
        if (COMP__calculate__valid_character_range(current, 0, 32)) {
            // check for new line
            if (COMP__calculate__valid_character_range(current, '\n', '\n') || COMP__calculate__valid_character_range(current, '\r', '\r')) {
                // next line
                current_line_number++;
            }

            // next character
            current.start += sizeof(ANVIL__character);

            // try next character
            continue;
        }

        // check for lexlings
        if (COMP__calculate__valid_character_range(current, '(', '(')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_parenthesis, ANVIL__create__buffer(current.start, current.start), current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, ')', ')')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_parenthesis, ANVIL__create__buffer(current.start, current.start), current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '{', '{')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_curly_bracket, ANVIL__create__buffer(current.start, current.start), current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '}', '}')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_curly_bracket, ANVIL__create__buffer(current.start, current.start), current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_name_character(current)) {
            // get lexling start and setup temp end
            temp_start = current.start;
            temp_end = temp_start - 1;

            // get lexling size
            while (current.start < user_code.end && COMP__calculate__valid_name_character(current)) {
                // next character
                current.start += sizeof(ANVIL__character);
                temp_end += sizeof(ANVIL__character);
            }

            // record lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__name, ANVIL__create__buffer(temp_start, temp_end), current_line_number, COMP__calculate__character_index(user_code, ANVIL__create__buffer(temp_start, current.end))), &memory_error_occured);
        } else if (COMP__calculate__valid_character_range(current, '@', '@')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__at, ANVIL__create__buffer(current.start, current.start), current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '=', '=')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__equals, ANVIL__create__buffer(current.start, current.start), current_line_number, COMP__calculate__character_index(user_code, current)), &memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        // no lexling found
        } else {
            // open error
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Lexical Error: Invalid character.", ANVIL__bt__true, ANVIL__bt__false), file_number, current_line_number, COMP__calculate__character_index(user_code, current));

            // quit
            return output;
        }

        // check for error
        if (memory_error_occured == ANVIL__bt__true) {
            // set error
            *error = COMP__create__internal_memory_error();

            // return lexlings as they are
            return output;
        }
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
        printf("\t%lu [ %lu, %lu ] [ line_index: %lu, character_index: %lu ] : ", (ANVIL__u64)temp.type, (ANVIL__u64)temp.value.start, (ANVIL__u64)temp.value.end, (ANVIL__u64)temp.line_number, (ANVIL__u64)temp.character_index);
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

// check if two names are exactly the same
ANVIL__bt COMP__check__name_data_is_identical(COMP__name a, COMP__name b) {
    // check if names are same length
    if (ANVIL__calculate__buffer_length(a.lexling.value) != ANVIL__calculate__buffer_length(b.lexling.value)) {
        // not same length so not identical
        return ANVIL__bt__false;
    }

    // get pointers
    ANVIL__address a_current = a.lexling.value.start;
    ANVIL__address b_current = b.lexling.value.start;

    // check each character
    while (a_current <= a.lexling.value.end) {
        // check character
        if (*(ANVIL__character*)a_current != *(ANVIL__character*)b_current) {
            // character not identical, string not identical
            return ANVIL__bt__false;
        }

        // next characters
        a_current += sizeof(ANVIL__character);
        b_current += sizeof(ANVIL__character);
    }

    // no issues found, buffers are identical
    return ANVIL__bt__true;
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
// statement type
typedef enum COMP__st {
    COMP__st__abstraction_call,
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
    ANVIL__list inputs;
    ANVIL__list outputs;
} COMP__parsling_statement;

// create a custom statement
COMP__parsling_statement COMP__create__parsling_statement(COMP__st type, COMP__name name, ANVIL__list inputs, ANVIL__list outputs) {
    COMP__parsling_statement output;

    // setup output
    output.type = type;
    output.name = name;
    output.inputs = inputs;
    output.outputs = outputs;

    return output;
}

// create a null statement
COMP__parsling_statement COMP__create_null__parsling_statement() {
    // return empty
    return COMP__create__parsling_statement(COMP__st__COUNT, COMP__create_null__name(), ANVIL__create_null__list(), ANVIL__create_null__list());
}

// one abstraction
typedef struct COMP__parsling_abstraction {
    // abstraction name
    COMP__name name;

    // io
    ANVIL__list inputs;
    ANVIL__list outputs;

    // body
    ANVIL__list statements;
} COMP__parsling_abstraction;

// create a custom abstraction
COMP__parsling_abstraction COMP__create__parsling_abstraction(COMP__name name, ANVIL__list inputs, ANVIL__list outputs, ANVIL__list statements) {
    COMP__parsling_abstraction output;

    // setup output
    output.name = name;
    output.inputs = inputs;
    output.outputs = outputs;
    output.statements = statements;

    return output;
}

// create a null abstraction
COMP__parsling_abstraction COMP__create_null__parsling_abstraction() {
    // return empty
    return COMP__create__parsling_abstraction(COMP__create_null__name(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list());
}

// one program
typedef struct COMP__parsling_program {
    ANVIL__list abstractions;
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
    // close io
    ANVIL__close__list(abstraction.inputs);
    ANVIL__close__list(abstraction.outputs);
    
    // close statements
    COMP__close__parsling_statements(abstraction.statements);

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

// parse arguments
ANVIL__list COMP__parse__arguments(COMP__current* current, COMP__file_number current_file_number, COMP__error* error) {
    ANVIL__list output;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // open names list
    output = ANVIL__open__list(sizeof(COMP__name) * 8, &memory_error_occured);

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
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Arguments is missing opening parethesis.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

        return output;
    }

    // get arguments
    while (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
        // get argument
        COMP__append__name(&output, COMP__create__name(COMP__read__lexling_from_current(*current)), &memory_error_occured);
        //ANVIL__list__append__buffer(&output, COMP__read__lexling_from_current(*current).value, &memory_error_occured);

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
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Arguments is missing closing parethesis.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

        return output;
    }

    return output;
}

// parse a statement
ANVIL__list COMP__parse__scope(COMP__current* current, COMP__file_number current_file_number, COMP__error* error) {
    ANVIL__list output;
    COMP__parsling_statement temp;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // create list
    output = ANVIL__open__list(sizeof(COMP__parsling_statement), &memory_error_occured);

    // check for scope opener
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__left_curly_bracket) {
        // advance current
        COMP__advance__lexling_current(current, 1);
    // scope opener not found, error
    } else {
        // set error
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Scope is missing left curly bracket.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);
        
        return output;
    }

    // get statements
    while (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type != COMP__lt__right_curly_bracket) {
        // check for offset
        if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__at) {
            // setup temp
            temp.type = COMP__st__offset;

            // advance current
            COMP__advance__lexling_current(current, 1);

            // check for offset name
            if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
                // set name
                temp.name = COMP__create__name_from_lexling_current(*current);

                // advance current
                COMP__advance__lexling_current(current, 1);
            // invalid syntax
            } else {
                // set error
                *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Offset statement name is an invalid lexling.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

                return output;
            }

            // null initialize unused data
            temp.inputs = ANVIL__create_null__list();
            temp.outputs = ANVIL__create_null__list();

            // add statement
            COMP__append__parsling_statement(&output, temp, &memory_error_occured);

            // check for error
            if (memory_error_occured == ANVIL__bt__true) {
                // set error
                *error = COMP__create__internal_memory_error();

                return output;
            }
        // is an abstraction call
        } else if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
            // set type
            temp.type = COMP__st__abstraction_call;

            // get name
            temp.name = COMP__create__name_from_lexling_current(*current);

            // advance current
            COMP__advance__lexling_current(current, 1);

            // get inputs
            temp.inputs = COMP__parse__arguments(current, current_file_number, error);

            // check for error
            if (COMP__check__error_occured(error) == ANVIL__bt__true) {
                return output;
            }

            // get outputs
            temp.outputs = COMP__parse__arguments(current, current_file_number, error);

            // append statement
            COMP__append__parsling_statement(&output, temp, &memory_error_occured);

            // check for error
            if (COMP__check__error_occured(error) == ANVIL__bt__true) {
                return output;
            }

            // check for error
            if (memory_error_occured == ANVIL__bt__true) {
                // set error
                *error = COMP__create__internal_memory_error();

                return output;
            }
        // error
        } else {
            // set error
            *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Unrecognized statement type.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

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
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: Scope is missing right curly bracket.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);
        
        return output;
    }

    return output;
}

// parse an abstraction
COMP__parsling_abstraction COMP__parse__abstraction(COMP__current* current, COMP__file_number current_file_number, COMP__error* error) {
    COMP__parsling_abstraction output = COMP__create_null__parsling_abstraction();

    // check for name
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
        // get name
        output.name = COMP__create__name_from_lexling_current(*current);

        // next lexling
        COMP__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: An abstraction definition has a name missing.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

        // return blank
        return output;
    }

    // get inputs
    output.inputs = COMP__parse__arguments(current, current_file_number, error);

    // check for error
    if (COMP__check__error_occured(error) == ANVIL__bt__true) {
        // quit
        return output;
    }

    // get outputs
    output.outputs = COMP__parse__arguments(current, current_file_number, error);

    // check for error
    if (COMP__check__error_occured(error) == ANVIL__bt__true) {
        // quit
        return output;
    }

    // check for equals sign
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__equals) {
        // no saving data necessary, next lexling
        COMP__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Parse Error: An abstraction definition has an equals sign missing.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, COMP__read__lexling_from_current(*current).line_number, COMP__read__lexling_from_current(*current).character_index);

        // quit
        return output;
    }

    // parse statements
    output.statements = COMP__parse__scope(current, current_file_number, error);

    return output;
}

// parse a file (program)
COMP__parsling_program COMP__parse__program(COMP__lexlings lexlings, COMP__file_number current_file_number, COMP__error* error) {
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

        return output;
    }

    // parse abstractions
    while (COMP__check__current_within_range(current)) {
        // parse abstraction
        temp = COMP__parse__abstraction(&current, current_file_number, error);

        // append abstraction
        COMP__append__parsling_abstraction(&(output.abstractions), temp, &memory_error_occured);

        // check for error
        if (COMP__check__error_occured(error) == ANVIL__bt__true) {
            return output;
        }
    }

    return output;
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

        // print name
        ANVIL__print__buffer((*(COMP__name*)current.start).lexling.value);

        // next buffer
        current.start += sizeof(COMP__name);
    }

    // print closer
    printf(")");

    return;
}

// print an abstraction
void COMP__print__parsed_abstraction(COMP__parsling_abstraction abstraction) {
    COMP__current current_statement = COMP__calculate__current_from_list_filled_index(&abstraction.statements);
    COMP__parsling_statement temp_statement;

    // print header
    printf("Abstraction: ");
    ANVIL__print__buffer(abstraction.name.lexling.value);

    // print inputs
    COMP__print__arguments(&(abstraction.inputs));

    // print outputs
    COMP__print__arguments(&(abstraction.outputs));

    // print new line for statements
    printf("\n");

    // print each statement
    while (COMP__check__current_within_range(current_statement)) {
        // get statement
        temp_statement = *((COMP__parsling_statement*)current_statement.start);

        // print styling
        printf("\t");

        // print statement
        if (temp_statement.type == COMP__st__offset) {
            // print offset information
            printf("@");
            ANVIL__print__buffer(temp_statement.name.lexling.value);
            printf("\n");
        } else if (temp_statement.type == COMP__st__abstraction_call) {
            // print statement
            ANVIL__print__buffer(temp_statement.name.lexling.value);

            // print inputs
            COMP__print__arguments(&temp_statement.inputs);

            // print outputs
            COMP__print__arguments(&temp_statement.outputs);

            // print new line
            printf("\n");
        }

        // advance current
        current_statement.start += sizeof(COMP__parsling_statement);
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

/* Account */
// one defined abstraction
typedef struct COMP__accountling_header {
    COMP__name name;
    COMP__header_input_count input_count;
    COMP__header_output_count output_count;
} COMP__accountling_header;

// create a custom accountling header
COMP__accountling_header COMP__create__accountling_header(COMP__name name, COMP__header_input_count input_count, COMP__header_output_count output_count) {
    COMP__accountling_header output;

    // setup output
    output.name = name;
    output.input_count = input_count;
    output.output_count = output_count;

    return output;
}

// create a null header
COMP__accountling_header COMP__create_null__accountling_header() {
    // return empty
    return COMP__create__accountling_header(COMP__create_null__name(), 0, 0);
}

// header table
typedef struct COMP__accountling_header_table {
    ANVIL__list headers;
} COMP__accountling_header_table;

// create custom header table
COMP__accountling_header_table COMP__create__accountling_header_table(ANVIL__list headers) {
    COMP__accountling_header_table output;

    output.headers = headers;

    return output;
}

COMP__accountling_header_table COMP__create_null__accountling_header_table() {
    return COMP__create__accountling_header_table(ANVIL__create_null__list());
}

// append accountling header
void COMP__append__accountling_header(ANVIL__list* list, COMP__accountling_header data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_header), memory_error_occured);

    // append data
    (*(COMP__accountling_header*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_header);

    return;
}

// check if headers are equal
ANVIL__bt COMP__check__accountling_headers_are_equal(COMP__accountling_header a, COMP__accountling_header b) {
    return (ANVIL__bt)(COMP__check__name_data_is_identical(a.name, b.name) && (a.input_count == b.input_count) && (a.output_count == b.output_count));
}

// check if a header already exists
ANVIL__bt COMP__check__accounting_header_already_exists(COMP__accountling_header_table* header_table, COMP__accountling_header header) {
    COMP__current current;

    // setup current
    current = COMP__calculate__current_from_list_filled_index(&((*header_table).headers));

    // go over each header
    while (COMP__check__current_within_range(current)) {
        // get header
        COMP__accountling_header temp_header = *(COMP__accountling_header*)current.start;

        // check header
        if (COMP__check__accountling_headers_are_equal(temp_header, header)) {
            // header exists
            return ANVIL__bt__true;
        }

        // next header
        current.start += sizeof(COMP__accountling_header);
    }

    // not found
    return ANVIL__bt__false;
}

// get the header table
COMP__accountling_header_table COMP__account__header_table(ANVIL__list* parsling_programs, COMP__error* error) {
    COMP__accountling_header_table output;
    COMP__current current_file;
    COMP__file_number current_file_number = 0;
    ANVIL__bt memory_error_occured = ANVIL__bt__false;

    // setup output
    output.headers = ANVIL__open__list(sizeof(COMP__accountling_header) * 32, &memory_error_occured);

    // setup current
    current_file = COMP__calculate__current_from_list_filled_index(parsling_programs);

    // get all headers from all programs
    while (COMP__check__current_within_range(current_file)) {
        // setup current abstraction
        COMP__current current_abstraction = COMP__calculate__current_from_list_filled_index(&((*(COMP__parsling_program*)current_file.start).abstractions));

        // get all headers from one file
        while (COMP__check__current_within_range(current_abstraction)) {
            // get current abstraction
            COMP__parsling_abstraction abstraction = *(COMP__parsling_abstraction*)current_abstraction.start;

            // get found header
            COMP__accountling_header header = COMP__create__accountling_header(abstraction.name, abstraction.inputs.filled_index / sizeof(COMP__name), abstraction.outputs.filled_index / sizeof(COMP__name));

            // check if header already exists
            if (COMP__check__accounting_header_already_exists(&output, header)) {
                // error, duplicate abstraction
                *error = COMP__create__error(ANVIL__bt__true, ANVIL__open__buffer_from_string((u8*)"Accounting Error. Header already exists.", ANVIL__bt__true, ANVIL__bt__false), current_file_number, header.name.lexling.line_number, header.name.lexling.character_index);

                // quit
                return output;
            }

            // append header
            COMP__append__accountling_header(&output.headers, header, &memory_error_occured);

            // next abstraction
            current_abstraction.start += sizeof(COMP__parsling_abstraction);
        }

        // next file
        current_file.start += sizeof(COMP__parsling_program);
        current_file_number++;
    }

    // unfinished
    return output;
}

// print the header table
void COMP__print__accountling_header_table(COMP__accountling_header_table header_table) {
    COMP__current current = COMP__calculate__current_from_list_filled_index(&(header_table.headers));

    // print initial message
    printf("Header Table:\n");

    // print each header
    while (COMP__check__current_within_range(current)) {
        // get header
        COMP__accountling_header header = *(COMP__accountling_header*)current.start;

        // print header
        printf("\t");
        ANVIL__print__buffer(header.name.lexling.value);
        printf("(%lu)(%lu)\n", header.input_count, header.output_count);

        // advance current
        current.start += sizeof(COMP__accountling_header);
    }

    return;
}

// accountling variable type
typedef enum COMP__avt {
    COMP__avt__invalid,
    COMP__avt__input,
    COMP__avt__output,
    COMP__avt__body,

    // COUNT
    COMP__avt__COUNT,
} COMP__avt;

// accountling variable
typedef struct COMP__accountling_variable {
    COMP__avt type;
    COMP__variable_index variable_index;
} COMP__accountling_variable;

// accountling statement type
typedef enum COMP__ast {
    COMP__ast__invalid,
    COMP__ast__call,
    COMP__ast__offset,

    // COUNT
    COMP__ast__COUNT,
} COMP__ast;

// accountling statement frame
typedef struct COMP__accountling_statement {
    COMP__ast type;
    COMP__call_index call_index;
    COMP__offset_index offset_index;
} COMP__accountling_statement;

// accountling abstraction type
typedef enum COMP__aat {
    COMP__aat__invalid,
    COMP__aat__predefined,
    COMP__aat__abstraction_call,

    // COUNT
    COMP__aat__COUNT,
} COMP__aat;

// accountling abstraction
typedef struct COMP__accountling_abstraction {
    COMP__aat type;
    ANVIL__list variables;
    ANVIL__list statement_map;
    ANVIL__list calls;
    ANVIL__list offsets;
} COMP__accountling_abstraction;

// accountling skeleton
typedef struct COMP__accountling_skeleton {
    COMP__accountling_header_table header_table;
    ANVIL__list abstractions;
} COMP__accountling_skeleton;

// create custom skeleton
COMP__accountling_skeleton COMP__create__accountling_skeleton(COMP__accountling_header_table header_table, ANVIL__list abstractions) {
    COMP__accountling_skeleton output;

    // setup output
    output.header_table = header_table;
    output.abstractions = abstractions;

    return output;
}

// create null skeleton
COMP__accountling_skeleton COMP__create_null__accountling_skeleton() {
    return COMP__create__accountling_skeleton(COMP__create_null__accountling_header_table(), ANVIL__create_null__list());
}

// account parslings
COMP__accountling_skeleton COMP__account__parsling_programs(ANVIL__list* parsling_buffers, COMP__error* error) {
    COMP__accountling_skeleton output;

    // null init output
    output = COMP__create_null__accountling_skeleton();

    // get header table
    output.header_table = COMP__account__header_table(parsling_buffers, error);

    // DEBUG
    COMP__print__accountling_header_table(output.header_table);

    return output;
}

/* Compile */
// one compiled object across multiple stages
typedef struct COMP__compilation_unit {
    ANVIL__list user_codes;
    ANVIL__list lexling_buffers;
    ANVIL__list parsling_buffers;
    COMP__accountling_skeleton accountling_skeleton;
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

    return;
}

// compile a program
void COMP__compile__files(ANVIL__list user_codes, ANVIL__bt print_debug, COMP__error* error) {
    COMP__compilation_unit compilation_unit;
    COMP__current current;
    COMP__file_number current_file_number = 0;
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
        COMP__lexlings lexlings = COMP__compile__lex(user_code, current_file_number, error);

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
        COMP__parsling_program parslings = COMP__parse__program(lexlings, current_file_number, error);

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
        current_file_number++;
    }

    // account
    compilation_unit.accountling_skeleton = COMP__account__parsling_programs(&compilation_unit.parsling_buffers, error);

    // quit label
    quit:

    // clean up
    COMP__close__compilation_unit(compilation_unit);

    return;
}

#endif
