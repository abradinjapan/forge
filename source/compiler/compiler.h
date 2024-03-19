#ifndef FROST__compiler
#define FROST__compiler

/* Include */
// anvil / basic
#include "../anvil/anvil.h"

/* Define */
// aliases
typedef ANVIL__buffer COMP__current;
typedef ANVIL__buffer COMP__name;

// error types
typedef ANVIL__u64 COMP__character_index;
typedef ANVIL__u64 COMP__line_number;
typedef ANVIL__u64 COMP__file_index;

// lexing types
typedef ANVIL__u8 COMP__lexling_type;
typedef ANVIL__address COMP__lexling_address;
typedef COMP__lexling_address COMP__lexling_start;
typedef COMP__lexling_address COMP__lexling_end;
typedef ANVIL__u64 COMP__lexling_index;

// accounting types
typedef ANVIL__u64 COMP__header_input_count;
typedef ANVIL__u64 COMP__header_output_count;

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

/* Error */
// compiler error type
typedef enum COMP__cet {
    // no error
    COMP__cet__no_error,

    // loading
    COMP__cet__loading__file_not_found,

    // lexing
    COMP__cet__lexling__unrecognized_character,
    COMP__cet__lexling__invalid_string_literal,
} COMP__cet;

// a compilation error
typedef struct COMP__compilation_error {
    COMP__file_index file_index;
    COMP__character_index character_index;
    COMP__cet error_code;
} COMP__compilation_error;

// create a compilation error
COMP__compilation_error COMP__create__compilation_error(COMP__file_index file_index, COMP__character_index character_index, COMP__cet error_code) {
    COMP__compilation_error output;

    // setup output
    output.file_index = file_index;
    output.character_index = character_index;
    output.error_code = error_code;

    return output;
}

// create a null compilation error
COMP__compilation_error COMP__create_null__compilation_error() {
    // return empty
    return COMP__create__compilation_error(0, 0, COMP__cet__no_error);
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
} COMP__lexling;

// create custom lexling
COMP__lexling COMP__create__lexling(COMP__lexling_type type, ANVIL__buffer value) {
    COMP__lexling output;

    // setup output
    output.type = type;
    output.value = value;

    return output;
}

// create null lexling
COMP__lexling COMP__create_null__lexling() {
    // return empty
    return COMP__create__lexling(0, ANVIL__create_null__buffer());
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
    (*(COMP__lexling*)ANVIL__calculate__list_current(&((*lexlings).data))) = lexling;

    // increase fill
    (*lexlings).data.filled_index += sizeof(COMP__lexling);

    return;
}

// append a lexlings to the list
void COMP__append__lexlings(ANVIL__list* list, COMP__lexlings lexlings, ANVIL__bt* error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__lexlings), error_occured);

    // append data
    (*(COMP__lexlings*)ANVIL__calculate__list_current(list)) = lexlings;

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

// lex a program
COMP__lexlings COMP__compile__lex(ANVIL__buffer user_code, ANVIL__bt* lexing_error_occured, ANVIL__bt* memory_error_occured) {
    COMP__lexlings output;
    COMP__current current;
    COMP__lexling_start temp_start;
    COMP__lexling_end temp_end;

    // setup output
    output.data = ANVIL__open__list(sizeof(COMP__lexling) * 64, memory_error_occured);

    // check for error
    if (*memory_error_occured == ANVIL__bt__true) {
        // return empty
        return COMP__create_null__lexlings();
    }

    // setup current
    current = user_code;

    // lex program
    while (current.start < user_code.end) {
        // skip whitespace
        if (COMP__calculate__valid_character_range(current, 0, 32)) {
            // next character
            current.start += sizeof(ANVIL__character);

            // try next character
            continue;
        }

        // check for lexlings
        if (COMP__calculate__valid_character_range(current, '(', '(')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_parenthesis, ANVIL__create__buffer(current.start, current.start)), memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, ')', ')')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_parenthesis, ANVIL__create__buffer(current.start, current.start)), memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '{', '{')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__left_curly_bracket, ANVIL__create__buffer(current.start, current.start)), memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '}', '}')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__right_curly_bracket, ANVIL__create__buffer(current.start, current.start)), memory_error_occured);

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
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__name, ANVIL__create__buffer(temp_start, temp_end)), memory_error_occured);
        } else if (COMP__calculate__valid_character_range(current, '@', '@')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__at, ANVIL__create__buffer(current.start, current.start)), memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        } else if (COMP__calculate__valid_character_range(current, '=', '=')) {
            // add lexling
            COMP__append__lexling(&output, COMP__create__lexling(COMP__lt__equals, ANVIL__create__buffer(current.start, current.start)), memory_error_occured);

            // next character
            current.start += sizeof(ANVIL__character);
        // no lexling found
        } else {
            // set error
            *lexing_error_occured = ANVIL__bt__true;

            // tell error
            printf("Lexical Error: Invalid character '%c'\n", *(ANVIL__character*)current.start);

            // quit
            return output;
        }

        // check for error
        if (*memory_error_occured == ANVIL__bt__true) {
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
        printf("\t%lu [ %lu, %lu ] : ", (ANVIL__u64)temp.type, (ANVIL__u64)temp.value.start, (ANVIL__u64)temp.value.end);
        fflush(stdout);

        // print lexling string
        ANVIL__print__buffer(temp.value);

        // print new line
        printf("\n");
    }

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
    return COMP__create__parsling_statement(COMP__st__COUNT, ANVIL__create_null__buffer(), ANVIL__create_null__list(), ANVIL__create_null__list());
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
    return COMP__create__parsling_abstraction(ANVIL__create_null__buffer(), ANVIL__create_null__list(), ANVIL__create_null__list(), ANVIL__create_null__list());
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
    (*(COMP__parsling_statement*)ANVIL__calculate__list_current(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_statement);

    return;
}

