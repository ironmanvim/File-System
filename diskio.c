//
// Created by ironmanvim on 24/6/19.
//

#include "diskio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE (16 * 1024)
#define NO_OF_METADATA_BLOCKS 4

static FILE *hdd = NULL;
static char hdd_name[128] = "";
static int hdd_initialized = 0;
static int current_block_no = 0;

/*
 * IT MOVES THE CURSOR IN HARD DISK TO THE START OF BLOCK NUMBER
 */
static void goto_block(unsigned int block_no) {
    int status = fseek(hdd, block_no * BLOCK_SIZE, SEEK_SET);
    if (status != 0) {
        printf("GOTO BLOCK FAILED.\n");
        return;
    }
    current_block_no = block_no;
}

struct Metadata *get_metadata() {
    void *buffer = malloc(BLOCK_SIZE);
    read_block(buffer, 0);
    struct Metadata *metadata = malloc(sizeof(struct Metadata));
    memcpy(metadata, buffer, sizeof(struct Metadata));
    free(buffer);
    return metadata;
}

void put_metadata(struct Metadata *metadata) {
    void *buffer = malloc(BLOCK_SIZE);
    memcpy(buffer, metadata, sizeof(struct Metadata));
    write_block(buffer, 0);
    free(buffer);
}

/*
 * IT INITIALIZES THE HARD DISK FOR USAGE
 */
void init(char *hard_disk_name) {
    hdd = fopen(hard_disk_name, "r+b");
    if (hdd == NULL) {
        printf("HARD DISK INITIALIZATION OF (%s) FAILED.\n", hard_disk_name);
        hdd_initialized = 0;
        return;
    }
    strcpy(hdd_name, hard_disk_name);
    hdd_initialized = 1;
    struct Metadata *metadata = get_metadata();
    if (metadata->magic_number != 0x594E4F54) {
        printf("HARD DISK IS CORRUPTED. DO YOU WANT TO FORMAT IT: ");
        char option = getchar();
        if (option != 'y') {
            hdd_initialized = 0;
            return;
        }
        format();
        printf("HARD FORMATTED SUCCESSFULLY");
    }
    free(metadata);
}

/*
 * READ A BLOCK FROM HARD DISK
 */
void read_block(void *buffer, unsigned int block_no) {
    if (!hdd_initialized) {
        printf("HARD DISK NOT INITIALIZED");
        return;
    }
    goto_block(block_no);
    int status = fread(buffer, BLOCK_SIZE, 1, hdd);
    if (status != 1) {
        printf("READ BLOCK UNSUCCESSFUL.\n");
        return;
    }
    fflush(hdd);
}

/*
 * WRITE A BLOCK FROM HARD DISK
 */
void write_block(void *buffer, unsigned int block_no) {
    if (!hdd_initialized) {
        printf("HARD DISK NOT INITIALIZED");
        return;
    }
    goto_block(block_no);
    int status = fwrite(buffer, BLOCK_SIZE, 1, hdd);
    if (status != 1) {
        printf("WRITE BLOCK UNSUCCESSFUL.\n");
        return;
    }
    fflush(hdd);
}

/*
 * FORMAT THE HARD DISK TO MY SPECIFICATIONS
 */
void format() {
    if (!hdd_initialized) {
        printf("HARD DISK NOT INITIALIZED");
        return;
    }
    struct Metadata *metadata = malloc(sizeof(struct Metadata));
    metadata->magic_number = 0x594E4F54;
    metadata->no_of_files = 0;
    put_metadata(metadata);
    free(metadata);
}
