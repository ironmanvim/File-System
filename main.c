#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "diskio.h"


int main() {
    init("hard_disk.hdd");
    copy_to_disk("hello.mp4", "hello.mp4");
}