// append parsling abstraction
void COMP__append__parsling_abstraction(ANVIL__list* list, COMP__parsling_abstraction data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_abstraction), memory_error_occured);

    // append data
    (*(COMP__parsling_abstraction*)ANVIL__calculate__list_current(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__parsling_abstraction);

    return;
}

// append parsling program
void COMP__append__parsling_program(ANVIL__list* list, COMP__parsling_program data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__parsling_program), memory_error_occured);

    // append data
    (*(COMP__parsling_program*)ANVIL__calculate__list_current(list)) = data;

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

// advance the current buffer by N number of lexlings
void COMP__advance__lexling_current(COMP__current* current, COMP__lexling_index lexling_count) {
    // advance the pointer
    (*current).start += sizeof(COMP__lexling) * lexling_count;

    return;
}

// parse arguments
ANVIL__list COMP__parse__arguments(COMP__current* current, ANVIL__bt* parse_error_occured, ANVIL__bt* memory_error_occured) {
    ANVIL__list output;

    // open names list
    output = ANVIL__open__list(sizeof(ANVIL__buffer) * 8, memory_error_occured);

    // check for opening parenthesis
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__left_parenthesis) {
        // next lexling
        COMP__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *parse_error_occured = ANVIL__bt__true;

        // tell error
        printf("Parse Error: Arguments is missing opening parethesis.\n");

        return output;
    }

    // get arguments
    while (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
        // get argument
        ANVIL__list__append__buffer(&output, COMP__read__lexling_from_current(*current).value, memory_error_occured);

        // next current
        COMP__advance__lexling_current(current, 1);

        // check memory error
        if (*memory_error_occured == ANVIL__bt__true) {
            // quit
            return output;
        }
    }

    // check for closing parenthesis
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__right_parenthesis) {
        // next lexling
        COMP__advance__lexling_current(current, 1);
    // not found, error
    } else {
        // set error
        *parse_error_occured = ANVIL__bt__true;

        // tell error
        printf("Parse Error: Arguments is missing closing parethesis.\n");

        return output;
    }

    return output;
}

// parse a statement
ANVIL__list COMP__parse__scope(COMP__current* current, ANVIL__bt* parse_error_occured, ANVIL__bt* memory_error_occured) {
    ANVIL__list output;
    COMP__parsling_statement temp;

    // create list
    output = ANVIL__open__list(sizeof(COMP__parsling_statement), memory_error_occured);

    // check for scope opener
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__left_curly_bracket) {
        // advance current
        COMP__advance__lexling_current(current, 1);
    // scope opener not found, error
    } else {
        // setup error
        *parse_error_occured = ANVIL__bt__true;

        // tell error
        printf("Parse Error: Scope is missing left curly bracket.\n");
        
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
                temp.name = COMP__read__lexling_from_current(*current).value;

                // advance current
                COMP__advance__lexling_current(current, 1);
            // invalid syntax
            } else {
                // set error
                *parse_error_occured = ANVIL__bt__true;

                // tell error
                printf("Parse Error: Offset statement name is an invalid lexling.\n");

                return output;
            }

            // null initialize unused data
            temp.inputs = ANVIL__create_null__list();
            temp.outputs = ANVIL__create_null__list();

            // add statement
            COMP__append__parsling_statement(&output, temp, memory_error_occured);
        // is an abstraction call
        } else if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
            // set type
            temp.type = COMP__st__abstraction_call;

            // get name
            temp.name = COMP__read__lexling_from_current(*current).value;

            // advance current
            COMP__advance__lexling_current(current, 1);

            // get inputs
            temp.inputs = COMP__parse__arguments(current, parse_error_occured, memory_error_occured);

            // check for error
            if (*parse_error_occured || *memory_error_occured) {
                // quit
                return output;
            }

            // get outputs
            temp.outputs = COMP__parse__arguments(current, parse_error_occured, memory_error_occured);

            // check for error
            if (*parse_error_occured || *memory_error_occured) {
                // quit
                return output;
            }

            // append statement
            COMP__append__parsling_statement(&output, temp, memory_error_occured);

            // check for error
            if (*parse_error_occured || *memory_error_occured) {
                // quit
                return output;
            }
        // error
        } else {
            // set error
            *parse_error_occured = ANVIL__bt__true;

            // tell error
            printf("Parse Error: Unrecognized statement type.\n");

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
        *parse_error_occured = ANVIL__bt__true;

        // tell error
        printf("Parse Error: Scope is missing right curly bracket.\n");
        
        return output;
    }

    return output;
}

