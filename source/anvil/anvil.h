#ifndef FROST__alloy
#define FROST__alloy

/* Include */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

/* Define */
typedef uint8_t ANVIL__u8;
typedef uint16_t ANVIL__u16;
typedef uint32_t ANVIL__u32;
typedef uint64_t ANVIL__u64;
typedef int8_t ANVIL__s8;
typedef int16_t ANVIL__s16;
typedef int32_t ANVIL__s32;
typedef int64_t ANVIL__s64;

typedef ANVIL__u8 u8;
typedef ANVIL__u16 u16;
typedef ANVIL__u32 u32;
typedef ANVIL__u64 u64;
typedef ANVIL__s8 s8;
typedef ANVIL__s16 s16;
typedef ANVIL__s32 s32;
typedef ANVIL__s64 s64;

typedef void* ANVIL__address;
#define ANVIL__define__null_address 0

typedef ANVIL__u64 ANVIL__length;

typedef ANVIL__u64 ANVIL__cell_count;
typedef ANVIL__cell_count ANVIL__cell_index;

typedef ANVIL__u64 ANVIL__cell_integer_value;

typedef ANVIL__u64 ANVIL__instruction_count;

typedef ANVIL__u8 ANVIL__character;

typedef ANVIL__u64 ANVIL__bit_count;
typedef ANVIL__u64 ANVIL__byte_count;

#define ANVIL__unused_cell_ID 0
#define ANVIL__define__bits_in_byte 8
#define ANVIL__define__run_forever (ANVIL__u64)-1
#define ANVIL__define__input_string_max_length 256

// boolean type
typedef enum ANVIL__bt {
	ANVIL__bt__false = 0,
	ANVIL__bt__true = 1,
	ANVIL__bt__COUNT = 2,
} ANVIL__bt;

/* Allocation */
// return memory to OS
void ANVIL__close__allocation(ANVIL__address start, ANVIL__address end) {
	// do useless operation to quiet compiler
	end = end;

	// return memory
	free(start);

	return;
}

// ask OS for memory
ANVIL__address ANVIL__open__allocation(ANVIL__length length) {
	// return allocation
	return (ANVIL__address)malloc(length);
}

/* Buffer */
// buffer
typedef struct ANVIL__buffer {
	ANVIL__address start;
	ANVIL__address end;
} ANVIL__buffer;

// close buffer
void ANVIL__close__buffer(ANVIL__buffer buffer) {
	// close allocation
	ANVIL__close__allocation(buffer.start, buffer.end);

	return;
}

// create buffer to have specific exact contents
ANVIL__buffer ANVIL__create__buffer(ANVIL__address start, ANVIL__address end) {
	ANVIL__buffer output;

	// setup output
	output.start = start;
	output.end = end;

	return output;
}

// create buffer in it's standard null setup
ANVIL__buffer ANVIL__create_null__buffer() {
	// return standard null buffer
	return ANVIL__create__buffer(ANVIL__define__null_address, ANVIL__define__null_address);
}

// calculate buffer length
ANVIL__length ANVIL__calculate__buffer_length(ANVIL__buffer buffer) {
    return (ANVIL__length)((u8*)buffer.end - (u8*)buffer.start) + 1;
}

// check to see if buffers are equal
ANVIL__bt ANVIL__calculate__are_buffers_exactly_equivalent(ANVIL__buffer a, ANVIL__buffer b) {
    return (ANVIL__bt)((a.start == b.start) && (a.end == b.end));
}

// calculate buffer contains range
ANVIL__bt ANVIL__calculate__buffer_range_in_buffer_range_inclusive(ANVIL__buffer outside, ANVIL__buffer inside) {
    // return calculation
    return (outside.start <= inside.start) && (outside.end >= inside.end);
}

// check to see if it is an empty buffer
ANVIL__bt ANVIL__check__empty_buffer(ANVIL__buffer buffer) {
    // return calculation
    return (ANVIL__bt)(buffer.start == ANVIL__define__null_address);
}

// open buffer
ANVIL__buffer ANVIL__open__buffer(ANVIL__length length) {
	ANVIL__buffer output;

	// attempt allocation
	output.start = ANVIL__open__allocation(length);

	// set end of buffer according to allocation success
	if (output.start != ANVIL__define__null_address) {
		output.end = (ANVIL__address)((((ANVIL__u64)output.start) + length) - 1);
	} else {
		output.end = ANVIL__define__null_address;
	}

	return output;
}

// check if buffers are the same size
ANVIL__bt ANVIL__calculate__are_buffers_same_size(ANVIL__buffer a, ANVIL__buffer b) {
    // return calculation
    return (ANVIL__bt)((a.end - a.start) == (b.end - b.start));
}

// copy buffer
void ANVIL__copy__buffer(ANVIL__buffer source, ANVIL__buffer destination, ANVIL__bt* error) {
    // check for invalid buffer
    if (ANVIL__calculate__are_buffers_same_size(source, destination) == ANVIL__bt__true) {
        // copy buffer
        for (ANVIL__length byte_index = 0; byte_index < (ANVIL__length)(destination.end - destination.start) + 1; byte_index++) {
            // copy byte
            ((ANVIL__u8*)destination.start)[byte_index] = ((ANVIL__u8*)source.start)[byte_index];
        }
    } else {
        // set error
        *error = ANVIL__bt__true;
    }

    return;
}

// create or open a buffer from a string literal (can either duplicate buffer or simply reference original) (can opt out of null termination)
ANVIL__buffer ANVIL__open__buffer_from_string(u8* string, ANVIL__bt duplicate, ANVIL__bt null_terminate) {
    ANVIL__buffer output;
    ANVIL__length length;

    // setup length
    length = 0;

    // get buffer length
    while (string[length] != 0) {
        length++;
    }

    // optionally append null termination
    if (null_terminate == ANVIL__bt__true) {
        length++;
    }

    // reference or duplicate
    if (duplicate == ANVIL__bt__true) {
        // attempt allocation
        output = ANVIL__open__buffer(length);

        // check for null allocation
        if (output.start == ANVIL__define__null_address) {
            // return empty buffer
            return output;
        }

        // copy buffer byte by byte
        for (ANVIL__length byte_index = 0; byte_index < length; byte_index++) {
            // copy byte
            ((ANVIL__u8*)output.start)[byte_index] = string[byte_index];
        }
    } else {
        // setup duplicate output
        output.start = string;
        output.end = string + length - 1;
    }

    return output;
}

// read buffer
ANVIL__u64 ANVIL__read__buffer(ANVIL__address source, ANVIL__length byte_amount) {
	ANVIL__u64 output;

	// setup output
	output = 0;

	// read buffer
	for (ANVIL__u64 byte_index = 0; byte_index < byte_amount; byte_index += 1) {
		// get byte
		((ANVIL__u8*)&output)[byte_index] = ((ANVIL__u8*)source)[byte_index];
	}

	// return output
	return output;
}

// write buffer
void ANVIL__write__buffer(ANVIL__u64 source, ANVIL__length byte_amount, ANVIL__address destination) {
	// write data to buffer
	for (ANVIL__length byte_index = 0; byte_index < byte_amount; byte_index += 1) {
		// write byte
		((ANVIL__u8*)destination)[byte_index] = ((ANVIL__u8*)&source)[byte_index];
	}
	
	return;
}

// create buffer from file
ANVIL__buffer ANVIL__move__file_to_buffer(ANVIL__buffer null_terminated_file_name) {
	ANVIL__buffer output;
	FILE* file_handle;
	ANVIL__u64 file_size;

	// open file
	file_handle = fopen((const char*)null_terminated_file_name.start, "rb");

	// check if the file opened
	if (file_handle == 0) {
		// if not, return empty buffer
		return ANVIL__create_null__buffer();
	}

	// get file size
	fseek(file_handle, 0, SEEK_END);
	file_size = ftell(file_handle);
	fseek(file_handle, 0, SEEK_SET);

	// allocate buffer
	output = ANVIL__open__buffer(file_size);

	// check if buffer allocated
	if (output.start == ANVIL__define__null_address) {
		// close file handle
		fclose(file_handle);

		// return empty buffer
		return output;
	}

	// read file into buffer
	fread(output.start, file_size, 1, file_handle);

	// close file handle
	fclose(file_handle);

	// return buffer
	return output;
}

// create file from buffer
void ANVIL__move__buffer_to_file(ANVIL__bt* error, ANVIL__buffer null_terminated_file_name, ANVIL__buffer data) {
	FILE* file_handle;

    // setup error to no error to start
    *error = ANVIL__bt__false;

	// open file
	file_handle = fopen((const char*)null_terminated_file_name.start, "w+b");

	// check if the file opened
	if (file_handle == 0) {
		// if not, return error
        *error = ANVIL__bt__true;

		return;
	}

	// write buffer to file
	fwrite(data.start, ANVIL__calculate__buffer_length(data), 1, file_handle);

	// close file handle
	fclose(file_handle);

	// return
	return;
}

// print buffer
void ANVIL__print__buffer(ANVIL__buffer buffer) {
    // print character by character
    for (ANVIL__address character = buffer.start; character <= buffer.end; character += sizeof(ANVIL__character)) {
        // print character
        putchar(*(ANVIL__character*)character);
    }

    return;
}

/* List */
// list types
typedef ANVIL__u64 ANVIL__list_filled_index;
typedef ANVIL__u64 ANVIL__list_increase;

// list object
typedef struct ANVIL__list {
    ANVIL__buffer buffer;
    ANVIL__list_filled_index filled_index;
    ANVIL__list_increase increase;
} ANVIL__list;

// create a list
ANVIL__list ANVIL__create__list(ANVIL__buffer buffer, ANVIL__list_filled_index filled_index, ANVIL__list_increase increase) {
    ANVIL__list output;

    // setup output
    output.buffer = buffer;
    output.filled_index = filled_index;
    output.increase = increase;

    return output;
}

// create a null list
ANVIL__list ANVIL__create_null__list() {
    // return empty list
    return ANVIL__create__list(ANVIL__create_null__buffer(), 0, 0);
}

