#include <stdio.h>

#define BUF_SIZE 100 //optional

void put_word_tx(u_int8_t msg);
u_int8_t read_word_tx();
void rb_clean_tx();
void put_word_rx(u_int8_t msg);
u_int8_t read_word_rx();
void rb_clean_rx();

typedef enum {
    OVERFLOW = 1,
    BUSY     = 1 << 1,
    FULL     = 1 << 2,
    EMPTY    = 1 << 3
}ringbuf_status_t;

int headtx = 0;
int tailtx = 0;
int headrx = 0;
int tailrx = 0;

ringbuf_status_t rb_status_rx = EMPTY;
ringbuf_status_t rb_status_tx = EMPTY;

u_int8_t op_buffer_tx[BUF_SIZE];
u_int8_t op_buffer_rx[BUF_SIZE];

void put_word_tx(u_int8_t msg)
{
    if(rb_status_tx != OVERFLOW && rb_status_tx != FULL)
    {
        op_buffer_tx[headtx] = msg;
        headtx = (headtx == (BUF_SIZE - 1) ? 0 : ++headtx);
        rb_status_tx = BUSY; 
    }

    if(headtx == tailtx)\
        rb_status_tx = FULL;
    if((rb_status_tx == FULL) && (headtx != tailtx))\
        rb_status_tx = OVERFLOW;
          
}

u_int8_t read_word_tx()
{
    if((rb_status_tx == FULL) && (headtx != tailtx)){
        rb_status_tx = OVERFLOW;
        return 0;
    }

    if(headtx == tailtx)\
        rb_status_tx = BUSY;
    
    return op_buffer_tx[(tailtx == BUF_SIZE - 1) ? 0 : tailtx++];
}

void rb_clean_tx()
{
    headtx = 0;
    tailtx = 0;

    rb_status_tx = EMPTY;
}

void put_word_rx(u_int8_t msg)
{    
    if(rb_status_rx != OVERFLOW && rb_status_rx != FULL)
    {
        op_buffer_rx[headrx] = msg;
        headrx = (headrx == (BUF_SIZE - 1) ? 0 : ++headrx);
        rb_status_rx = BUSY; 
    }

    if(headrx == tailrx)\
        rb_status_rx = FULL;
    if((rb_status_rx == FULL) && (headrx != tailrx))\
        rb_status_rx = OVERFLOW;

}

u_int8_t read_word_rx()
{
    if((rb_status_rx == FULL) && (headrx != tailrx)){
        rb_status_rx = OVERFLOW;
        return 0;
    }

    if(headrx == tailrx)\
        rb_status_rx = BUSY;

    return op_buffer_rx[(tailrx == BUF_SIZE - 1) ? 0 : tailrx++];
}

void rb_clean_rx()
{
    headrx = 0;
    tailrx = 0;

    rb_status_rx = EMPTY;
}