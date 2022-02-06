; \file    dsPIC33_FIR_filter.s
; \brief   ��� ������ ��� dsPIC33
; \version 1.0.1
; \date    08-02-2018
; \author  ������ �.�.

.title	   "FIR Digital Filter Routine"
.include   "p33EP512GM710.inc"                          ; ���� ����������� ��� ������������� ����������
.global	   _SignalFilter			        ; ��� ������� ���������� ���-������� ��� �-�����������

    
; *************************************************************************************************
; SignalFilter( int16_t    *FilterCoeffStartAddress,    [W0] - ��������� ����� B-������������� �������
;               unt16_ t   **FilterDelayPtr,            [W1] - ����� ������ ���, ���������� ����� ������� ������� � ����� ��������
;               uint16_t   FilterCoeffLen		[W2] - ���������� B-������������� �������
;               uint16_t   SampleCount			[W3] - ���������� �������� � ������� InputSamples (���������� ������� ��������)
;               int16_t    *FilterDelayStartAddress,    [W4] - ��������� �� 1-�� ������ � ����� �������� (������ ���������� ������)
;               int16_t    *InputSamples                [W5] - ��������� �� ������� ������
;               int16_t    AccShift                     [W6] - ����� ������� ������������ ��� ���������� ���������� � ������� int16_t
;
; �������� ������:    InputSamples[SampleCount] - ��������������� ������

    
_SignalFilter:
    push    W8
    push    W10
    push    CORCON
    push    MODCON
    push    DSRPAG

    cp0	    w3					    ; ��������, ���� SampleCount == 0
    bra	    z, NoSamples
    
; *** ��������� ������� ������ DSP-���� (������� CORCON)***
    mov # 0<<US1 | 0<<US0 | 0<<EDT  | 1<<SATA | 0<<SATB | 1<<SATDW | 1<<ACCSAT | 0<<IPL3 | 0<<RND | 1<<IF, W8
;                   |       |         |         |         |          |           |         |         |        |
;                   |       |         |         |         |          |           |         |         |        |_1 - Integer mode enabled for DSP multiply
;                   |       |         |         |         |          |           |         |         |__________0 - Unbiased (convergent) rounding enabled
;                   |       |         |         |         |          |           |         |____________________1 - Program space visible in data space
;                   |       |         |         |         |          |           |______________________________0 - CPU interrupt priority level is 7 or less
;                   |       |         |         |         |          |__________________________________________1 - 9.31 saturation (super saturation)
;                   |       |         |         |         |_____________________________________________________1 - Data space write saturation enabled
;                   |       |         |         |_______________________________________________________________0 - Accumulator B saturation disabled
;                   |       |         |_________________________________________________________________________1 - Accumulator A saturation enabled
;                   |       |___________________________________________________________________________________0 - Early DO Loop Termination Control bit - no effect
;                   |___________________________________________________________________________________________0 - DSP engine multiplies are signed
    mov	    W8, CORCON


; *** ������������� ����������� ���������� ������ ����� �������� ("Modulo Address Buffer", ������� MODCON)                  
    mov     W4, XMODSRT				    ; ��������� ����� ���������� ������ ����� �������� ("Modulo Address Buffer")


    movpag  #0x0001, DSRPAG
    
    sl	    W2, W7				    ; ��������� �������� ����� ���������� ������ ����� �������� ("Modulo Address Buffer")
    add	    W7, W4, W7				    ; XMODEND = XMODSRT + BufferLen - 1
    dec	    W7, W7                      
    mov     W7, XMODEND				    ; �������� ����� ���������� ������ ����� �������� ("Modulo Address Buffer")         

    mov     # 1<<XMODEN | 0<<YMODEN | 1<<BWM3 | 1<<BWM2 | 1<<BWM1 | 1<<BWM0 | 1<<YWM3 | 0<<YWM2 | 0<<YWM1 | 0<<YWM0 | 1<<XWM3 | 0<<XWM2 | 0<<XWM1 | 0<<XWM0, W4            
;                      |           |           |         |         |         |         |         |         |         |         |         |         |         |
;                      |           |           |         |         |         |         |         |         |         |         |         |         |         |_XWM<3:0>: X RAGU and X WAGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |         |         |         |         |         |         |___________XWM<3:0>: X RAGU and X WAGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |         |         |         |         |         |_____________________XWM<3:0>: X RAGU and X WAGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |         |         |         |         |_______________________________XWM<3:0>: X RAGU and X WAGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |         |         |         |_________________________________________YWM<3:0>: Y AGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |         |         |___________________________________________________YWM<3:0>: Y AGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |         |_____________________________________________________________YWM<3:0>: Y AGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |         |_______________________________________________________________________YWM<3:0>: Y AGU W Register Select for Modulo Addressing bits
;                      |           |           |         |         |         |_________________________________________________________________________________BWM<3:0>: X WAGU Register Select for Bit-Reversed Addressing bits
;                      |           |           |         |         |___________________________________________________________________________________________BWM<3:0>: X WAGU Register Select for Bit-Reversed Addressing bits
;                      |           |           |         |_____________________________________________________________________________________________________BWM<3:0>: X WAGU Register Select for Bit-Reversed Addressing bits
;                      |           |           |_______________________________________________________________________________________________________________BWM<3:0>: X WAGU Register Select for Bit-Reversed Addressing bits
;                      |           |___________________________________________________________________________________________________________________________YMODEN: Y AGU Modulus Addressing Enable bit
;                      |_______________________________________________________________________________________________________________________________________XMODEN: X RAGU and X WAGU Modulus Addressing Enable bit
    mov     W4, MODCON    
;   nop						    ; "nop" �����, ���� �� ���� �������� ������� ��������� ������/������

    
; *** ���������� ������ ��� ������ MAC-������                   
    sub	    #3, W2				    ; ������� ����� ��� ������� REPEAT ��������� �� 3, �.�. ���� ����������� REPEAT+1 ��� � ������������� ��� ������� MAC

SC0:    
    mov	    [W1], W8				    ; ����� �������� ���������� ������� � ����� ��������->W8
    mov	    [W5], [--W8]				    ; ������� ������->� ����� �������� (�� ��������� [W8])
    mov	    W8, [W1]				    ; ���������� ������������������� ������ �� ������ � ����� ��������
    
    mov	    W0, W10				    ; �������� ��������� �� ������������ ������� � Y-������ (���������� ��������� ��� MAC-�������)

; *** ���������� ���������� ������� ***
    clr     A, [W8]+=2, W4, [W10]+=2, W7            ; ������� ������������ MACA, �������� ������ � W4(������) � W5(����������� ������� Bn), ������������� ������ ������������� ������� � ����� ��������
    repeat  W2					    ; ���������� ������� Bnum-2 ���
    mac     W4*W7, A, [W8]+=2, W4, [W10]+=2, W7     ; MACA = MACA + B([W10++]) * S(W[8++]) ��� ������������� B(0...n-2)
    mac     W4*W7, A, [W8], W4, [W10], W7           ; ������������� ����������� B(n-1)
    mac     W4*W7, A				    ; ��������� ����������� B(n)    

; *** ������������ ���������� ����������, [W5] ����� ������������ �������� 16-������ ��������� � ������� int16_t
    sftac   A, W6				    ; ����� ������������ ��� ���������� ���������� � int16 (������� �� ���������������� ������������� �������, ����������� � MatlLab)
    sac.r   A, #0, [W5++]			    ; ������ ���������� �������� MACA[31...16] � �������� ������

    dec	    W3, W3				    ; ���� �� ����� �������� � ������� ������� ������
    bra	    nz, SC0


NoSamples:
    pop	    DSRPAG
    pop	    MODCON
    pop     CORCON
    pop     W10
    pop     W8

    return 
.end