// open a list
ANVIL__list ANVIL__open__list(ANVIL__list_increase increase, ANVIL__bt* error_occured) {
    ANVIL__list output;
    ANVIL__buffer allocation;

    // allocate list
    allocation = ANVIL__open__buffer(increase);

    // check list validity
    if (allocation.start == ANVIL__define__null_address) {
        // set error
        *error_occured = ANVIL__bt__true;

        // return empty
        return ANVIL__create_null__list();
    // list is valid
    } else {
        // set error to false
        *error_occured = ANVIL__bt__false;
    }

    // setup output
    output.buffer = allocation;
    output.filled_index = 0;
    output.increase = increase;

    return output;
}

// destroy a list
void ANVIL__close__list(ANVIL__list list) {
    // free buffer
    ANVIL__close__buffer(ANVIL__create__buffer(list.buffer.start, list.buffer.end));

    return;
}

// expand a list
void ANVIL__list__expand(ANVIL__list* list, ANVIL__bt* error_occured) {
    ANVIL__list_filled_index new_size;
    ANVIL__buffer new_allocation;

    // calculate new buffer size
    new_size = ((ANVIL__u64)(*list).buffer.end - (ANVIL__u64)(*list).buffer.start + 1) + (*list).increase;

    // request new memory
    new_allocation = ANVIL__open__buffer(new_size);

    // check for failure
    if (new_allocation.start == ANVIL__define__null_address) {
        // set error
        *error_occured = ANVIL__bt__true;

        // return unmodified list
        return;
    }

    // copy old data to new list
    for (ANVIL__list_filled_index i = 0; i < (*list).filled_index; i++) {
        // copy one byte
        ((ANVIL__u8*)new_allocation.start)[i] = ((ANVIL__u8*)(*list).buffer.start)[i];
    }

    // free old buffer
    ANVIL__close__buffer((*list).buffer);

    // setup new list allocation
    (*list).buffer = new_allocation;

    return;
}

// request space for the list
void ANVIL__list__request__space(ANVIL__list* list, ANVIL__byte_count byte_count, ANVIL__bt* error_occured) {
    // expand the list until there is enough space
    while (((ANVIL__u64)(*list).buffer.end - (ANVIL__u64)(*list).buffer.start + 1) < ((*list).filled_index + byte_count)) {
        // expand the list
        ANVIL__list__expand(list, error_occured);

        // check for error
        if (*error_occured == ANVIL__bt__true) {
            // return last modified list
            return;
        }
    }

    return;
}

// add index to address
ANVIL__address ANVIL__calculate__address_from_buffer_index(ANVIL__address start, ANVIL__list_filled_index index) {
    return start + index;
}

// calculate the tip of the list
ANVIL__address ANVIL__calculate__list_current_address(ANVIL__list* list) {
    return ANVIL__calculate__address_from_buffer_index((*list).buffer.start, (*list).filled_index);
}

// calculate the current buffer
ANVIL__buffer ANVIL__calculate__list_current_buffer(ANVIL__list* list) {
    return ANVIL__create__buffer(((*list).buffer.start), ANVIL__calculate__list_current_address(list) - 1);
}

// add a buffer to a list
void ANVIL__list__append__buffer(ANVIL__list* list, ANVIL__buffer buffer, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(ANVIL__buffer), memory_error_occured);

    // append data
    (*(ANVIL__buffer*)ANVIL__calculate__list_current_address(list)) = buffer;

    // increase fill
    (*list).filled_index += sizeof(ANVIL__buffer);

    return;
}

// add a buffer's data to a list
void ANVIL__list__append__buffer_data(ANVIL__list* list, ANVIL__buffer buffer, ANVIL__bt* memory_error_occured) {
    ANVIL__length buffer_length;
    ANVIL__address buffer_old_end;

    // calculate buffer length
    buffer_length = ANVIL__calculate__buffer_length(buffer);

    // calculate old buffer end
    buffer_old_end = (*list).buffer.start + (*list).filled_index - 1;

    // request space
    ANVIL__list__request__space(list, buffer_length, memory_error_occured);

    // append data
    ANVIL__copy__buffer(buffer, ANVIL__create__buffer(buffer_old_end + 1, buffer_old_end + buffer_length), memory_error_occured);

    // increase fill
    (*list).filled_index += buffer_length;

    return;
}

// add a list to a list
void ANVIL__list__append__list(ANVIL__list* list, ANVIL__list data, ANVIL__bt* memory_error_occured) {
    // request space
    ANVIL__list__request__space(list, sizeof(ANVIL__list), memory_error_occured);

    // append data
    (*(ANVIL__list*)ANVIL__calculate__list_current_address(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(ANVIL__list);

    return;
}

// remove a slice of data from a list
void ANVIL__list__erase__space(ANVIL__list* list, ANVIL__list_filled_index range_start_index, ANVIL__list_filled_index range_end_index) {
    ANVIL__buffer old_right;
    ANVIL__buffer new_right;
    ANVIL__bt error;

    // get new right buffer
    old_right = ANVIL__create__buffer(ANVIL__calculate__address_from_buffer_index((*list).buffer.start, range_end_index), ANVIL__calculate__list_current_address(list));
    new_right = ANVIL__create__buffer(old_right.start - (range_end_index - range_start_index), old_right.end - (range_end_index - range_start_index));

    // move data from left to right filling in the gap
    ANVIL__copy__buffer(old_right, new_right, &error);

    // should not happen but handled anyways
    if (error == ANVIL__bt__true) {
        // tell user
        printf("Internal Error: Buffer could not erase data.\n");
    // buffer was clipped, change filled index
    } else {
        // change current
        (*list).filled_index -= range_end_index - range_start_index;
    }

    return;
}

// check if two lists are filled up to the same amount
ANVIL__bt ANVIL__calculate__lists_have_same_fill_size(ANVIL__list* a, ANVIL__list* b) {
    return (ANVIL__bt)((*a).filled_index == (*b).filled_index);
}

// take a list and make a standalone buffer
ANVIL__buffer ANVIL__list__create_buffer_from_list(ANVIL__list* list, ANVIL__bt* memory_error_occured) {
    ANVIL__buffer output;

    // allocate output
    output = ANVIL__open__buffer((*list).filled_index);

    // if buffer did not open
    if (ANVIL__check__empty_buffer(output) == ANVIL__bt__true) {
        // set error
        *memory_error_occured = ANVIL__bt__true;
    // if buffer opened
    } else {
        // copy data from list to buffer
        ANVIL__copy__buffer(ANVIL__calculate__list_current_buffer(list), output, memory_error_occured);
    }

    return output;
}

/* Machine Specifications */
// instruction parts
typedef ANVIL__address ANVIL__cell;
typedef ANVIL__u8 ANVIL__instruction_ID;
typedef ANVIL__u8 ANVIL__flag_ID;
typedef ANVIL__u8 ANVIL__operation_ID;
typedef ANVIL__u16 ANVIL__cell_ID;

// scraplet types
typedef enum ANVIL__st {
    // scraplet types
    ANVIL__st__instruction_ID,
    ANVIL__st__flag_ID,
    ANVIL__st__operation_ID,
    ANVIL__st__cell_ID,
    ANVIL__st__cell,

    // count
    ANVIL__st__COUNT,
} ANVIL__st;

// should the next instruction process
// next instruction type
typedef enum ANVIL__nit {
    // next instruction types
    ANVIL__nit__return_context,
    ANVIL__nit__next_instruction,

    // count
    ANVIL__nit__COUNT,
} ANVIL__nit;

/* Allocations */
// allocations
typedef struct ANVIL__allocations {
    ANVIL__list buffers;
} ANVIL__allocations;

// find an allocation
ANVIL__list_filled_index ANVIL__find__allocation(ANVIL__allocations* allocations, ANVIL__buffer allocation, ANVIL__bt* found) {
    ANVIL__list_filled_index output = 0;
    ANVIL__buffer current;
    
    // setup current
    current = ANVIL__calculate__list_current_buffer(&((*allocations).buffers));

    // check for valid allocation
    while (current.start <= current.end) {
        // check one allocation
        if (ANVIL__calculate__are_buffers_exactly_equivalent(*(ANVIL__buffer*)current.start, allocation)) {
            // allocation is valid
            *found = ANVIL__bt__true;

            return output;
        }

        // next allocation
        current.start += sizeof(ANVIL__buffer);
        output += sizeof(ANVIL__buffer);
    }

    // not valid
    *found = ANVIL__bt__false;

    return output;
}

// add a buffer
void ANVIL__remember__allocation(ANVIL__allocations* allocations, ANVIL__buffer allocation, ANVIL__bt* list_error_occured) {
    // append allocation
    ANVIL__list__append__buffer(&((*allocations).buffers), allocation, list_error_occured);

    return;
}

// remove a buffer
void ANVIL__forget__allocation(ANVIL__allocations* allocations, ANVIL__buffer allocation, ANVIL__bt* buffer_did_not_exist) {
    ANVIL__bt found = ANVIL__bt__false;

    // find allocation index
    ANVIL__list_filled_index start_index = ANVIL__find__allocation(allocations, allocation, &found);

    // if found
    if (found == ANVIL__bt__true) {
        // erase from list
        ANVIL__list__erase__space(&((*allocations).buffers), start_index, start_index + sizeof(ANVIL__buffer) - 1);
    // not found, so not removed
    } else {
        // set error
        *buffer_did_not_exist = ANVIL__bt__true;
    }

    return;
}

// check to see if an address is valid
ANVIL__bt ANVIL__check__valid_address_range_in_allocations(ANVIL__allocations* allocations, ANVIL__buffer range) {
    ANVIL__buffer current;
    
    // setup current
    current = ANVIL__create__buffer((*allocations).buffers.buffer.start, ANVIL__calculate__list_current_address(&((*allocations).buffers)));

    // check for valid allocation range
    while (current.start <= current.end) {
        // check one allocation
        if (ANVIL__calculate__buffer_range_in_buffer_range_inclusive(*(ANVIL__buffer*)current.start, range)) {
            // allocation is valid
            return ANVIL__bt__true;
        }
        
        // next allocation
        current.start += sizeof(ANVIL__buffer);
    }

    return ANVIL__bt__false;
}

// open allocations
ANVIL__allocations ANVIL__open__allocations(ANVIL__bt* memory_error_occured) {
    ANVIL__allocations output;

    // setup output
    output.buffers = ANVIL__open__list(sizeof(ANVIL__buffer) * 256, memory_error_occured);

    return output;
}

// close allocations (does NOT clear actual allocations)
void ANVIL__close__allocations(ANVIL__allocations* allocations) {
    // clean up
    ANVIL__close__list((*allocations).buffers);

    return;
}

/* Alloy Specification */
// context cell organization defines
// cell type
typedef enum ANVIL__rt {
    // start of defined values
    ANVIL__rt__START = 0,

    // basic cells
    ANVIL__rt__program_start_address = ANVIL__rt__START,
    ANVIL__rt__program_current_address,
    ANVIL__rt__program_end_address,
    ANVIL__rt__error_code,
    ANVIL__rt__flags_0,
    ANVIL__rt__flags_1,
    ANVIL__rt__flags_2,
    ANVIL__rt__flags_3,

    // end of defined cells
    ANVIL__rt__END,

    // count
    ANVIL__rt__RESERVED_COUNT = ANVIL__rt__END - ANVIL__rt__START,

    // statistics
    ANVIL__rt__FIRST_ID = ANVIL__rt__START,
    ANVIL__rt__LAST_ID = 65535, // NUMBER DOES NOT CHANGE!
    ANVIL__rt__TOTAL_COUNT = ANVIL__rt__LAST_ID + 1,
} ANVIL__rt;

// context
typedef struct ANVIL__context {
    ANVIL__cell cells[ANVIL__rt__TOTAL_COUNT];
} ANVIL__context;

// instruction type and instruction ID defines (type number is also ID)
typedef enum ANVIL__it {
    // start of defined instructions
    ANVIL__it__START = 0,

    // defined instructions
    ANVIL__it__stop = ANVIL__it__START, // returns context to caller
    ANVIL__it__write_cell, // overwrites entire cell with hard coded value
    ANVIL__it__operate, // inter-cell operations
    ANVIL__it__request_memory, // allocate request
    ANVIL__it__return_memory, // deallocate request
    ANVIL__it__address_to_cell, // read memory into cell
    ANVIL__it__cell_to_address, // write cell to memory
    ANVIL__it__file_to_buffer, // get file into buffer
    ANVIL__it__buffer_to_file, // write buffer to disk
    ANVIL__it__run, // run context (both until finished and run one instruction)

    // extra defined instructions
    ANVIL__it__debug__putchar, // print one character to stdout
    ANVIL__it__debug__print_cell_as_decimal, // print an entire cell as a decimal number
    ANVIL__it__debug__fgets, // read one line from stdin
    ANVIL__it__debug__mark_data_section, // mark a section of data (NOP)
    ANVIL__it__debug__mark_code_section, // mark a section of code (NOP)

    // end of defined instruction types
    ANVIL__it__END,

    // count
    ANVIL__it__COUNT = ANVIL__it__END - ANVIL__it__START,
} ANVIL__it;

// instruction length types
typedef enum ANVIL__ilt {
    ANVIL__ilt__stop = sizeof(ANVIL__instruction_ID),
    ANVIL__ilt__write_cell = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell) + sizeof(ANVIL__cell_ID),
    ANVIL__ilt__operate = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__flag_ID) + sizeof(ANVIL__operation_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__request_memory = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__return_memory = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__address_to_cell = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__flag_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__cell_to_address = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__flag_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__file_to_buffer = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__buffer_to_file = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 4),
    ANVIL__ilt__run = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 3),
    ANVIL__ilt__debug__putchar = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell_ID),
    ANVIL__ilt__debug__print_cell_as_decimal = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell_ID),
    ANVIL__ilt__debug__fgets = sizeof(ANVIL__instruction_ID) + (sizeof(ANVIL__cell_ID) * 2),
    ANVIL__ilt__debug__mark_data_section = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell),
    ANVIL__ilt__debug__mark_code_section = sizeof(ANVIL__instruction_ID) + sizeof(ANVIL__cell),
} ANVIL__ilt;

