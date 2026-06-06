; Register map:
; R0/R1 (register pair 0): Values for SRC and FIN
; R2: Loop counter
; R3: Keyboard column for input
; R4: Keyboard row for input
; R5: The number 6 (loop counter start value)
; R6/R7 (register pair 3): Character to print

; Go ahead and set R5 = 6
LDM #6
XCH #5

:WAIT_FOR_INPUT
JCN %0001 :WAIT_FOR_INPUT

; Send low bit and clock pulse to shifter (selects first column)
LDM %0001
WRR
; reset shifter
LDM %0000
WRR

; #15 indicates no input
LDM #15
XCH #3

; Set loop counter
LDM #6 ; Loop 16-6 = 10 times (10-bit shifter)
XCH #2

; Loop to read keyboard
:READ_KEYBOARD
; Set source to second ROM chip (keyboard)
LDM #1
XCH #0
SRC #0

; Read keyboard column
RDR
JCN %0100 :NO_INPUT_FROM_COLUMN

; Input found - compute and store row number
KBP
DAC
XCH #4

; Compute and store column number
LD #2
SUB #5
XCH #3

:NO_INPUT_FROM_COLUMN

; Shift to the next keyboard column
; Set source to first ROM chip (shifter)
LDM #0
XCH #0
SRC #0

; Send high bit and clock pulse to shifter (selects next column)
LDM %0011
WRR
; reset shifter
LDM %0000
WRR

ISZ #2 :READ_KEYBOARD
; End of loop to read keyboard

; Figure out the character to print, print it, and return
; to beginning to wait for more input
JMS :MAP_ROW_COLUMN_TO_CHARACTER
XCH #6
WMP
XCH #7
WMP
JUN :WAIT_FOR_INPUT

; Subroutine to get keyboard character value from row and column
; Input:  R3, R4 (row, column)
; Output: R6/R7 (register pair 3)
; Register map:
; R0/R1: for SRC and FIN
; R6/R7: number of rows and columns for keyboard
; (R6 read in but not used. Could be used for error checking.)
; R8: loop counter
:MAP_ROW_COLUMN_TO_CHARACTER
; Set source to second ROM chip (keyboard character map)
LDM #1
XCH #0
SRC #0

; Load number of rows and columns into R6/R7
CLB
XCH #0
CLB
XCH #1
FIN #3

; Clear R0 and R1
CLB
XCH #0
CLB
XCH #1

; Calculate loop counter
LDM #15
CLC
SUB #4 ; row number
XCH #8

; Calculate index of key value

; keyboard map starts at index 1
LDM #1

; This loop implements multiplication
; ISZ is cleary designed to be used at the end of a loop, but we may need 0
; loops, so we use ISZ at the beginning. Thus, we end up having to jump over
; a jump!
:CALC_INDEX_LOOP
ISZ #8 :CONT_CALC_INDEX_LOOP
JUN :END_CALC_INDEX_LOOP
:CONT_CALC_INDEX_LOOP
CLC
ADD #7 ; columns per row
JCN %1010 :NO_CARRY
INC #0
:NO_CARRY
JUN :CALC_INDEX_LOOP
:END_CALC_INDEX_LOOP

; Finish by adding column number
CLC
ADD #3
JCN %1010 :NO_CARRY_2
INC #0
:NO_CARRY_2

; Fetch key value
XCH #1
FIN #3

; Before returning, set back to first ROM chip (shifter)
LDM #0
XCH #0
SRC #0

BBL #0
