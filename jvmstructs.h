#ifndef JVM_STRUCTS_H_
#define JVM_STRUCTS_H_

typedef struct _cp_info {
    int tag;
    int class_index;
    int name_and_type_index;
    int string_index;
    int name_index;
    int length;
    unsigned char *bytes;
    int descriptor_index;
    int high_bytes;
    int low_bytes;
} cp_info;

typedef struct _accessor_info {
    char flag_name[32];
    int mask;
} accessor_info;

typedef struct _attribute_info {
    int attribute_name_index;
    int attribute_length;
    unsigned char *info;
} attribute_info ;

typedef struct _field_info {
    int access_flags;
    int name_index;
    int descriptor_index;
    int attribute_count;
    attribute_info *attributes;
} field_info;

#endif // !JVM_STRUCTS_H_