// error codes
typedef enum ANVIL__et {
    // error codes
    ANVIL__et__no_error = 0,
    ANVIL__et__invalid_instruction_ID,
    ANVIL__et__divide_by_zero,
    ANVIL__et__modulous_by_zero,
    ANVIL__et__file_not_found,
    ANVIL__et__file_not_created,
    ANVIL__et__invalid_address__address_to_cell,
    ANVIL__et__invalid_address__cell_to_address,
    ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer,
    ANVIL__et__invalid_allocation__allocation_does_not_exist,
    ANVIL__et__invalid_address_range,

    // count
    ANVIL__et__COUNT,
} ANVIL__et;

// operation types
typedef enum ANVIL__ot {
    // copy
    ANVIL__ot__cell_to_cell, // copies one cell to another without transformation

    // binary operations
    ANVIL__ot__bits_or,
    ANVIL__ot__bits_invert,
    ANVIL__ot__bits_and,
    ANVIL__ot__bits_xor,
    ANVIL__ot__bits_shift_higher,
    ANVIL__ot__bits_shift_lower,
    ANVIL__ot__bits_overwrite,

    // arithmetic operations
    ANVIL__ot__integer_add,
    ANVIL__ot__integer_subtract,
    ANVIL__ot__integer_multiply,
    ANVIL__ot__integer_division,
    ANVIL__ot__integer_modulous,

    // comparison operations
    ANVIL__ot__integer_within_range, // equivalent to (range_start <= integer_n && integer_n <= range_end) -> boolean

    // flag operations
    ANVIL__ot__flag_or,
    ANVIL__ot__flag_invert,
    ANVIL__ot__flag_and,
    ANVIL__ot__flag_xor,
    ANVIL__ot__flag_get,
    ANVIL__ot__flag_set,

    // count
    ANVIL__ot__COUNT,
} ANVIL__ot;

/* Helper Functions */
// get a pointer from a context to a cell inside that context
ANVIL__cell* ANVIL__get__cell_address_from_context(ANVIL__context* context, ANVIL__cell_ID cell_ID) {
    // return data
    return &((*context).cells[cell_ID]);
}

// get cell value from context
ANVIL__cell ANVIL__get__cell_from_context(ANVIL__context* context, ANVIL__cell_ID cell_ID) {
    // return data
    return *ANVIL__get__cell_address_from_context(context, cell_ID);
}

// set cell value by pointer
void ANVIL__set__cell_by_address(ANVIL__cell* destination_cell, ANVIL__cell value) {
    // set value
    *destination_cell = value;

    return;
}

// set value in error code cell
void ANVIL__set__error_code_cell(ANVIL__context* context, ANVIL__et error_code) {
    // set error
    ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, ANVIL__rt__error_code), (ANVIL__cell)error_code);

    return;
}

// get flag (value)
ANVIL__bt ANVIL__get__flag_from_context(ANVIL__context* context, ANVIL__flag_ID flag_ID) {
    ANVIL__u8 masks[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // return bit
    return (ANVIL__bt)((((ANVIL__u8*)&((*context).cells[ANVIL__rt__flags_0]))[flag_ID / 8] & masks[flag_ID % 8]) > 0);
}

// set flag (value)
void ANVIL__set__flag_in_context(ANVIL__context* context, ANVIL__flag_ID flag_ID, ANVIL__bt value) {
    ANVIL__u8 masks[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    // clear bit
    ((((ANVIL__u8*)&((*context).cells[ANVIL__rt__flags_0]))[flag_ID / 8] &= (~masks[flag_ID % 8])));

    // set bit
    ((ANVIL__u8*)&((*context).cells[ANVIL__rt__flags_0]))[flag_ID / 8] |= (value << (flag_ID % 8));

    return;
}

// read item from buffer and advance read address
ANVIL__u64 ANVIL__read_next__buffer_item(ANVIL__cell* address, ANVIL__byte_count byte_count) {
    ANVIL__u64 output;

    // read data from buffer
    output = ANVIL__read__buffer(*address, byte_count);

    // advance pointer
    *address = (ANVIL__address)(*((u8**)address) + byte_count);

    // return data
    return output;
}

// get cell value and advance
ANVIL__cell ANVIL__read_next__cell(ANVIL__cell* address) {
    // read data
    return (ANVIL__cell)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__cell));
}

// get instruction ID and advance
ANVIL__instruction_ID ANVIL__read_next__instruction_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__instruction_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__instruction_ID));
}

// get flag ID and advance
ANVIL__flag_ID ANVIL__read_next__flag_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__flag_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__flag_ID));
}

// get operation ID and advance
ANVIL__operation_ID ANVIL__read_next__operation_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__operation_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__operation_ID));
}

// get cell ID and advance
ANVIL__cell_ID ANVIL__read_next__cell_ID(ANVIL__cell* address) {
    // read data
    return (ANVIL__cell_ID)ANVIL__read_next__buffer_item(address, sizeof(ANVIL__cell_ID));
}

/* Setup Alloy Code */
// create a skeleton context
ANVIL__context ANVIL__setup__context(ANVIL__buffer program) {
    ANVIL__context output;

    // setup program and execution cells
    output.cells[ANVIL__rt__program_start_address] = (ANVIL__cell)program.start;
    output.cells[ANVIL__rt__program_end_address] = (ANVIL__cell)program.end;
    output.cells[ANVIL__rt__program_current_address] = output.cells[ANVIL__rt__program_start_address];

    return output;
}

/* Run Alloy Code */
void ANVIL__run__context(ANVIL__allocations* allocations, ANVIL__context* context, ANVIL__instruction_count instruction_count);

