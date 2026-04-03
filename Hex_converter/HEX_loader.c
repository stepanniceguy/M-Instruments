/*Includes----------------------------------------------------*/

#include <stdio.h>

/*Macro-------------------------------------------------------*/

#define PARSE_BYTE_FROM_FILE(a) (ascii_to_hex_f(fgetc(a)) << 4) | ascii_to_hex_f(fgetc(a))

/*Variables---------------------------------------------------*/

int inc = 0;

/*Prototypes--------------------------------------------------*/

u_int8_t ascii_to_hex_f(u_int8_t half_byte);
u_int16_t packing_f(int8_t number_of_halfbytes, u_int8_t* buff);

/*Typedefs----------------------------------------------------*/

typedef enum{
    BINARY = 0,
    EOF_HEX,
    SEG_ADRR_WRITE,
    FORMAT_80x86,
    EXTENDED_ADDR,
    LINEAR_ADDR
}HEX_command_status_t;

typedef struct {

    u_int8_t byte_count;

    union{
        int32_t addr_32bit;
        int16_t addr_16bit[2];
        int8_t addr_8bit[4];
    };
    
    u_int8_t narrow_addr[2];
    u_int16_t narrow_addr_v;

    HEX_command_status_t command_status;

    u_int8_t data[16];
    u_int8_t control_byte;
}Intel_HEX_t;

/*------------------------------------------------------------*/

int main(void){

    FILE *fp = fopen("HEX_file_example.hex", "r");

    if(fp == NULL){
        perror("FAULT");
        return 1;
    }

    while(!feof(fp)){

/*String reading----------------------------------------------*/
        
        Intel_HEX_t frame = {0};
            
        char doubledot = fgetc(fp); // ':' parsing

        frame.byte_count = PARSE_BYTE_FROM_FILE(fp);

        for(int i = 3; i >= 0; i--)\
            frame.narrow_addr[i] = ascii_to_hex_f(fgetc(fp));
        
        frame.narrow_addr_v = packing_f(4, frame.narrow_addr);

        frame.command_status = PARSE_BYTE_FROM_FILE(fp);
    
        switch(frame.command_status){
            default:
                for(int j = 0; j<frame.byte_count; j++)\
                    frame.data[j] = (u_int8_t)PARSE_BYTE_FROM_FILE(fp);
            
                break;
        }

        frame.control_byte = PARSE_BYTE_FROM_FILE(fp);

        doubledot = fgetc(fp); // '\0' parsing

/*Printing----------------------------------------------------*/

        inc++;    
        printf("STRING NUMBER: %d\n\n", inc);

        printf(" BYTE COUNT: 0x%.2x\n NARROW ADDR: 0x%.4x \n CMD: 0x%.2x\n CRC: 0x%.2x\n",\
            frame.byte_count, frame.narrow_addr_v,\
            frame.command_status, (u_int8_t)frame.control_byte);

        printf(" DATA: 0x");

        for(int k = 0; k < frame.byte_count; k++)\
            printf("%.2x", frame.data[k]);
        
        puts("\n");

/*------------------------------------------------------------*/
    
    }

    fclose(fp);

    return 0;
}

/*Functions---------------------------------------------------*/

u_int8_t ascii_to_hex_f(u_int8_t half_byte){

    switch(half_byte){
        case(48): half_byte = 0x0; break;
        case(49): half_byte = 0x1; break;
        case(50): half_byte = 0x2; break;
        case(51): half_byte = 0x3; break;
        case(52): half_byte = 0x4; break;
        case(53): half_byte = 0x5; break;
        case(54): half_byte = 0x6; break;
        case(55): half_byte = 0x7; break;
        case(56): half_byte = 0x8; break;
        case(57): half_byte = 0x9; break;
        case(65): half_byte = 0xA; break;
        case(66): half_byte = 0xB; break;
        case(67): half_byte = 0xC; break;
        case(68): half_byte = 0xD; break;
        case(69): half_byte = 0xE; break;
        case(70): half_byte = 0xF; break;

        default:
           puts("Wrong data!");
    }

    return half_byte;
}

u_int16_t packing_f(int8_t number_of_halfbytes, u_int8_t* buff){

    u_int16_t variable = 0;

    for(int i = 0; i < number_of_halfbytes; i++)\
        variable |= (buff[i] << i*4);

    return variable;

} // function turn array to united data in form of variable