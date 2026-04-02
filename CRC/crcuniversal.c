#include <stdio.h>
#include <stdbool.h>

#define BITMASK(A) (1ULL << (A))

typedef enum{
    BIT_8 = 8,
    BIT_16 = 16,
    BIT_32 = 32,
    BIT_64 = 64
}bit_width_t; 

typedef struct{
    bit_width_t width; //Степень полинома
    unsigned long long poly; //Генераторный полином
    unsigned long long initial; //Исходное содержимое регистра вычислений
    bool refin; //Реверсивный вход
    bool refout; //Реверсивный выход
    unsigned short xorout; //Значение второго операнда для выходной операции исключающего ИЛИ

    unsigned long long  reg; //Оперативный регистр
}crc_obj_t;

unsigned long long widmask(crc_obj_t* crc_t){
    return (( 1 << (crc_t->width)) - 1ULL);
}

unsigned long long byte_reflect(unsigned long long value, unsigned char bit_width){

    unsigned long long refdata = 0;

    for(int i = 0; i < bit_width; i++)\
        refdata |= ((value & (1 << i)) == 0) ? (0 << ((bit_width - 1) - i)) : (1 << ((bit_width - 1) - i));

    return refdata;

}

void byte_crc_iteration(unsigned char crc_byte_in, crc_obj_t* crc_t){

    unsigned char crc_byte_temp = crc_byte_in;
    unsigned long long topbit = BITMASK(crc_t->width - 1);

    if(crc_t->refin)crc_byte_temp = byte_reflect(crc_byte_in, 8);

    crc_t->reg ^= crc_byte_temp << (crc_t->width - 8);

    for(int i = 0; i < 8; i++)
    {
        if(crc_t->reg & topbit){
            crc_t->reg = (crc_t->reg << 1) ^ crc_t->poly;
        }else{
            crc_t->reg <<= 1;
        }
    }
    crc_t->reg &= widmask(crc_t);
}

void crc_calc(crc_obj_t* crc_t, unsigned char* buf, int len){
    while(len--){
        byte_crc_iteration(*buf++, crc_t);
    }
}

unsigned long long crc_result(crc_obj_t* crc_t){
    if(crc_t->refout){
        return byte_reflect(crc_t->reg, crc_t->width) ^ crc_t->xorout;
    }else{
        return crc_t->reg ^ crc_t->xorout;
    }
}

int main(void){

    unsigned char test_data[] = {0x1, 0x2, 0x3, 0x4};

/* MODBUS example ---------------------------------------------------*/

    crc_obj_t MB = {0};
    
    MB.width = BIT_16;
    MB.poly = 0x8005;
    MB.initial = 0xFFFF;
    MB.refin = true;
    MB.refout = true;
    MB.xorout = 0x0000;
    MB.reg = (unsigned long long)MB.initial;

/*-------------------------------------------------------------------*/

    crc_calc(&MB, test_data, 4);

    printf("%llx", crc_result(&MB));

    return 0;
}
