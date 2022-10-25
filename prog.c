#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tags.h"
#include "jvmstructs.h"

void assert_it(int predicate, const char* mssg) {
    if (predicate) return;
    printf("%s\n", mssg);
    exit(1);
}

char* read_bytes_to_hex(FILE* fp, int bytes) {
    unsigned char buffer[bytes];
    char *hex = (char *) malloc(sizeof(char) * bytes * 2 + 1);
    char *str_ptr = hex;

    fread(buffer, sizeof(buffer), 1, fp);
    for (size_t i = 0; i < bytes; i++) {
        sprintf(str_ptr, "%02x", buffer[i]);
        str_ptr += 2 * sizeof(char);
    }
    return hex;
}

void print_hex(unsigned char* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", buffer[i]);
    }
    printf("\n");
}

unsigned char* read_raw_bytes(FILE* fp, size_t size) {
    unsigned char *buffer = (unsigned char*)malloc(size * sizeof(unsigned char));
    fread(buffer, size, 1, fp);
    return buffer;
}

unsigned char* reverse_buffer_u4(unsigned char* buffer, size_t size) {
    unsigned char *reversed_buffer = (unsigned char*) malloc(sizeof(unsigned int));
    for (size_t i = 0; i < sizeof(unsigned int); i++) {
        if (i >= size) {
            reversed_buffer[i] = 0;
        }
        reversed_buffer[i] = buffer[size - i - 1];
    }
    return reversed_buffer;
}

unsigned int read_to_int(FILE* fp, int bytes) {
    unsigned char buffer[bytes];
    fread(buffer, sizeof(buffer), 1, fp);
    return *(unsigned int*)reverse_buffer_u4(buffer, bytes);
}

unsigned int read_u1(FILE* fp) {
    return read_to_int(fp, 1);
}

unsigned int read_u2(FILE* fp) {
    return read_to_int(fp, 2);
}

unsigned int read_u4(FILE* fp) {
    return read_to_int(fp, 4);
}

cp_info* parse_constant_pool(FILE* file, int constant_pool_count) {
    cp_info *constant_pool = (cp_info*)malloc(sizeof(cp_info) * constant_pool_count);
    for (int i = 1; i < constant_pool_count; i++) {
        int tag = read_u1(file);
        // TODO: Add rest of the cp_tags
        switch (tag) {
            case CONSTANT_Methodref:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .class_index = read_u2(file),
                    .name_and_type_index = read_u2(file)
                };
                printf("%d -> {CONSTANT_Methodref, %d, %d}\n", i, constant_pool[i].class_index, constant_pool[i].name_and_type_index);
                break;

            case CONSTANT_Fieldref:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .class_index = read_u2(file),
                    .name_and_type_index = read_u2(file)
                };
                printf("%d -> {CONSTANT_Fieldref, %d, %d}\n", i, constant_pool[i].class_index, constant_pool[i].name_and_type_index);
                break;

            case CONSTANT_String:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .string_index = read_u2(file)
                };
                printf("%d -> {CONSTANT_String, %d}\n", i, constant_pool[i].string_index);
                break;

            case CONSTANT_Class:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .name_index = read_u2(file)
                };
                printf("%d -> {CONSTANT_Class, %d}\n", i, constant_pool[i].name_index);
                break;

            case CONSTANT_Utf8: {
                int length = read_u2(file);
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .length = length
                };
                constant_pool[i].bytes = (unsigned char*)malloc(sizeof(unsigned char) * length);
                constant_pool[i].bytes = read_raw_bytes(file, length);
                // *(constant_pool[i].bytes + length) = '\0';
                printf("%d -> {CONSTANT_Utf8, %d, %s}\n", i, constant_pool[i].length, constant_pool[i].bytes);
                break;
            }

            case CONSTANT_NameAndType:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .name_index = read_u2(file),
                    .descriptor_index = read_u2(file)
                };
                printf("%d -> {CONSTANT_NameAndType, %d, %d}\n", i, constant_pool[i].name_index, constant_pool[i].descriptor_index);
                break;

            case CONSTANT_Integer:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .bytes = read_raw_bytes(file, 4)
                };
                printf("%d -> {CONSTANT_Integer, %d}\n", i,*(int*)reverse_buffer_u4(constant_pool[i].bytes, 4));
                break;

            case CONSTANT_Float:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .bytes = read_raw_bytes(file, 4)
                };
                printf("%d -> {CONSTANT_Float, %f}\n", i,*(float*)reverse_buffer_u4(constant_pool[i].bytes, 4));
                break;

            case CONSTANT_Long:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .high_bytes = read_u4(file),
                    .low_bytes = read_u4(file)
                };
                i++;
                printf("%d -> {CONSTANT_Long, %ld}\n", i, ((long)constant_pool[i].high_bytes << 32) + constant_pool[i].low_bytes);
                break;

            case CONSTANT_Double:
                constant_pool[i] = (cp_info) {
                    .tag = tag,
                    .high_bytes = read_u4(file),
                    .low_bytes = read_u4(file)
                };
                i++;
                long x = ((long)constant_pool[i].high_bytes << 32) + constant_pool[i].low_bytes;
                printf("%d -> {CONSTANT_Double, %lf}\n", i, *(double*)(&x));
                break;

            default: {
                char error_mssg[50];
                sprintf(error_mssg, "Tag not implemented %d\n", tag);
                assert_it(0, error_mssg);
                break;
            }
        }
    }
    return constant_pool;
}

