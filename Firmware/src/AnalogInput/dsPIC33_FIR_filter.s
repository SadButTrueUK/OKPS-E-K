; \file    dsPIC33_FIR_filter.s
; \brief   КИХ фильтр для dsPIC33
; \version 1.0.1
; \date    08-02-2018
; \author  Агулов М.А.

.title	   "FIR Digital Filter Routine"
.include   "p33EP512GM710.inc"                          ; Файл определений для используемого процессора
.global	   _SignalFilter			        ; Имя функция вычисления КИХ-фильтра для С-компилятора

    
; *************************************************************************************************
; SignalFilter( int16_t    *FilterCoeffStartAddress,    [W0] - стартовый адрес B-коэффициентов фильтра
;               unt16_ t   **FilterDelayPtr,            [W1] - адрес ячейки ОЗУ, содержащую адрес текущей позиции в линии задержки
;               uint16_t   FilterCoeffLen		[W2] - количество B-коэффициентов фильтра
;               uint16_t   SampleCount			[W3] - количество отсчётов в массиве InputSamples (количество входных отсчётов)
;               int16_t    *FilterDelayStartAddress,    [W4] - указатель на 1-ый отсчёт в линии задержки (начало кольцевого буфера)
;               int16_t    *InputSamples                [W5] - указатель на входные данные
;               int16_t    AccShift                     [W6] - число сдвигов аккумулятора для приведения результата к формату int16_t
;
; Выходные данные:    InputSamples[SampleCount] - отфильтрованные данные

    
_SignalFilter:
    push    W8
    push    W10
    push    CORCON
    push    MODCON
    push    DSRPAG

    cp0	    w3					    ; Проверка, если SampleCount == 0
    bra	    z, NoSamples
    
; *** Настройка режимов работы DSP-ядра (регистр CORCON)***
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


; *** Инициализация аппаратного кольцевого буфера линии задержки ("Modulo Address Buffer", регистр MODCON)                  
    mov     W4, XMODSRT				    ; Начальный адрес кольцевого буфера линии задержки ("Modulo Address Buffer")


    movpag  #0x0001, DSRPAG
    
    sl	    W2, W7				    ; Вычисляем конечный адрес кольцевого буфера линии задержки ("Modulo Address Buffer")
    add	    W7, W4, W7				    ; XMODEND = XMODSRT + BufferLen - 1
    dec	    W7, W7                      
    mov     W7, XMODEND				    ; Конечный адрес кольцевого буфера линии задержки ("Modulo Address Buffer")         

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
;   nop						    ; "nop" нужен, если за этой командой следует индексное чтение/запись

    
; *** Подготовка данных для работы MAC-команд                   
    sub	    #3, W2				    ; Счётчик цикла для команды REPEAT уменьшаем на 3, т.к. цикл выполняется REPEAT+1 раз и дополнительно две команды MAC

SC0:    
    mov	    [W1], W8				    ; Адрес текущего положениия отсчета в линии задержки->W8
    mov	    [W5], [--W8]				    ; Текущий отсчёт->в линию задержки (по указателю [W8])
    mov	    W8, [W1]				    ; Сохранение декрементированного адреса на отсчет в линии задержки
    
    mov	    W0, W10				    ; Загрузка указателя на коэффициенты фильтра в Y-памяти (подготовка регистров для MAC-команды)

; *** Собственно реализация фильтра ***
    clr     A, [W8]+=2, W4, [W10]+=2, W7            ; Очистка аккумулятора MACA, загрузка данных в W4(отсчёт) и W5(коэффициент фильтра Bn), постинкремент адреса коэффициентов фильтра и линию задержки
    repeat  W2					    ; Вычисление фильтра Bnum-2 раз
    mac     W4*W7, A, [W8]+=2, W4, [W10]+=2, W7     ; MACA = MACA + B([W10++]) * S(W[8++]) для коэффициентов B(0...n-2)
    mac     W4*W7, A, [W8], W4, [W10], W7           ; Предпоследний коэффициент B(n-1)
    mac     W4*W7, A				    ; Последний коэффициент B(n)    

; *** Нормализация результата фильтрации, [W5] после нормализации содержит 16-битный результат в формате int16_t
    sftac   A, W6				    ; Сдвиг аккумулятора для приведения результата к int16 (зависит от масштабироваения коэффициентов фильтра, вычисляется в MatlLab)
    sac.r   A, #0, [W5++]			    ; Читаем округлённое значение MACA[31...16] в выходной массив

    dec	    W3, W3				    ; Цикл по числу отсчётов в массиве входных данных
    bra	    nz, SC0


NoSamples:
    pop	    DSRPAG
    pop	    MODCON
    pop     CORCON
    pop     W10
    pop     W8

    return 
.end
