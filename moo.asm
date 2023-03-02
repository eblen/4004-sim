; Read solution index (2 hex digits)
; Six similar instructions for each digit

; READ FIRST DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait1
JCN %0001 :kb_wait1

RDR ; Read from keyboard
WMP ; Write to tape
XCH #0 ; Write first digit to R0


; READ SECOND DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait2
JCN %0001 :kb_wait2

RDR ; Read from keyboard
WMP ; Write to tape
XCH #1 ; Write second digit to R1

; Double value in register pair R0/R1 to create a byte address
CLC
LD #1
RAL
XCH #1
LD #0
RAL
XCH #0

; Now fetch solution from ROM

; Set to second ROM chip
; (Using index register pair 3)
LDM #1
XCH #6
SRC #3

; Fetch
FIN #2
INC #1
FIN #1

; Set back to first ROM chip
LDM #0
XCH #6
SRC #3

; Place high digits into R0 and R1 (previously needed for FIN)
XCH #4
XCH #0
XCH #5
XCH #1

; Print newline
LDM #0
WMP
LDM #1
WMP

; ========= DEBUGGING - UNCOMMENT TO PRINT ANSWER TO TAPE ========

; Print first digit
; LDM #1
; WMP
; LD  #0
; WMP

; Print second digit
; LDM #1
; WMP
; LD  #1
; WMP

; Print third digit
; LDM #1
; WMP
; LD  #2
; WMP

; Print fourth digit
; LDM #1
; WMP
; LD  #3
; WMP

; Print newline
; LDM #0
; WMP
; LDM #1
; WMP

; ========= END DEBUGGING - UNCOMMENT TO PRINT ANSWER TO TAPE ========

; Main Loop
:MAIN

; Read player guess (4 digits)
; Six similar instructions for each digit

; READ FIRST DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait1
JCN %0001 :kb_wait1

RDR ; Read from keyboard
WMP ; Write to tape
XCH #4 ; Write first digit to R4


; READ SECOND DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait2
JCN %0001 :kb_wait2

RDR ; Read from keyboard
WMP ; Write to tape
XCH #5 ; Write second digit to R5


; READ THIRD DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait3
JCN %0001 :kb_wait3

RDR ; Read from keyboard
WMP ; Write to tape
XCH #6 ; Write third digit to R6


; READ FOURTH DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait4
JCN %0001 :kb_wait4

RDR ; Read from keyboard
WMP ; Write to tape
XCH #7 ; Write fourth digit to R7

; Print newline
LDM #0
WMP
LDM #1
WMP

; Clear previous counts
CLB
XCH #8
CLB
XCH #9
CLB
XCH #10

; Compute new counts of bulls, cows, and sheep
LD  #0
CLC
SUB #4
JCN %1100 :NO_BULL
INC #8
JUN :SECOND_DIGIT

:NO_BULL
LD  #1
CLC
SUB #4
JCN %0100 :COW_FOUND

LD  #2
CLC
SUB #4
JCN %0100 :COW_FOUND

LD  #3
CLC
SUB #4
JCN %0100 :COW_FOUND
INC #10
JUN :SECOND_DIGIT

:COW_FOUND
INC #9

:SECOND_DIGIT
LD  #1
CLC
SUB #5
JCN %1100 :NO_BULL_2
INC #8
JUN :THIRD_DIGIT

:NO_BULL_2
LD  #0
CLC
SUB #5
JCN %0100 :COW_FOUND_2

LD  #2
CLC
SUB #5
JCN %0100 :COW_FOUND_2

LD  #3
CLC
SUB #5
JCN %0100 :COW_FOUND_2
INC #10
JUN :THIRD_DIGIT

:COW_FOUND_2
INC #9

:THIRD_DIGIT
LD  #2
CLC
SUB #6
JCN %1100 :NO_BULL_3
INC #8
JUN :FOURTH_DIGIT

:NO_BULL_3
LD  #0
CLC
SUB #6
JCN %0100 :COW_FOUND_3

LD  #1
CLC
SUB #6
JCN %0100 :COW_FOUND_3

LD  #3
CLC
SUB #6
JCN %0100 :COW_FOUND_3
INC #10
JUN :FOURTH_DIGIT

:COW_FOUND_3
INC #9

:FOURTH_DIGIT
LD  #3
CLC
SUB #7
JCN %1100 :NO_BULL_4
INC #8
JUN :PRINT_BCS_RESULTS

:NO_BULL_4
LD  #0
CLC
SUB #7
JCN %0100 :COW_FOUND_4

LD  #1
CLC
SUB #7
JCN %0100 :COW_FOUND_4

LD  #2
CLC
SUB #7
JCN %0100 :COW_FOUND_4
INC #10
JUN :PRINT_BCS_RESULTS

:COW_FOUND_4
INC #9


; Print the results before returning to top of main loop
:PRINT_BCS_RESULTS
LDM #1
WMP
LD  #8
WMP

LDM #1
WMP
LD  #9
WMP

LDM #1
WMP
LD  #10
WMP

; Print newline
LDM #0
WMP
LDM #1
WMP

JUN :MAIN
