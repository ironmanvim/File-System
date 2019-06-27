#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "diskio.h"


int main() {
    init("hard_disk.hdd");
    copy_from_disk("hello.mp4", "demo.mp4");
}