// parse an abstraction
COMP__parsling_abstraction COMP__parse__abstraction(COMP__current* current, ANVIL__bt* parse_error_occured, ANVIL__bt* memory_error_occured) {
    COMP__parsling_abstraction output;

    // check for name
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__name) {
        // get name
        output.name = COMP__read__lexling_from_current(*current).value;

        // next lexling
        COMP__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        (*parse_error_occured) = ANVIL__bt__true;

        // tell error
        printf("Parse Error: An abstraction definition has a name missing.\n");

        // return blank
        return COMP__create_null__parsling_abstraction();
    }

    // get inputs
    output.inputs = COMP__parse__arguments(current, parse_error_occured, memory_error_occured);

    // check for error
    if (*parse_error_occured || *memory_error_occured) {
        // quit
        return output;
    }

    // get outputs
    output.outputs = COMP__parse__arguments(current, parse_error_occured, memory_error_occured);

    // check for equals sign
    if (COMP__check__current_within_range(*current) && COMP__read__lexling_from_current(*current).type == COMP__lt__equals) {
        // no saving data necessary, next lexling
        COMP__advance__lexling_current(current, 1);
    // error
    } else {
        // set error
        (*parse_error_occured) = ANVIL__bt__true;

        // tell error
        printf("Parse Error: An abstraction definition has an equals sign missing.\n");

        // return blank
        return COMP__create_null__parsling_abstraction();
    }

    // check for error
    if (*parse_error_occured || *memory_error_occured) {
        // quit
        return output;
    }

    // parse statements
    output.statements = COMP__parse__scope(current, parse_error_occured, memory_error_occured);

    return output;
}

