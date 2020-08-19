; \file    BinInDecoder.s
; \brief   
; \version 1.0.1
; \date    08-02-2018
; \author  Агулов М.А.

.title ""
.include "p33EP512GM710.inc"                        ; Файл определений для используемого процессора
.global _BitDecode_asm				    ;

;void BitDecode_asm( uint8_t *RawDataStartPtr, uint8_t *RawDataFinishPtr, uint16_t NumStates, uint8_t *cntPtr );
; *************************************************************************************************
; BitDecode ( uint16_t    *RawDataStartPtr,		 [W0] - начальный адрес буфера накопления (этот байт проверяется)
;             uint16_ t   *RawDataFinishPtr,		 [W1] - конечный адрес буфера накопления
;             uint8_t	   NumStates,			  W2  - число состояний (шаг в буфере накопления)
;             uint8_t      cntPtr,		         [W3] - конечный адрес на счетчик количества обнаруженных единиц в битах байта
;
    
_BitDecode_asm:

    mov	    W0, W6		; Сохраняем стартовый адрес
    mov	    #7, W7		; Счётчик бит в байте (обратный отсчёт)

C1:
    mov	    W6, W0		; Восстанавливаем стартовый адрес
    clr	    W4			; Сбрасываем счетчик количества лог.1

ByteCycle:
    mov.b  [W0], W5
    lsr	    W5, W7, W5
    and	    #0x0001, W5

    add	    W4, W5, W4		; Счётчик лог.1

    add	    W0, W2, W0		; Инкремент адреса

    cpsgt   W0, W1 
    goto    ByteCycle

    mov.b    W4, [W3--]

    dec.b   W7, W7		; Декремент счётчика бит в байте
    bra	   NN, C1		; Проверка счётчика бит в байте
    
    return 
.end
