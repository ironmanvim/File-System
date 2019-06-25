//
// Created by ironmanvim on 25/6/19.
//

#include "fileio.h"
#include "diskio.h"
#include <stdio.h>

unsigned int get_next_free_block() {
    struct Metadata *metadata = get_metadata();
    return 0;
}

unsigned int get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

void copy_to_disk(char *filename, char *alias_name) {
    FILE *file = fopen(filename, "rb");
    unsigned int file_size = get_file_size(file);

}

void copy_from_disk(char *filenaame, char *alias_name) {

}

