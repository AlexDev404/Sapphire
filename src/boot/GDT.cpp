// Online C++ compiler to run C++ program online
#include <iostream>
#include <sstream>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

std::string convert_int(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

void init_gdt_desc(u32 base, u32 limite, u8 acces, u8 other)
{
    std::cout << "lim0_15 = " << convert_int((limite & 0xffff)) << std::endl;
    std::cout << "base0_15 = " << convert_int((base & 0xffff)) << std::endl;
    std::cout << "base16_23 = " << convert_int(((base & 0xff0000)) >> 16) << std::endl;
    std::cout << "access = " << convert_int(acces) << std::endl;
    std::cout << "lim16_19 = " << convert_int(((limite & 0xf0000) >> 16)) << std::endl;
    // std::cout << "other = " << convert_int((other & 0xf)) << std::endl;
    std::cout << "base24_31 = " << convert_int(((base & 0xff000000)) >> 24) << std::endl;
    return;
}

int main()
{
    // Write C++ code here
    /* initialize gdt segments */
    std::cout << std::endl
              << "NULL_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0x0, 0x0, 0x0);
    std::cout << std::endl
              << "CODE_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D); /* code */
    std::cout << std::endl
              << "DATA_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D); /* data */
    std::cout << std::endl
              << "STACK_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0x0, 0x97, 0x0D); /* stack */
    std::cout << std::endl
              << "UCODE_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D); /* ucode */
    std::cout << std::endl
              << "UDATA_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D); /* udata */
    std::cout << std::endl
              << "USTACK_SEGMENT" << std::endl
              << "============" << std::endl
              << std::endl;
    init_gdt_desc(0x0, 0x0, 0xF7, 0x0D); /* ustack */

    // init_gdt_desc((u32) & 0, 0x67, 0xE9, 0x00);    /* descripteur de tss */

    return 0;
}