.include "p33EP512GM710.inc" 
                    

;*************************************************************************************
                    .section .xdata, data, near, xmemory
                    .text
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;=====================================================================================
; ������� �������� ALU, W0-W15,  �������� ����� � �����
;-------------------------------------------
;   ������������� ���������:
;   W0-W1 - ��������� �������� ������
;-------------------------------------------
					.global _CheckCPU_run
_CheckCPU_run:
		                        ;       ���� Z
                    disi    #(4-1)                              ;������ ����������
                    xor     W0, W0, W0                          ;��������� �������� W0
                    bra     nz, SFRFail                         ;���� ���� Z �������
                    dec     W0, W0                              ;���������� �������� W0 �� 1
                    bra     z, SFRFail                          ;���� ���� Z ����������
                    ;       ���� C
                    disi    #(4-1)                              ;������ ����������
                    add     W0, W0, W0                          ;�������� ��������� W0 � W0
                    bra     nc, SFRFail                         ;���� ���� C �������
                    add     #0, W0                              ;�������� �������� W0 � ��������� 0
                    bra     c, SFRFail                          ;���� ���� C ����������
                    ;       ���� N
                    disi    #(4-1)                              ;������ ����������
                    xor     W0, W0, W0                          ;��������� �������� W0
                    bra     n, SFRFail                          ;���� ���� N ����������
                    dec     W0, W0                              ;���������� �������� W0 �� 1
                    bra     nn, SFRFail                         ;���� ���� N �������
                    ;       ���� OV
                    disi    #(5-1)                              ;������ ����������
                    xor     W0, W0, W0                          ;��������� �������� W0
                    bra     ov, SFRFail                         ;���� ���� OV �������
                    mov     #0x7FFF, W0                         ;������ ��������� 0x7FFF � ������� W0
                    inc     W0, W0                              ;���������� �������� W0 �� 1
                    bra     nov, SFRFail                        ;���� ���� OV ����������
                    ;       ������� W0
                    mov     #0xA5, W0                           ;������ ��������� 0xA5 � ������� W0
                    xor     #0xA5, W0                           ;��������� ��������� W0 � ��������� 0xA5
                    bra     nz, SFRFail                         ;���� �� �����
                    mov     #0xA5, W0                           ;������ ��������� 0xA5 � ������� W0
                    swap.b  W0                                  ;������ ��������� 0x005A � ������� W0
                    xor     #0x5A, W0                           ;��������� ��������� W0 � ��������� 0x5A
                    bra     nz, SFRFail                         ;���� �� �����
                    mov     #0xA500, W0                         ;������ ��������� 0xA500 � ������� W0
                    swap    W0                                  ;������ ��������� 0x00A5 � ������� W0
                    xor     #0xA5, W0                           ;��������� ��������� W0 � ��������� 0xA5
                    bra     nz, SFRFail                         ;���� �� �����
                    mov     #0x5A00, W0                         ;������ ��������� 0x5A00 � ������� W0
                    swap    W0                                  ;������ ��������� 0x005A � ������� W0
                    xor     #0x5A, W0                           ;��������� ��������� W0 � ��������� 0x5A
                    bra     nz, SFRFail                         ;���� �� �����
                    ;       ������� W1
                    mov     #0x5AA5, W0                         ;������ ��������� 0x5AA5 � ������� W0
                    mov     W0, W1                              ;������ ��������� 0x5AA5 � ������� W1
                    cp      W1, W0                              ;��������� ��������� W0 � W1
                    bra     nz, SFRFail                         ;���� �� �����
                    swap    W0                                  ;������ ��������� 0xA55A � ������� W0
                    mov     W0, W1                              ;������ ��������� 0xA55A � ������� W1
                    cp      W1, W0                              ;��������� ��������� W0 � W1
                    bra     nz, SFRFail                         ;���� �� �����

                   ;       �������� ��������� FLASH-������
		       ;TBLPAG
                    mov     TBLPAG,  W1                         
                    mov     #0x5AA5, W0   
		    disi    #(3-1)    
                    mov     W0, TBLPAG
                    cp.b    TBLPAG
		    mov     W1, TBLPAG 
                    bra     nz, SFRFail
                    swap    W0
		    disi    #(3-1)    
                    mov     W0, TBLPAG
                    cp.b    TBLPAG
                    bra     nz, SFRFail
                       ; DSRPAG
	            mov     DSRPAG,  W1                
                    mov     #0x02A5, W0                         
		    disi    #(3-1)    
                    mov     W0, DSRPAG
                    cp      DSRPAG
	            mov     W1, DSRPAG                
                    bra     nz, SFRFail
                    mov     #0x015A, W0
		    disi    #(3-1)    
                    mov     W0, DSRPAG
                    cp.b    DSRPAG
	            mov     W1, DSRPAG                
                    bra     nz, SFRFail
                      ;DSWPAG
	         0: mov     DSWPAG,  W1                
                    mov     #0x00A5, W0                         
		    disi    #(3-1)    
                    mov     W0, DSWPAG
                    cp      DSWPAG
                    mov     W1, DSWPAG
                    bra     nz, SFRFail
                    mov     #0x015A, W0
		    disi    #(3-1)    
                    mov     W0, DSWPAG
                    cp.b    DSWPAG
                    mov     W1, DSWPAG
                    bra     nz, SFRFail
 
		    mov     RCOUNT,  W1 
                    mov     #0x1A5A, W0                         
		    disi    #(3-1)    
                    mov     W0, RCOUNT                          
                    cp      RCOUNT                              
                    mov     W1, RCOUNT
                    bra     nz, SFRFail                              
                    mov     #0x25A5, W0                         
		    disi    #(3-1)    
                    mov     W0, RCOUNT                          
                    cp      RCOUNT                              
                    mov     W1, RCOUNT
                    bra     nz, SFRFail
		    
		    ;-------������� W15 -- 
                    disi    #10  
                    mov     W15, W1      ; ����������
                    mov     #0x5AA4, W0
                    mov     W0, W15
                    cp      W15, W0
                    bra     nz, SFRFail
                    mov     #0xA55A, W0
                    mov     W0, W15
                    cp      W15, W0
                    bra     nz, SFRFail
                    mov     W1, W15      ; ��������������

		    ;-------������� SPLIM -- 
                    mov     SPLIM, W1      ; ����������
                    mov     #0x5AA4, W0
                    mov     W0, SPLIM
                    cp      SPLIM
                    bra     nz, SFRFail
                    mov     #0xA55A, W0
                    mov     W0, SPLIM
                    cp      SPLIM
                    bra     nz, SFRFail
                    mov     W1, SPLIM      ; ��������������
                    ;-------�������� W2-W14
                    mov     #0x5AA5, W0
                    mov     #0x0004, W1
                    do      #(13-1), 0f
                    push.w [W1]           ; ���������� �������� � ����� - ������ � ������������
                    mov     W0, [W1]
                    cp      W0, [W1]
                    bra     nz, SFRFail
                    swap    W0
                    mov     W0, [W1]
                    cp      W0, [W1]
                    bra     nz, SFRFail
                 2: pop.w  [W1]           ; �������������� �������� �� �����
                    ;                 
                 0: inc2    W1, W1
                   ;
           Ok:
                    mov     #0x0001, W0
                    return;
                    ;
           SFRFail: mov     #0x0000, W0
                    return;
.end
