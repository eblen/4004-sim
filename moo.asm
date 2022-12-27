; Read seed (4 digits)
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


; READ THIRD DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait3
JCN %0001 :kb_wait3

RDR ; Read from keyboard
WMP ; Write to tape
XCH #2 ; Write third digit to R2


; READ FOURTH DIGIT
; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait4
JCN %0001 :kb_wait4

RDR ; Read from keyboard
WMP ; Write to tape
XCH #3 ; Write fourth digit to R3


; Compute guess from seed
; Goal is to make it highly difficult to guess "guess"
; from seed itself.
; First, combine all digits in A in some convoluted way.
SUB #0
ADD #1
ADD #2
ADD #2
ADD #3

; Now, apply A to each digit in a convoluted way
; Add A to digit 1
ADD #0
XCH #0
LD  #0

; Rotate A and add to digit 2
RAL
ADD #1
XCH #1
LD  #1

; increment and rotate A twice and add to digit 3
IAC
RAL
RAL
ADD #2
XCH #2
LD  #2

; Double A, rotate right, and add to digit 4
ADD #2
RAR
ADD #3
XCH #3

; For now, just print guess to tape

; Print newline
LDM #0
WMP
LDM #1
WMP

; Print first digit
LDM #1
WMP
LD  #0
WMP

; Print second digit
LDM #1
WMP
LD  #1
WMP

; Print third digit
LDM #1
WMP
LD  #2
WMP

; Print fourth digit
LDM #1
WMP
LD  #3
WMP

; Halt
:halt
JUN :halt
