
#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdbool.h>
#include <stdint.h>

/*!
 * FIFO structure
 */
typedef struct Fifo_s
{
    uint16_t Begin;
    uint16_t End;
    uint8_t *Data;
    uint16_t Size;
}Fifo_t;

void FifoInit( Fifo_t *fifo, uint8_t *buffer, uint16_t size );

void FifoPush( Fifo_t *fifo, uint8_t data );
uint8_t FifoPop( Fifo_t *fifo );
void FifoFlush( Fifo_t *fifo );
bool IsFifoEmpty( Fifo_t *fifo );
bool IsFifoFull( Fifo_t *fifo );

#endif 