// process operation (assumes flag was checked)
ANVIL__nit ANVIL__run__operation(ANVIL__context* context, ANVIL__ot operation_type, ANVIL__cell_ID input_0, ANVIL__cell_ID input_1, ANVIL__cell_ID input_2, ANVIL__cell_ID output_0) {
    ANVIL__cell_integer_value temp_input_0;
    ANVIL__cell_integer_value temp_input_1;
    ANVIL__cell_integer_value temp_input_2;
    ANVIL__cell_integer_value temp_result;

    // do operation based on type
    switch (operation_type) {
    // cell to cell
    case ANVIL__ot__cell_to_cell:
        // set value
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), ANVIL__get__cell_from_context(context, input_0));

        break;
    // binary or
    case ANVIL__ot__bits_or:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 | temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary invert
    case ANVIL__ot__bits_invert:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);

        // perform operation
        temp_result = ~temp_input_0;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary and
    case ANVIL__ot__bits_and:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 & temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary xor
    case ANVIL__ot__bits_xor:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 ^ temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary bit shift higher
    case ANVIL__ot__bits_shift_higher:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 << temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary bit shift lower
    case ANVIL__ot__bits_shift_lower:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 >> temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // binary overwrite bits
    case ANVIL__ot__bits_overwrite:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // mask (positive bits are the ones being overwritten!)
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // old bits
        temp_input_2 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_2); // new bits

        // perform operation
        temp_result = (~temp_input_0) & temp_input_1;
        temp_result = temp_result | (temp_input_2 & temp_input_0);

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer addition
    case ANVIL__ot__integer_add:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 + temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer subtraction
    case ANVIL__ot__integer_subtract:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 - temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer multiplication
    case ANVIL__ot__integer_multiply:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // perform operation
        temp_result = temp_input_0 * temp_input_1;

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer division
    case ANVIL__ot__integer_division:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // check if division is possible
        if (temp_input_1 != 0) {
            // compute result
            temp_result = temp_input_0 / temp_input_1;
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__divide_by_zero);

            // set blank output
            temp_result = 0;
        }

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer modulous
    case ANVIL__ot__integer_modulous:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0);
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1);

        // check if modulous is possible
        if (temp_input_1 != 0) {
            // compute result
            temp_result = temp_input_0 / temp_input_1;
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__modulous_by_zero);

            // set blank output
            temp_result = 0;
        }

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // integer range check
    case ANVIL__ot__integer_within_range:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // range start
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // value to be checked
        temp_input_2 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_2); // range end

        // check range
        temp_result = (ANVIL__cell_integer_value)((temp_input_0 <= temp_input_1) && (temp_input_1 <= temp_input_2));

        // set result cell
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)temp_result);

        break;
    // flag or
    case ANVIL__ot__flag_or: // untested!
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // second flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'or' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (ANVIL__bt)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) | ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_1)));

        break;
    // flag invert
    case ANVIL__ot__flag_invert: // untested!
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'or' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (ANVIL__bt)!(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0)));

        break;
    // flag and
    case ANVIL__ot__flag_and: // untested!
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // second flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'or' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (ANVIL__bt)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) & ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_1)));

        break;
    // flag xor
    case ANVIL__ot__flag_xor: // untested!
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // first flag
        temp_input_1 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_1); // second flag
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output flag

        // 'xor' flags into new flag
        ANVIL__set__flag_in_context(context, temp_result, (ANVIL__bt)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) != ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_1)));

        break;
    // flag get
    case ANVIL__ot__flag_get:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // flag address
        //temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // output cell

        // get flag
        ANVIL__set__cell_by_address(ANVIL__get__cell_address_from_context(context, output_0), (ANVIL__cell)(ANVIL__u64)(ANVIL__get__flag_from_context(context, (ANVIL__flag_ID)temp_input_0) > 0));

        break;
    // flag set
    case ANVIL__ot__flag_set:
        // get data
        temp_input_0 = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, input_0); // flag value
        temp_result = (ANVIL__cell_integer_value)ANVIL__get__cell_from_context(context, output_0); // destination flag ID

        // set flag
        ANVIL__set__flag_in_context(context, (ANVIL__flag_ID)temp_result, (ANVIL__bt)temp_input_0);

        break;
    // instruction ID was invalid
    default:
        // return failed instruction
        return ANVIL__nit__return_context;
    }

    return ANVIL__nit__next_instruction;
}

