; \file    BinInDecoder.s
; \brief   
; \version 1.0.1
; \date    08-02-2018
; \author  ������ �.�.

.title ""
.include "p33EP512GM710.inc"                        ; ���� ����������� ��� ������������� ����������
.global _BitDecode_asm				    ;

;void BitDecode_asm( uint8_t *RawDataStartPtr, uint8_t *RawDataFinishPtr, uint16_t NumStates, uint8_t *cntPtr );
; *************************************************************************************************
; BitDecode ( uint16_t    *RawDataStartPtr,		 [W0] - ��������� ����� ������ ���������� (���� ���� �����������)
;             uint16_ t   *RawDataFinishPtr,		 [W1] - �������� ����� ������ ����������
;             uint8_t	   NumStates,			  W2  - ����� ��������� (��� � ������ ����������)
;             uint8_t      cntPtr,		         [W3] - �������� ����� �� ������� ���������� ������������ ������ � ����� �����
;
    
_BitDecode_asm:

    mov	    W0, W6		; ��������� ��������� �����
    mov	    #7, W7		; ������� ��� � ����� (�������� ������)

C1:
    mov	    W6, W0		; ��������������� ��������� �����
    clr	    W4			; ���������� ������� ���������� ���.1

ByteCycle:
    mov.b  [W0], W5
    lsr	    W5, W7, W5
    and	    #0x0001, W5

    add	    W4, W5, W4		; ������� ���.1

    add	    W0, W2, W0		; ��������� ������

    cpsgt   W0, W1 
    goto    ByteCycle

    mov.b    W4, [W3--]

    dec.b   W7, W7		; ��������� �������� ��� � �����
    bra	   NN, C1		; �������� �������� ��� � �����
    
    return 
.end
