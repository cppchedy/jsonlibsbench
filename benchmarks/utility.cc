#include "utility.h"

std::pair<char *, size_t> open_file(const char *file_name){
    FILE *file = fopen(file_name, "rb");
    if (!file){
        fprintf(stderr, "Failed to open file\n");
        return {};
    }
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = new char[length];

    if (length != fread(buffer, 1, length, file)){
        fprintf(stderr, "Failed to read entire file\n");
        return {};
    }
    fclose(file);
    return {buffer, length};
}