// process instruction
ANVIL__nit ANVIL__run__instruction(ANVIL__allocations* allocations, ANVIL__context* context) {
    // execution current read address
    ANVIL__cell* execution_read_address;

    // instruction ID
    ANVIL__it instruction_ID;

    // write cell temps
    ANVIL__cell write_cell__cell;
    ANVIL__cell_ID write_cell__cell_ID;

    // operate temps
    ANVIL__flag_ID operate__flag_ID;
    ANVIL__operation_ID operate__operation_ID;
    ANVIL__cell_ID operate__input_cell_ID_0;
    ANVIL__cell_ID operate__input_cell_ID_1;
    ANVIL__cell_ID operate__input_cell_ID_2;
    ANVIL__cell_ID operate__output_cell_ID_0;

    // request memory temps
    ANVIL__cell_ID request_memory__allocation_size;
    ANVIL__cell_ID request_memory__allocation_start;
    ANVIL__cell_ID request_memory__allocation_end;
    ANVIL__buffer request_memory__allocation;
    ANVIL__bt request_memory__buffer_appending_error = ANVIL__bt__false;

    // return memory temps
    ANVIL__cell_ID return_memory__allocation_start;
    ANVIL__cell_ID return_memory__allocation_end;
    ANVIL__buffer return_memory__allocation;
    ANVIL__bt return_memory__allocation_does_not_exist = ANVIL__bt__false;

    // address to cell temps
    ANVIL__flag_ID address_to_cell__flag_ID;
    ANVIL__length address_to_cell__source_cell_ID;
    ANVIL__length address_to_cell__bit_count_cell_ID;
    ANVIL__length address_to_cell__destination_cell_ID;

    // cell to address temps
    ANVIL__flag_ID cell_to_address__flag_ID;
    ANVIL__length cell_to_address__source_cell_ID;
    ANVIL__length cell_to_address__bit_count_cell_ID;
    ANVIL__length cell_to_address__destination_cell_ID;

    // file to buffer temps
    ANVIL__cell_ID file_to_buffer__file_name_start;
    ANVIL__cell_ID file_to_buffer__file_name_end;
    ANVIL__cell_ID file_to_buffer__file_data_start;
    ANVIL__cell_ID file_to_buffer__file_data_end;
    ANVIL__buffer file_to_buffer__file_name;
    ANVIL__buffer file_to_buffer__file_data;
    ANVIL__bt file_to_buffer__buffer_appending_error = ANVIL__bt__false;

    // buffer to file temps
    ANVIL__cell_ID buffer_to_file__file_data_start;
    ANVIL__cell_ID buffer_to_file__file_data_end;
    ANVIL__cell_ID buffer_to_file__file_name_start;
    ANVIL__cell_ID buffer_to_file__file_name_end;
    ANVIL__buffer buffer_to_file__file_data;
    ANVIL__buffer buffer_to_file__file_name;
    ANVIL__bt buffer_to_file__error = ANVIL__bt__false;

    // run temps
    ANVIL__cell_ID run__context_buffer_start;
    ANVIL__cell_ID run__context_buffer_end;
    ANVIL__cell_ID run__instruction_count;

    // debug putchar temps
    ANVIL__cell_ID debug__putchar__printing_cell_ID;

    // debug print cell as decimal temps
    ANVIL__cell_ID debug__print_cell_as_decimal__printing_cell_ID;

    // debug fgets temps
    ANVIL__cell_ID debug__fgets__buffer_address_start;
    ANVIL__cell_ID debug__fgets__buffer_address_end;
    ANVIL__u8 debug__fgets__temporary_string[ANVIL__define__input_string_max_length];
    ANVIL__buffer debug__fgets__buffer;
    ANVIL__length debug__fgets__buffer_length;
    ANVIL__bt debug__fgets__buffer_appending_error = ANVIL__bt__false;

    // debug mark data section temps
    ANVIL__cell debug__mark_data_section__section_length;

    // debug mark code section temps
    ANVIL__cell debug__mark_code_section__section_length;

    // setup execution read address
    execution_read_address = ANVIL__get__cell_address_from_context(context, ANVIL__rt__program_current_address);

    // get instruction ID from program
    instruction_ID = (ANVIL__it)ANVIL__read_next__instruction_ID(execution_read_address);

    // DEBUG
    // printf("[%lu]: instruction_ID: %lu\n", (ANVIL__u64)(*execution_read_address) - 1, (ANVIL__u64)instruction_ID);

    // process instruction accordingly
    switch (instruction_ID) {
    // if context should stop
    case ANVIL__it__stop:
        // DEBUG
        //printf("ANVIL__it__stop is running.\n");

        // return exit context
        return ANVIL__nit__return_context;
    // overwrite cell value
    case ANVIL__it__write_cell:
        // get parameters
        write_cell__cell = ANVIL__read_next__cell(execution_read_address);
        write_cell__cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // do action
        (*context).cells[write_cell__cell_ID] = write_cell__cell;

        break;
    // operate between cells
    case ANVIL__it__operate:
        // get parameters
        operate__flag_ID = ANVIL__read_next__flag_ID(execution_read_address);
        operate__operation_ID = ANVIL__read_next__operation_ID(execution_read_address);
        operate__input_cell_ID_0 = ANVIL__read_next__cell_ID(execution_read_address);
        operate__input_cell_ID_1 = ANVIL__read_next__cell_ID(execution_read_address);
        operate__input_cell_ID_2 = ANVIL__read_next__cell_ID(execution_read_address);
        operate__output_cell_ID_0 = ANVIL__read_next__cell_ID(execution_read_address);

        // if flag enabled
        if (ANVIL__get__flag_from_context(context, operate__flag_ID) == ANVIL__bt__true) {
            // perform operation
            return ANVIL__run__operation(context, (ANVIL__ot)operate__operation_ID, operate__input_cell_ID_0, operate__input_cell_ID_1, operate__input_cell_ID_2, operate__output_cell_ID_0);
        }

        break;
    // ask os for new buffer
    case ANVIL__it__request_memory:
        // get parameters
        request_memory__allocation_size = ANVIL__read_next__cell_ID(execution_read_address);
        request_memory__allocation_start = ANVIL__read_next__cell_ID(execution_read_address);
        request_memory__allocation_end = ANVIL__read_next__cell_ID(execution_read_address);

        // do action
        request_memory__allocation = ANVIL__open__buffer((ANVIL__length)(*context).cells[request_memory__allocation_size]);

        // add allocation if successful
        if (ANVIL__check__empty_buffer(request_memory__allocation) == ANVIL__bt__false) {
            // remember allocation
            ANVIL__remember__allocation(allocations, request_memory__allocation, &request_memory__buffer_appending_error);

            // if buffer could not be added
            if (request_memory__buffer_appending_error == ANVIL__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                ANVIL__close__buffer(request_memory__allocation);

                // setup empty buffer
                request_memory__allocation = ANVIL__create_null__buffer();
            }
        }

        // set cell data
        (*context).cells[request_memory__allocation_start] = request_memory__allocation.start;
        (*context).cells[request_memory__allocation_end] = request_memory__allocation.end;

        break;
    // return buffer to OS
    case ANVIL__it__return_memory:
        // get parameters
        return_memory__allocation_start = ANVIL__read_next__cell_ID(execution_read_address);
        return_memory__allocation_end = ANVIL__read_next__cell_ID(execution_read_address);

        // get parameters
        return_memory__allocation.start = (*context).cells[return_memory__allocation_start];
        return_memory__allocation.end = (*context).cells[return_memory__allocation_end];

        // remove buffer from valid allocation list
        ANVIL__forget__allocation(allocations, return_memory__allocation, &return_memory__allocation_does_not_exist);

        // if allocation existed
        if (return_memory__allocation_does_not_exist == ANVIL__bt__false) {
            // deallocate
            ANVIL__close__buffer(return_memory__allocation);
        // allocation did not exist
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // take data from an address and put it into a cell
    case ANVIL__it__address_to_cell:
        // get parameters
        address_to_cell__flag_ID = ANVIL__read_next__flag_ID(execution_read_address);
        address_to_cell__source_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        address_to_cell__bit_count_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        address_to_cell__destination_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // if valid address range
        if (ANVIL__check__valid_address_range_in_allocations(allocations, ANVIL__create__buffer((ANVIL__address)(*context).cells[address_to_cell__source_cell_ID], (ANVIL__address)(*context).cells[address_to_cell__source_cell_ID] + (((ANVIL__length)(*context).cells[address_to_cell__bit_count_cell_ID]) / ANVIL__define__bits_in_byte) - 1))) {
            // if flag enabled
            if (ANVIL__get__flag_from_context(context, address_to_cell__flag_ID) == ANVIL__bt__true) {
                // read data into cell
                (*context).cells[address_to_cell__destination_cell_ID] = (ANVIL__cell)ANVIL__read__buffer((ANVIL__address)(*context).cells[address_to_cell__source_cell_ID], ((ANVIL__length)(*context).cells[address_to_cell__bit_count_cell_ID]) / ANVIL__define__bits_in_byte);
            }
        // invalid address range
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_address_range);
        }

        break;
    // take data from a cell and put it at an address
    case ANVIL__it__cell_to_address:
        // get parameters
        cell_to_address__flag_ID = ANVIL__read_next__flag_ID(execution_read_address);
        cell_to_address__source_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        cell_to_address__bit_count_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);
        cell_to_address__destination_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // if valid address range
        if (ANVIL__check__valid_address_range_in_allocations(allocations, ANVIL__create__buffer((*context).cells[cell_to_address__destination_cell_ID], (*context).cells[cell_to_address__destination_cell_ID] + (((ANVIL__length)(*context).cells[cell_to_address__bit_count_cell_ID]) / ANVIL__define__bits_in_byte) - 1))) {
            // if flag enabled
            if (ANVIL__get__flag_from_context(context, cell_to_address__flag_ID) == ANVIL__bt__true) {
                // write data to an address
                ANVIL__write__buffer((ANVIL__u64)(*context).cells[cell_to_address__source_cell_ID], ((ANVIL__length)(*context).cells[cell_to_address__bit_count_cell_ID]) / ANVIL__define__bits_in_byte, (*context).cells[cell_to_address__destination_cell_ID]);
            }
        // invalid address range
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_address_range);
        }

        break;
    // take data from a file and create a buffer with it
    case ANVIL__it__file_to_buffer:
        // get parameters
        file_to_buffer__file_name_start = ANVIL__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_name_end = ANVIL__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_data_start = ANVIL__read_next__cell_ID(execution_read_address);
        file_to_buffer__file_data_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        file_to_buffer__file_name.start = (*context).cells[file_to_buffer__file_name_start];
        file_to_buffer__file_name.end = (*context).cells[file_to_buffer__file_name_end];

        // get data from file
        file_to_buffer__file_data = ANVIL__move__file_to_buffer(file_to_buffer__file_name);

        // check for errors
        if (file_to_buffer__file_data.start == ANVIL__define__null_address) {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__file_not_found);
        // append buffer as valid allocation
        } else {
            // remember allocation
            ANVIL__remember__allocation(allocations, file_to_buffer__file_data, &file_to_buffer__buffer_appending_error);

            // if buffer could not be added
            if (file_to_buffer__buffer_appending_error == ANVIL__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                ANVIL__close__buffer(file_to_buffer__file_data);

                // setup empty buffer
                file_to_buffer__file_data = ANVIL__create_null__buffer();
            }
        }

        // write data to cells
        (*context).cells[file_to_buffer__file_data_start] = file_to_buffer__file_data.start;
        (*context).cells[file_to_buffer__file_data_end] = file_to_buffer__file_data.end;

        break;
    // take a buffer and overwrite a file with it
    case ANVIL__it__buffer_to_file:
        // get parameters
        buffer_to_file__file_data_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_data_end = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_name_start = ANVIL__read_next__cell_ID(execution_read_address);
        buffer_to_file__file_name_end = ANVIL__read_next__cell_ID(execution_read_address);

        // setup temps
        buffer_to_file__file_data.start = (*context).cells[buffer_to_file__file_data_start];
        buffer_to_file__file_data.end = (*context).cells[buffer_to_file__file_data_end];
        buffer_to_file__file_name.start = (*context).cells[buffer_to_file__file_name_start];
        buffer_to_file__file_name.end = (*context).cells[buffer_to_file__file_name_end];

        // if source allocations exists
        if (ANVIL__check__valid_address_range_in_allocations(allocations, buffer_to_file__file_data) && ANVIL__check__valid_address_range_in_allocations(allocations, buffer_to_file__file_name)) {
            // create file
            ANVIL__move__buffer_to_file(&buffer_to_file__error, buffer_to_file__file_name, buffer_to_file__file_data);

            // check for errors
            if (buffer_to_file__error == ANVIL__bt__true) {
                // set error
                ANVIL__set__error_code_cell(context, ANVIL__et__file_not_created);
            }
        // if any allocations do not exist
        } else {
            // set error
            ANVIL__set__error_code_cell(context, ANVIL__et__invalid_allocation__allocation_does_not_exist);
        }

        break;
    // run a context like a program
    case ANVIL__it__run:
        // get parameters
        run__context_buffer_start = ANVIL__read_next__cell_ID(execution_read_address);
        run__context_buffer_end = ANVIL__read_next__cell_ID(execution_read_address);
        run__instruction_count = ANVIL__read_next__cell_ID(execution_read_address);

        // useless operation to quiet compiler
        run__context_buffer_end = run__context_buffer_end;

        // run context
        ANVIL__run__context(allocations, (ANVIL__context*)(*context).cells[run__context_buffer_start], (u64)(*context).cells[run__instruction_count]);

        break;
    // print one char to stdout
    case ANVIL__it__debug__putchar:
        // get parameters
        debug__putchar__printing_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // print
        putchar((u8)(u64)((*context).cells[debug__putchar__printing_cell_ID]));

        // flush stream for full update
        fflush(stdout);

        break;
    // print one cell as a decimal number
    case ANVIL__it__debug__print_cell_as_decimal:
        // get parameters
        debug__print_cell_as_decimal__printing_cell_ID = ANVIL__read_next__cell_ID(execution_read_address);

        // print
        printf("%lu", (u64)(*context).cells[debug__print_cell_as_decimal__printing_cell_ID]);

        break;
    // read one string from stdin
    case ANVIL__it__debug__fgets:
        // get parameters
        debug__fgets__buffer_address_start = ANVIL__read_next__cell_ID(execution_read_address);
        debug__fgets__buffer_address_end = ANVIL__read_next__cell_ID(execution_read_address);

        // zero out temporaries
        for (ANVIL__length i = 0; i < ANVIL__define__input_string_max_length; i++) {
            // zero out character
            debug__fgets__temporary_string[i] = 0;
        }
        debug__fgets__buffer_length = 0;

        // read string
        fgets((char*)debug__fgets__temporary_string, ANVIL__define__input_string_max_length, stdin);

        // read buffer for string size
        while (debug__fgets__buffer_length < ANVIL__define__input_string_max_length && debug__fgets__temporary_string[debug__fgets__buffer_length] != 0) {
            // increment length
            debug__fgets__buffer_length++;
        }

        // create buffer based on string length
        debug__fgets__buffer = ANVIL__open__buffer(debug__fgets__buffer_length);

        // if buffer is not empty
        if (ANVIL__check__empty_buffer(debug__fgets__buffer) == ANVIL__bt__false) {
            // copy data into buffer
            for (ANVIL__u64 i = 0; i < debug__fgets__buffer_length; i++) {
                // write character
                ANVIL__write__buffer((u8)debug__fgets__temporary_string[i], sizeof(ANVIL__u8), (ANVIL__u8*)debug__fgets__buffer.start + (i * sizeof(ANVIL__u8)));
            }

            // remember allocation
            ANVIL__remember__allocation(allocations, debug__fgets__buffer, &debug__fgets__buffer_appending_error);

            // if buffer could not be added
            if (debug__fgets__buffer_appending_error == ANVIL__bt__true) {
                // set internal error
                ANVIL__set__error_code_cell(context, ANVIL__et__internal_allocation_tracking_error__could_not_record_buffer);

                // free buffer since it could not be safely added and is thus useless
                ANVIL__close__buffer(debug__fgets__buffer);

                // setup empty buffer
                debug__fgets__buffer = ANVIL__create_null__buffer();
            }
        }

        // setup cells
        (*context).cells[debug__fgets__buffer_address_start] = debug__fgets__buffer.start;
        (*context).cells[debug__fgets__buffer_address_end] = debug__fgets__buffer.end;

        break;
    // mark section of data
    case ANVIL__it__debug__mark_data_section:
        // get parameters
        debug__mark_data_section__section_length = ANVIL__read_next__cell(execution_read_address);

        // skip over data section
        (*context).cells[ANVIL__rt__program_current_address] = (ANVIL__address)((u64)(*context).cells[ANVIL__rt__program_current_address] + (u64)debug__mark_data_section__section_length);

        break;
    // mark section of code
    case ANVIL__it__debug__mark_code_section:
        // instruction does nothing but mark code space, so get length and do nothing
        debug__mark_code_section__section_length = ANVIL__read_next__cell(execution_read_address);

        // useless operation to quiet compiler
        debug__mark_code_section__section_length = debug__mark_code_section__section_length;

        break;
    // in case instruction ID was invalid
    default:
        // set error
        ANVIL__set__error_code_cell(context, ANVIL__et__invalid_instruction_ID);

        // return exit context
        return ANVIL__nit__return_context;
    }

    // return next instruction by default
    return ANVIL__nit__next_instruction;
}