char** parse_accessor_flags(int access_flag) {
    char** flags = (char**) malloc(sizeof(char*) * accessor_table_length);
    int flags_size = 0;
    for (int i = 0; i < accessor_table_length; i++) {
        if (accessor_table[i].mask & access_flag) {
            flags[flags_size] = (char*)malloc(sizeof(char) * strlen(accessor_table[i].flag_name));
            strcpy(flags[flags_size++], accessor_table[i].flag_name);
        }
    }
    return flags;
}

void assert_class_property(int class_u2, int constant_pool_count, cp_info* constant_pool, int strict_nonzero) {
    assert_it(
        (strict_nonzero == 0 && class_u2 == 0) || (class_u2 > 0 && class_u2 < constant_pool_count),
        "Corrupted class file, `this_class` is out of expected range!"
    );
    if (strict_nonzero == 0) return;
    assert_it(
        (constant_pool[class_u2].tag == CONSTANT_Class),
        "Corrupted, `this_class` doesn't refer to a class!"
    );
}

attribute_info* parse_attributes(FILE* fp, int attribute_count) {
    attribute_info* attributes = (attribute_info*) malloc(sizeof(attribute_info) * attribute_count);
    for (int i = 0; i < attribute_count; i++) {
        attributes[i] = (attribute_info) {
            .attribute_name_index = read_u2(fp),
            .attribute_length = read_u4(fp),
        };
        attributes[i].info = read_raw_bytes(fp, attributes[i].attribute_length);
    }
    return attributes;
}

field_info* parse_fields(FILE* fp, int fields_count) {
    field_info* fields = (field_info*) malloc(sizeof(field_info) * fields_count);
    for (int i = 0; i < fields_count; i++) {
        fields[i] = (field_info) {
            .access_flags = read_u2(fp),
            .name_index = read_u2(fp),
            .descriptor_index = read_u2(fp),
            .attribute_count = read_u2(fp),
        };
        fields[i].attributes = parse_attributes(fp, fields[i].attribute_count);
    }

    return fields;
}

int main() {
    FILE *file = fopen("./Main.class", "rb");
    char* magic = read_bytes_to_hex(file, 4);
    assert_it(strcmp(magic, "cafebabe") == 0, "Invalid class file!");
    unsigned int major = read_u2(file);
    unsigned int minor = read_u2(file);

    unsigned int constant_pool_count = read_u2(file);
    cp_info* constant_pool = parse_constant_pool(file, constant_pool_count);

    unsigned int access_flag = read_u2(file);
    char** flags = parse_accessor_flags(access_flag);
    printf("access flags:\n");
    for (int i=0;i<accessor_table_length;i++) {
        if(!flags[i]) break;
        printf("  %s\n", flags[i]);
    }

    unsigned int this_class = read_u2(file);
    assert_class_property(this_class, constant_pool_count, constant_pool, 1);
    unsigned int super_class = read_u2(file);
    assert_class_property(super_class, constant_pool_count, constant_pool, 0);

    unsigned int interfaces_count = read_u2(file);
    printf("interfaces_count: %d\n", interfaces_count);
    unsigned int interfaces[interfaces_count];
    for (int i = 0;i < interfaces_count; i++) {
        interfaces[i] = read_u2(file);
        assert_it(interfaces[i] > 0 && interfaces[i] < constant_pool_count, "Invalid interfaces");
    }

    unsigned int fields_count = read_u2(file);
    field_info* fields = parse_fields(file, fields_count);
    
    unsigned int methods_count = read_u2(file);
    field_info* methods = parse_fields(file, methods_count);

    unsigned int attribute_count = read_u2(file);
    attribute_info* attributes = parse_attributes(file, attribute_count);
    printf("%s\n", constant_pool[attributes[0].attribute_name_index].bytes);
    fclose(file);
}
