#ifndef TAGS_H_
#define TAGS_H_ 

#include "jvmstructs.h"
const int CONSTANT_Class = 7;
const int CONSTANT_Fieldref	= 9;
const int CONSTANT_Methodref = 10;
const int CONSTANT_InterfaceMethodref = 11;
const int CONSTANT_String = 8;
const int CONSTANT_Integer = 3;
const int CONSTANT_Float = 4;
const int CONSTANT_Long = 5;
const int CONSTANT_Double = 6;
const int CONSTANT_NameAndType = 12;
const int CONSTANT_Utf8 = 1;
const int CONSTANT_MethodHandle = 15;
const int CONSTANT_MethodType = 16;
const int CONSTANT_InvokeDynamic = 18;

const int accessor_table_length = 8;
const accessor_info accessor_table[] = {
    (accessor_info){.flag_name = "ACC_PUBLIC", .mask =  0x0001},
    (accessor_info){.flag_name = "ACC_FINAL", .mask =  0x0010},
    (accessor_info){.flag_name = "ACC_SUPER", .mask =  0x0020},
    (accessor_info){.flag_name = "ACC_INTERFACE", .mask =  0x0200},
    (accessor_info){.flag_name = "ACC_ABSTRACT", .mask =  0x0400},
    (accessor_info){.flag_name = "ACC_SYNTHETIC", .mask =  0x1000},
    (accessor_info){.flag_name = "ACC_ANNOTATION", .mask =  0x2000},
    (accessor_info){.flag_name = "ACC_ENUM", .mask =  0x3000},
};

#endif