// run context
void ANVIL__run__context(ANVIL__allocations* allocations, ANVIL__context* context, ANVIL__instruction_count instruction_count) {
    ANVIL__nit next_instruction_action;

    // if an infinite amount of instructions can execute
    if (instruction_count == ANVIL__define__run_forever) {
        // run instructions
        while (1) {
            // run instruction
            next_instruction_action = ANVIL__run__instruction(allocations, context);

            // if quit
            if (next_instruction_action == ANVIL__nit__return_context) {
                return;
            }

            // assuming ANVIL__nit__next_instruction
            continue;
        }
    // if a finite amount of instructions can execute
    } else {
        for (ANVIL__instruction_count i = 0; i < instruction_count; i++) {
            // run instruction
            next_instruction_action = ANVIL__run__instruction(allocations, context);

            // check for early quitting
            if (next_instruction_action == ANVIL__nit__return_context) {
                return;
            }
        }
    }

    return;
}

/* Instruction Workspace */
// pass type
typedef enum ANVIL__pt {
    ANVIL__pt__get_offsets,
    ANVIL__pt__write_program,
    ANVIL__pt__COUNT,
} ANVIL__pt;

// offsets
typedef ANVIL__u64 ANVIL__offset;

// invalid offset placeholder
#define ANVIL__invalid_offset -1

// instruction creation container
typedef struct ANVIL__workspace {
    ANVIL__pt pass;
    ANVIL__u64 current_program_offset;
    ANVIL__address write_to;
    ANVIL__buffer* program_buffer;
} ANVIL__workspace;

// setup workspace
ANVIL__workspace ANVIL__setup__workspace(ANVIL__buffer* program_buffer_destination) {
    ANVIL__workspace output;

    // setup output
    output.pass = ANVIL__pt__get_offsets;
    output.current_program_offset = 0;
    output.write_to = (*program_buffer_destination).start;
    output.program_buffer = program_buffer_destination;

    return output;
}

// create an offset
ANVIL__offset ANVIL__get__offset(ANVIL__workspace* workspace) {
    // return current offset
    return (*workspace).current_program_offset;
}

// setup pass
void ANVIL__setup__pass(ANVIL__workspace* workspace, ANVIL__pt pass) {
    // setup pass in workspace
    (*workspace).pass = pass;

    // do stuff
    switch ((*workspace).pass) {
    case ANVIL__pt__get_offsets:
        (*workspace).current_program_offset = 0;
        
        break;
    case ANVIL__pt__write_program:
        // allocate program buffer
        (*(*workspace).program_buffer) = ANVIL__open__buffer((*workspace).current_program_offset);

        // setup pass
        (*workspace).current_program_offset = 0;
        (*workspace).write_to = (*(*workspace).program_buffer).start;

        break;
    default:
        break;
    }

    return;
}

/* Write Instruction Scraplets */
// write instruction ID
void ANVIL__write_next__instruction_ID(ANVIL__workspace* workspace, ANVIL__instruction_ID instruction_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__instruction_ID*)(*workspace).write_to) = instruction_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__instruction_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__instruction_ID));

    return;
}

// write flag ID
void ANVIL__write_next__flag_ID(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__flag_ID*)(*workspace).write_to) = flag_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__flag_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__flag_ID));

    return;
}

// write operation ID
void ANVIL__write_next__operation_ID(ANVIL__workspace* workspace, ANVIL__operation_ID operation_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__operation_ID*)(*workspace).write_to) = operation_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__operation_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__operation_ID));

    return;
}

// write cell ID
void ANVIL__write_next__cell_ID(ANVIL__workspace* workspace, ANVIL__cell_ID cell_ID) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__cell_ID*)(*workspace).write_to) = cell_ID;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__cell_ID);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__cell_ID));

    return;
}

// write cell value
void ANVIL__write_next__cell(ANVIL__workspace* workspace, ANVIL__cell cell_value) {
    // write value
    if ((*workspace).pass == ANVIL__pt__write_program) {
        *((ANVIL__cell*)(*workspace).write_to) = cell_value;
    }

    // advance
    (*workspace).current_program_offset += sizeof(ANVIL__cell);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + sizeof(ANVIL__cell));

    return;
}

// write buffer
void ANVIL__write_next__buffer(ANVIL__workspace* workspace, ANVIL__buffer buffer) {
    ANVIL__length buffer_length;

    // set buffer length
    buffer_length = ANVIL__calculate__buffer_length(buffer);

    // write buffer
    if ((*workspace).pass == ANVIL__pt__write_program) {
        // write buffer length
        ANVIL__write__buffer(buffer_length, sizeof(ANVIL__length), (*workspace).write_to);

        // copy buffer
        for (ANVIL__length byte_index = 0; byte_index < buffer_length; byte_index++) {
            ((ANVIL__u8*)(*workspace).write_to)[byte_index + sizeof(ANVIL__length)] = ((ANVIL__u8*)buffer.start)[byte_index];
        }
    }

    // advance
    (*workspace).current_program_offset += buffer_length + sizeof(ANVIL__length);
    (*workspace).write_to = (ANVIL__address)((u64)(*workspace).write_to + buffer_length + sizeof(ANVIL__length));

    return;
}

/* Write Instructions */
// write buffer data
void ANVIL__code__buffer(ANVIL__workspace* workspace, ANVIL__buffer buffer) {
    // write data
    ANVIL__write_next__buffer(workspace, buffer);

    return;
}

// write stop instruction
void ANVIL__code__stop(ANVIL__workspace* workspace) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__stop);

    return;
}

// write write cell instruction
void ANVIL__code__write_cell(ANVIL__workspace* workspace, ANVIL__cell value, ANVIL__cell_ID value_destination) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__write_cell);
    ANVIL__write_next__cell(workspace, value);
    ANVIL__write_next__cell_ID(workspace, value_destination);

    return;
}

// write operate instruction
void ANVIL__code__operate(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID, ANVIL__operation_ID operation_ID, ANVIL__cell_ID input_0, ANVIL__cell_ID input_1, ANVIL__cell_ID input_2, ANVIL__cell_ID output_0) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__operate);
    ANVIL__write_next__flag_ID(workspace, flag_ID);
    ANVIL__write_next__operation_ID(workspace, operation_ID);
    ANVIL__write_next__cell_ID(workspace, input_0);
    ANVIL__write_next__cell_ID(workspace, input_1);
    ANVIL__write_next__cell_ID(workspace, input_2);
    ANVIL__write_next__cell_ID(workspace, output_0);

    return;
}

// write request memory instruction
void ANVIL__code__request_memory(ANVIL__workspace* workspace, ANVIL__cell_ID allocation_size, ANVIL__cell_ID allocation_start, ANVIL__cell_ID allocation_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__request_memory);
    ANVIL__write_next__cell_ID(workspace, allocation_size);
    ANVIL__write_next__cell_ID(workspace, allocation_start);
    ANVIL__write_next__cell_ID(workspace, allocation_end);

    return;
}

// write return memory instruction
void ANVIL__code__return_memory(ANVIL__workspace* workspace, ANVIL__cell_ID allocation_start, ANVIL__cell_ID allocation_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__return_memory);
    ANVIL__write_next__cell_ID(workspace, allocation_start);
    ANVIL__write_next__cell_ID(workspace, allocation_end);

    return;
}

// write address to cell instruction
void ANVIL__code__address_to_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID, ANVIL__cell_ID source_address, ANVIL__cell_ID bit_count, ANVIL__cell_ID destination_cell) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__address_to_cell);
    ANVIL__write_next__flag_ID(workspace, flag_ID);
    ANVIL__write_next__cell_ID(workspace, source_address);
    ANVIL__write_next__cell_ID(workspace, bit_count);
    ANVIL__write_next__cell_ID(workspace, destination_cell);

    return;
}

// write cell to address instruction
void ANVIL__code__cell_to_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag_ID, ANVIL__cell_ID source_cell, ANVIL__cell_ID bit_count, ANVIL__cell_ID destination_address) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__cell_to_address);
    ANVIL__write_next__flag_ID(workspace, flag_ID);
    ANVIL__write_next__cell_ID(workspace, source_cell);
    ANVIL__write_next__cell_ID(workspace, bit_count);
    ANVIL__write_next__cell_ID(workspace, destination_address);

    return;
}