// parse a file (program)
COMP__parsling_program COMP__parse__program(COMP__lexlings lexlings, ANVIL__bt* parse_error_occured, ANVIL__bt* memory_error_occured) {
    COMP__parsling_program output;
    COMP__parsling_abstraction temp;
    COMP__current current;

    // setup errors
    *parse_error_occured = ANVIL__bt__false;
    *memory_error_occured = ANVIL__bt__false;

    // setup current
    current = COMP__calculate__current_from_list_filled_index(&lexlings.data);

    // open the abstraction list
    output.abstractions = ANVIL__open__list(sizeof(COMP__parsling_abstraction) * 64, memory_error_occured);

    // parse abstractions
    while (COMP__check__current_within_range(current)) {
        // parse abstraction
        temp = COMP__parse__abstraction(&current, parse_error_occured, memory_error_occured);

        // check for error
        if (*parse_error_occured == ANVIL__bt__true || *memory_error_occured == ANVIL__bt__true) {
            return output;
        }

        // append abstraction
        COMP__append__parsling_abstraction(&(output.abstractions), temp, memory_error_occured);
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
        ANVIL__print__buffer(*(ANVIL__buffer*)current.start);

        // next buffer
        current.start += sizeof(ANVIL__buffer);
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
    ANVIL__print__buffer(abstraction.name);

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
            ANVIL__print__buffer(temp_statement.name);
            printf("\n");
        } else if (temp_statement.type == COMP__st__abstraction_call) {
            // print statement
            ANVIL__print__buffer(temp_statement.name);

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

// header table
typedef struct COMP__accountling_header_table {
    ANVIL__list headers;
} COMP__accountling_header_table;

// open header table
COMP__accountling_header_table COMP__open__accountling_header_table(ANVIL__bt* memory_error_occured) {
    COMP__accountling_header_table output;

    // open headers
    output.headers = ANVIL__open__list(sizeof(COMP__accountling_header) * 32, memory_error_occured);

    return output;
}

// append accountling header
void COMP__append__accountling_header(ANVIL__list* list, COMP__accountling_header data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(COMP__accountling_header), memory_error_occured);

    // append data
    (*(COMP__accountling_header*)ANVIL__calculate__list_current(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(COMP__accountling_header);

    return;
}

// get the header table
COMP__accountling_header_table COMP__account__header_table(COMP__parsling_program parslings, ANVIL__bt* memory_error_occured) {
    COMP__accountling_header_table output;

    // setup output
    output = COMP__open__accountling_header_table(memory_error_occured);

    // get all headers
    {
        // setup current abstraction
        COMP__current current = COMP__calculate__current_from_list_filled_index(&parslings.abstractions);

        // get current abstraction
        COMP__parsling_abstraction abstraction = *(COMP__parsling_abstraction*)current.start;

        // get header
        COMP__accountling_header header;
        header.name = abstraction.name;
        header.input_count = abstraction.inputs.filled_index / sizeof(COMP__name);
        header.output_count = abstraction.outputs.filled_index / sizeof(COMP__name);

        // append header
        COMP__append__accountling_header(&output.headers, header, memory_error_occured);

        // check memory error
        if (*memory_error_occured) {
            return output;
        }
    }

    // unfinished
    return output;
}

/* Compile */
// one compiled object across multiple stages
typedef struct COMP__compilation_unit {
    ANVIL__list user_codes;
    ANVIL__list parsling_buffers;
} COMP__compilation_unit;

// close a compilation unit
void COMP__close__compilation_unit(COMP__compilation_unit unit) {
    // close user codes (Assumes that user codes are allocated elsewhere!)
    ANVIL__close__list(unit.user_codes);

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

// lex and parse one file
COMP__parsling_program COMP__dissect__file(ANVIL__buffer user_code, ANVIL__bt print_debug, ANVIL__bt* compilation_error_occured, ANVIL__bt* memory_error_occured) {
    COMP__parsling_program output;
    COMP__lexlings lexlings;

    // inform user of compilation start
    if (print_debug) {
        printf("Compiling:\n");
        ANVIL__print__buffer(user_code);
        printf("\n");
    }

    // lex user code
    lexlings = COMP__compile__lex(user_code, compilation_error_occured, memory_error_occured);

    // check for error
    if (*compilation_error_occured || *memory_error_occured) {
        // clean up
        COMP__close__lexlings(lexlings);

        return output;
    }

    // print lexlings
    if (print_debug) {
        COMP__debug__print_lexlings(lexlings);
    }

    // parse lexlings
    output = COMP__parse__program(lexlings, compilation_error_occured, memory_error_occured);

    // check for error
    if (*compilation_error_occured || *memory_error_occured) {
        // clean up
        COMP__close__lexlings(lexlings);
        
        return output;
    }

    // print parslings
    if (print_debug) {
        COMP__print__parsed_program(output);
    }

    // clean up
    COMP__close__lexlings(lexlings);

    return output;
}

// compile a program
void COMP__compile__files(ANVIL__list user_codes, ANVIL__bt print_debug, ANVIL__bt* compilation_error_occured, ANVIL__bt* memory_error_occured) {
    COMP__compilation_unit compilation_unit;
    COMP__current current;

    // setup error codes
    *compilation_error_occured = ANVIL__bt__false;
    *memory_error_occured = ANVIL__bt__false;

    // setup compilation unit
    compilation_unit.user_codes = ANVIL__open__list(sizeof(ANVIL__buffer) * 5, memory_error_occured);
    compilation_unit.parsling_buffers = ANVIL__open__list(sizeof(COMP__parsling_program) * 5, memory_error_occured);

    // setup current
    current = COMP__calculate__current_from_list_filled_index(&user_codes);

    // lex and parse each file
    while (COMP__check__current_within_range(current)) {
        // get file
        ANVIL__buffer user_code = *(ANVIL__buffer*)current.start;

        // append file
        ANVIL__list__append__buffer(&compilation_unit.user_codes, user_code, memory_error_occured);

        // check for error
        if (*memory_error_occured) {
            break;
        }

        // lex and parse file
        COMP__parsling_program program = COMP__dissect__file(user_code, print_debug, compilation_error_occured, memory_error_occured);

        // check for error
        if (*compilation_error_occured || *memory_error_occured) {
            // quit
            break;
        }

        // append program
        COMP__append__parsling_program(&compilation_unit.parsling_buffers, program, memory_error_occured);

        // check for error
        if (*memory_error_occured) {
            break;
        }

        // advance current
        current.start += sizeof(ANVIL__buffer);
    }

    // check for error
    if (*compilation_error_occured || *memory_error_occured) {
        // clean up
        COMP__close__compilation_unit(compilation_unit);

        // quit
        return;
    }

    // clean up
    COMP__close__compilation_unit(compilation_unit);

    return;
}

#endif
