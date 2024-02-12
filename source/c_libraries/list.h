#ifndef ANVIL__c_list
#define ANVIL__c_list

/* Include */
// basic / anvil
#include "../anvil/anvil.h"

/* Define */
// list types
typedef ANVIL__address LIST__list_start;
typedef ANVIL__address LIST__list_end;
typedef ANVIL__u64 LIST__list_filled_index;
typedef ANVIL__u64 LIST__list_increase;
typedef ANVIL__u64 LIST__list_data;
typedef ANVIL__u64 LIST__byte_size;

/* List */

// list object
typedef struct LIST__list {
    ANVIL__buffer buffer;
    LIST__list_filled_index filled_index;
    LIST__list_increase increase;
} LIST__list;

// create a list
LIST__list LIST__create__list(ANVIL__buffer buffer, LIST__list_filled_index filled_index, LIST__list_increase increase) {
    LIST__list output;

    // setup output
    output.buffer = buffer;
    output.filled_index = filled_index;
    output.increase = increase;

    return output;
}

// create a null list
LIST__list LIST__create_null__list() {
    // return empty list
    return LIST__create__list(ANVIL__create_null__buffer(), 0, 0);
}

// open a list
LIST__list LIST__open__list(LIST__list_increase increase, ANVIL__bt* error_occured) {
    LIST__list output;
    ANVIL__buffer allocation;

    // allocate list
    allocation = ANVIL__open__buffer(increase);

    // check list validity
    if (allocation.start == 0) {
        // set error
        *error_occured = ANVIL__bt__true;

        // return empty
        return LIST__create_null__list();
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
void LIST__close__list(LIST__list list) {
    // free buffer
    ANVIL__close__buffer(ANVIL__create__buffer(list.buffer.start, list.buffer.end));

    return;
}

// expand a list
void LIST__expand__list(LIST__list* list, ANVIL__bt* error_occured) {
    LIST__list_filled_index new_size;
    ANVIL__buffer new_allocation;

    // calculate new buffer size
    new_size = ((ANVIL__u64)(*list).buffer.end - (ANVIL__u64)(*list).buffer.start + 1) + (*list).increase;

    // request new memory
    new_allocation = ANVIL__open__buffer(new_size);

    // check for failure
    if (new_allocation.start == 0) {
        // set error
        *error_occured = ANVIL__bt__true;

        // return unmodified list
        return;
    }

    // copy old data to new list
    for (LIST__list_filled_index i = 0; i < (*list).filled_index; i++) {
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
void LIST__request__space(LIST__list* list, LIST__byte_size byte_size, ANVIL__bt* error_occured) {
    // expand the list until there is enough space
    while (((ANVIL__u64)(*list).buffer.end - (ANVIL__u64)(*list).buffer.start + 1) < ((*list).filled_index + byte_size)) {
        // expand the list
        LIST__expand__list(list, error_occured);

        // check for error
        if (*error_occured == ANVIL__bt__true) {
            // return last modified list
            return;
        }
    }

    return;
}

// calculate the tip of the list
ANVIL__address LIST__calculate__list_current(LIST__list* list) {
    return (*list).buffer.start + (*list).filled_index;
}

// add a buffer to a list
void LIST__append__buffer(LIST__list* list, ANVIL__buffer buffer, ANVIL__bt* memory_error_occured) {
    // request space
    LIST__request__space(list, sizeof(ANVIL__buffer), memory_error_occured);

    // append data
    (*(ANVIL__buffer*)LIST__calculate__list_current(list)) = buffer;

    // increase fill
    (*list).filled_index += sizeof(ANVIL__buffer);

    return;
}

// add a list to a list
void LIST__append__list(LIST__list* list, LIST__list data, ANVIL__bt* memory_error_occured) {
    // request space
    LIST__request__space(list, sizeof(LIST__list), memory_error_occured);

    // append data
    (*(LIST__list*)LIST__calculate__list_current(list)) = data;

    // increase fill
    (*list).filled_index += sizeof(LIST__list);

    return;
}

#endif