// write file to buffer instruction
void ANVIL__code__file_to_buffer(ANVIL__workspace* workspace, ANVIL__cell_ID file_name_start, ANVIL__cell_ID file_name_end, ANVIL__cell_ID file_data_start, ANVIL__cell_ID file_data_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__file_to_buffer);
    ANVIL__write_next__cell_ID(workspace, file_name_start);
    ANVIL__write_next__cell_ID(workspace, file_name_end);
    ANVIL__write_next__cell_ID(workspace, file_data_start);
    ANVIL__write_next__cell_ID(workspace, file_data_end);

    return;
}

// write buffer to file instruction
void ANVIL__code__buffer_to_file(ANVIL__workspace* workspace, ANVIL__cell_ID file_data_start, ANVIL__cell_ID file_data_end, ANVIL__cell_ID file_name_start, ANVIL__cell_ID file_name_end) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__buffer_to_file);
    ANVIL__write_next__cell_ID(workspace, file_data_start);
    ANVIL__write_next__cell_ID(workspace, file_data_end);
    ANVIL__write_next__cell_ID(workspace, file_name_start);
    ANVIL__write_next__cell_ID(workspace, file_name_end);

    return;
}

// write run instruction
void ANVIL__code__run(ANVIL__workspace* workspace, ANVIL__cell_ID context_buffer_start, ANVIL__cell_ID context_buffer_end, ANVIL__cell_ID instruction_count) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__run);
    ANVIL__write_next__cell_ID(workspace, context_buffer_start);
    ANVIL__write_next__cell_ID(workspace, context_buffer_end);
    ANVIL__write_next__cell_ID(workspace, instruction_count);

    return;
}

// write debug putchar instruction
void ANVIL__code__debug__putchar(ANVIL__workspace* workspace, ANVIL__cell_ID printing_cell_ID) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__putchar);
    ANVIL__write_next__cell_ID(workspace, printing_cell_ID);

    return;
}

// write debug print cell as decimal instruction
void ANVIL__code__debug__print_cell_as_decimal(ANVIL__workspace* workspace, ANVIL__cell_ID printing_cell_ID) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__print_cell_as_decimal);
    ANVIL__write_next__cell_ID(workspace, printing_cell_ID);

    return;
}

// write debug fgets instruction
void ANVIL__code__debug__fgets(ANVIL__workspace* workspace, ANVIL__cell_ID buffer_start_ID, ANVIL__cell_ID buffer_end_ID) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__fgets);
    ANVIL__write_next__cell_ID(workspace, buffer_start_ID);
    ANVIL__write_next__cell_ID(workspace, buffer_end_ID);

    return;
}

// write debug mark data section instruction
void ANVIL__code__debug__mark_data_section(ANVIL__workspace* workspace, ANVIL__cell buffer_length) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__mark_data_section);
    ANVIL__write_next__cell(workspace, buffer_length);

    return;
}

// write debug mark code section instruction
void ANVIL__code__debug__mark_code_section(ANVIL__workspace* workspace, ANVIL__cell code_buffer_length) {
    // write instruction
    ANVIL__write_next__instruction_ID(workspace, ANVIL__it__debug__mark_code_section);
    ANVIL__write_next__cell(workspace, code_buffer_length);

    return;
}

/* Stack ABI Defines */
// types
typedef u64 ANVIL__preserve;
typedef ANVIL__preserve ANVIL__preserve__start;
typedef ANVIL__preserve ANVIL__preserve__end;

// stack cell types
typedef enum ANVIL__srt {
    // start of cells
    ANVIL__srt__START = ANVIL__rt__END,

    // constant cells
    ANVIL__srt__constant__0 = ANVIL__srt__START,
    ANVIL__srt__constant__1,
    ANVIL__srt__constant__2,
    ANVIL__srt__constant__4,
    ANVIL__srt__constant__8,
    ANVIL__srt__constant__16,
    ANVIL__srt__constant__24,
    ANVIL__srt__constant__32,
    ANVIL__srt__constant__40,
    ANVIL__srt__constant__48,
    ANVIL__srt__constant__56,
    ANVIL__srt__constant__64,
    ANVIL__srt__constant__cell_byte_size,
    ANVIL__srt__constant__return_address_offset_creation_size,

    // context io cells
    ANVIL__srt__input_buffer_start,
    ANVIL__srt__input_buffer_end,
    ANVIL__srt__output_buffer_start,
    ANVIL__srt__output_buffer_end,

    // stack cells
    ANVIL__srt__stack__start_address,
    ANVIL__srt__stack__current_address,
    ANVIL__srt__stack__end_address,

    // control flow cells
    ANVIL__srt__return_address,

    // temporary cells
    ANVIL__srt__temp__write,
    ANVIL__srt__temp__offset,
    ANVIL__srt__temp__address,
    ANVIL__srt__temp__flag,
    ANVIL__srt__temp__flag_ID,
    ANVIL__srt__temp__bit_count,

    // end of cells
    ANVIL__srt__END,

    // aliases
    ANVIL__srt__constant__true = ANVIL__srt__constant__1,
    ANVIL__srt__constant__false = ANVIL__srt__constant__0,
    ANIVL__srt__constant__cell_byte_count = ANVIL__srt__constant__8,
    ANIVL__srt__constant__cell_bit_count = ANVIL__srt__constant__64,
    ANVIL__srt__constant__bits_in_byte = ANVIL__srt__constant__8,

    // locations
    ANVIL__srt__start__workspace = 4096,
    ANVIL__srt__start__function_io = ANVIL__rt__TOTAL_COUNT - 4096,

    // count
    ANVIL__srt__COUNT = ANVIL__srt__END - ANVIL__srt__START,
} ANVIL__srt;

// stack instruction types
typedef enum ANVIL__sit {
    // start
    ANVIL__sit__START = ANVIL__it__END,

    // instructions
    ANVIL__sit__cell_to_cell = ANVIL__sit__START,
    ANVIL__sit__push_cell,
    ANVIL__sit__pop_cell,
    ANVIL__sit__calculate_dynamically__offset_address,
    ANVIL__sit__calculate_statically__offset_address,
    ANVIL__sit__jump__explicit,
    ANVIL__sit__jump__static,
    ANVIL__sit__create_return_address__directly_after_jump,
    ANVIL__sit__call__explicit,
    ANVIL__sit__call__static,
    ANVIL__sit__start,
    ANVIL__sit__preserve_workspace,
    ANVIL__sit__restore_workspace,
    ANVIL__sit__operate__flag,
    ANVIL__sit__operate__jump__explicit,
    ANVIL__sit__operate__jump__dynamic,
    ANVIL__sit__operate__jump__static,
    ANVIL__sit__setup__context,

    // end
    ANVIL__sit__END,

    // counts
    ANVIL__sit__COUNT = ANVIL__sit__END - ANVIL__sit__START,
    ANVIL__sit__TOTAL_COUNT = ANVIL__sit__END - ANVIL__it__START,
} ANVIL__sit;

// stack instruction length types
typedef enum ANVIL__silt {
    ANVIL__silt__cell_to_cell = ANVIL__ilt__operate,
    ANVIL__silt__push_cell = ANVIL__ilt__cell_to_address + ANVIL__ilt__operate,
    ANVIL__silt__pop_cell = ANVIL__ilt__operate + ANVIL__ilt__address_to_cell,
    ANVIL__silt__calculate_dynamically__offset_address = ANVIL__ilt__operate,
    ANVIL__silt__calculate_statically__offset_address = ANVIL__ilt__write_cell + ANVIL__silt__calculate_dynamically__offset_address,
    ANVIL__silt__jump__explicit = ANVIL__silt__cell_to_cell,
    ANVIL__silt__jump__static = ANVIL__silt__calculate_statically__offset_address + ANVIL__silt__jump__explicit,
} ANVIL__silt;

// stack flag types
typedef enum ANVIL__sft {
    // start of flags
    ANVIL__sft__START = 0,

    // flags
    ANVIL__sft__always_run = ANVIL__sft__START,
    ANVIL__sft__never_run,
    ANVIL__sft__temp,

    // end of stack flags
    ANVIL__sft__END,
} ANVIL__sft;

// stack size
typedef u64 ANVIL__stack_size;

/* Context IO */
// pass input
void ANVIL__set__input(ANVIL__context* context, ANVIL__buffer input) {
    // write data to cells
    (*context).cells[ANVIL__srt__input_buffer_start] = (ANVIL__cell)input.start;
    (*context).cells[ANVIL__srt__input_buffer_end] = (ANVIL__cell)input.end;

    return;
}

/* Stack ABI Code */
// move one cell to the next
void ANVIL__code__cell_to_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID source, ANVIL__cell_ID destination) {
    // write code
    ANVIL__code__operate(workspace, flag, ANVIL__ot__cell_to_cell, source, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, destination);

    return;
}

// push a cell onto the stack
void ANVIL__code__push_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID source_cell) {
    // write data to stack
    ANVIL__code__cell_to_address(workspace, flag, source_cell, ANVIL__srt__constant__64, ANVIL__srt__stack__current_address);

    // increase stack pointer
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__srt__stack__current_address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__stack__current_address);

    return;
}

// pop a cell from the stack
void ANVIL__code__pop_cell(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID destination_cell) {
    // decrease stack pointer
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_subtract, ANVIL__srt__stack__current_address, ANVIL__srt__constant__cell_byte_size, ANVIL__unused_cell_ID, ANVIL__srt__stack__current_address);

    // read data from stack
    ANVIL__code__address_to_cell(workspace, flag, ANVIL__srt__stack__current_address, ANVIL__srt__constant__64, destination_cell);

    return;
}

// calculate an address from the program start and an offset cell
void ANVIL__code__calculate_dynamically__offset_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID offset_cell, ANVIL__cell_ID destination) {
    // calculate address
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__rt__program_start_address, offset_cell, ANVIL__unused_cell_ID, destination);

    return;
}

// calculate an address from the program start and an offset
void ANVIL__code__calculate_statically__offset_address(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__offset offset, ANVIL__cell_ID destination) {
    // write temp
    ANVIL__code__write_cell(workspace, (ANVIL__cell)offset, ANVIL__srt__temp__offset);

    // calculate address
    ANVIL__code__calculate_dynamically__offset_address(workspace, flag, ANVIL__srt__temp__offset, destination);

    return;
}

// jump to a specific address
void ANVIL__code__jump__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID address) {
    // jump
    ANVIL__code__cell_to_cell(workspace, flag, address, ANVIL__rt__program_current_address);

    return;
}

// jump to an offset calculated address
void ANVIL__code__jump__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__offset offset) {
    // calculate offset
    ANVIL__code__calculate_statically__offset_address(workspace, flag, offset, ANVIL__srt__temp__address);

    // jump
    ANVIL__code__jump__explicit(workspace, flag, ANVIL__srt__temp__address);

    return;
}

// create return address
void ANVIL__code__create_return_address__directly_after_jump(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID destination) {
    // create offset
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_add, ANVIL__rt__program_current_address, ANVIL__srt__constant__return_address_offset_creation_size, ANVIL__unused_cell_ID, destination);

    return;
}

// call function explicitly
void ANVIL__code__call__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID address) {
    // preserve return address
    ANVIL__code__push_cell(workspace, flag, ANVIL__srt__return_address);

    // setup new return address
    ANVIL__code__create_return_address__directly_after_jump(workspace, flag, ANVIL__srt__return_address);

    // jump
    ANVIL__code__jump__explicit(workspace, flag, address);

    // restore return address
    ANVIL__code__pop_cell(workspace, flag, ANVIL__srt__return_address);

    return;
}

// call function statically
void ANVIL__code__call__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__offset offset) {
    // calculate address
    ANVIL__code__calculate_statically__offset_address(workspace, flag, offset, ANVIL__srt__temp__address);

    // call function with offset set
    ANVIL__code__call__explicit(workspace, flag, ANVIL__srt__temp__address);

    return;
}

// kickstart program (assumes program variables are set!)
void ANVIL__code__start(ANVIL__workspace* workspace, ANVIL__stack_size stack_size, ANVIL__offset jump_to) {
    // setup code marker
    ANVIL__code__debug__mark_code_section(workspace, 0);

    // setup error cell
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__error_code);

    // setup flag cells
    ANVIL__code__write_cell(workspace, (ANVIL__cell)1, ANVIL__rt__flags_0);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__flags_1);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__flags_2);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__rt__flags_3);

    // setup constants
    ANVIL__code__write_cell(workspace, (ANVIL__cell)0, ANVIL__srt__constant__0);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)1, ANVIL__srt__constant__1);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)2, ANVIL__srt__constant__2);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)4, ANVIL__srt__constant__4);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)8, ANVIL__srt__constant__8);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)16, ANVIL__srt__constant__16);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)24, ANVIL__srt__constant__24);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)32, ANVIL__srt__constant__32);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)40, ANVIL__srt__constant__40);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)48, ANVIL__srt__constant__48);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)56, ANVIL__srt__constant__56);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)64, ANVIL__srt__constant__64);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__cell), ANVIL__srt__constant__cell_byte_size);
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__silt__jump__explicit, ANVIL__srt__constant__return_address_offset_creation_size);

    // setup output
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, ANVIL__srt__output_buffer_start);
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, ANVIL__srt__constant__0, ANVIL__srt__output_buffer_end);

    // setup stack
    ANVIL__code__write_cell(workspace, (ANVIL__cell)stack_size, ANVIL__srt__temp__write);
    ANVIL__code__request_memory(workspace, ANVIL__srt__temp__write, ANVIL__srt__stack__start_address, ANVIL__srt__stack__end_address);
    ANVIL__code__cell_to_cell(workspace, (ANVIL__flag_ID)ANVIL__sft__always_run, ANVIL__srt__stack__start_address, ANVIL__srt__stack__current_address);

    // jump to main
    ANVIL__code__call__static(workspace, ANVIL__sft__always_run, jump_to);

    // deallocate stack
    ANVIL__code__return_memory(workspace, ANVIL__srt__stack__start_address, ANVIL__srt__stack__end_address);

    // quit program
    ANVIL__code__stop(workspace);

    return;
}

// preserve workspace
void ANVIL__code__preserve_workspace(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__preserve__start preserve_start, ANVIL__preserve__end preserve_end) {
    // preserve flags
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_0);
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_1);
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_2);
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__flags_3);

    // preserve error code
    ANVIL__code__push_cell(workspace, flag, ANVIL__rt__error_code);

    // preserve workspace cells
    for (ANVIL__preserve i = preserve_start; i <= preserve_end; i++) {
        // preserve cell
        ANVIL__code__push_cell(workspace, flag, i);
    }

    return;
}

// restore workspace
void ANVIL__code__restore_workspace(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__preserve__start preserve_start, ANVIL__preserve__end preserve_end) {
    // restore workspace cells
    for (ANVIL__preserve i = preserve_end; i >= preserve_start; i--) {
        // restore cell
        ANVIL__code__pop_cell(workspace, flag, i);
    }

    // restore error code
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__error_code);

    // restore flags
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_3);
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_2);
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_1);
    ANVIL__code__pop_cell(workspace, flag, ANVIL__rt__flags_0);

    return;
}

// operate flag
void ANVIL__code__operate__flag(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID output_flag_ID) {
    // get comparison result set to variable
    ANVIL__code__operate(workspace, flag, ANVIL__ot__integer_within_range, lower_bound, value, upper_bound, ANVIL__srt__temp__flag);

    // write flag
    ANVIL__code__operate(workspace, flag, ANVIL__ot__flag_set, ANVIL__srt__temp__flag, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, output_flag_ID);

    // invert flag
    ANVIL__code__operate(workspace, invert_result, ANVIL__ot__flag_invert, output_flag_ID, ANVIL__unused_cell_ID, ANVIL__unused_cell_ID, output_flag_ID);

    return;
}

// operate jump explicitly
void ANVIL__code__operate__jump__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID jump_address) {
    // setup flag temp
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__sft__temp, ANVIL__srt__temp__flag_ID);

    // perform comparison
    ANVIL__code__operate__flag(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__flag_ID);

    // attempt jump
    ANVIL__code__jump__explicit(workspace, ANVIL__sft__temp, jump_address);

    return;
}

// operate jump dynamically
void ANVIL__code__operate__jump__dynamic(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID jump_offset_cell) {
    // calculate offset
    ANVIL__code__calculate_dynamically__offset_address(workspace, flag, jump_offset_cell, ANVIL__srt__temp__address);

    // attempt jump
    ANVIL__code__operate__jump__explicit(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__address);

    return;
}

// operate jump statically
void ANVIL__code__operate__jump__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__offset jump_offset) {
    // write offset to cell
    ANVIL__code__write_cell(workspace, (ANVIL__cell)jump_offset, ANVIL__srt__temp__offset);

    // attempt jump
    ANVIL__code__operate__jump__dynamic(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__offset);

    return;
}

/*// operate call explicitly
void ANVIL__code__operate__call__explicit(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__cell_ID address) {
    // setup flag temp
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__sft__temp, ANVIL__srt__temp__flag_ID);

    // perform comparison
    ANVIL__code__operate__flag(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__flag_ID);

    // perform call
    ANVIL__code__call__explicit(workspace, ANVIL__sft__temp, address);

    return;
}

// operate call statically
void ANVIL__code__operate__call__static(ANVIL__workspace* workspace, ANVIL__flag_ID flag, ANVIL__cell_ID lower_bound, ANVIL__cell_ID value, ANVIL__cell_ID upper_bound, ANVIL__flag_ID invert_result, ANVIL__offset jump_offset) {
    // setup flag temp
    ANVIL__code__write_cell(workspace, (ANVIL__cell)ANVIL__sft__temp, ANVIL__srt__temp__flag_ID);

    // perform comparison
    ANVIL__code__operate__flag(workspace, flag, lower_bound, value, upper_bound, invert_result, ANVIL__srt__temp__flag_ID);

    // perform call
    ANVIL__code__call__static(workspace, ANVIL__sft__temp, jump_offset);

    return;
}*/

// setup context
void ANVIL__code__setup__context(ANVIL__workspace* workspace, ANVIL__cell_ID program_buffer_start, ANVIL__cell_ID program_buffer_end, ANVIL__cell_ID context_buffer_start, ANVIL__cell_ID context_buffer_end) {
    // setup allocation size
    ANVIL__code__write_cell(workspace, (ANVIL__cell)sizeof(ANVIL__context), ANVIL__srt__temp__write);

    // allocate context
    ANVIL__code__request_memory(workspace, ANVIL__srt__temp__write, context_buffer_start, context_buffer_end);

    // setup skeleton context
    // setup buffer start
    ANVIL__code__cell_to_cell(workspace, ANVIL__sft__always_run, context_buffer_start, ANVIL__srt__temp__address);
    ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, program_buffer_start, ANIVL__srt__constant__cell_bit_count, ANVIL__srt__temp__address);

    // setup current address
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, ANVIL__srt__temp__address, ANIVL__srt__constant__cell_byte_count, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);
    ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, program_buffer_start, ANIVL__srt__constant__cell_bit_count, ANVIL__srt__temp__address);

    // setup end address
    ANVIL__code__operate(workspace, ANVIL__sft__always_run, ANVIL__ot__integer_add, ANVIL__srt__temp__address, ANIVL__srt__constant__cell_byte_count, ANVIL__unused_cell_ID, ANVIL__srt__temp__address);
    ANVIL__code__cell_to_address(workspace, ANVIL__sft__always_run, program_buffer_end, ANIVL__srt__constant__cell_bit_count, ANVIL__srt__temp__address);
    
    return;
}

#endif
