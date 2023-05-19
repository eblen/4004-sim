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
; (Using index register pair 1)
LDM #1
XCH #2
SRC #1

; Fetch solution
; (Using index register pairs 2 and 3)
FIN #2
INC #1
FIN #3

; Set back to first ROM chip
; Also set RAM to first chip, first register, and main
; memory character 12
CLB
XCH #2
LDM #12
XCH #3
SRC #1

; Write solution to RAM
XCH #4
WRM
INC #3
SRC #1

XCH #5
WRM
INC #3
SRC #1

XCH #6
WRM
INC #3
SRC #1

XCH #7
WRM
INC #3
SRC #1

; Print newline
LDM #0
WMP
LDM #1
WMP

; ========= DEBUGGING - COMMENT FIRST LINE TO PRINT ANSWER TO TAPE ========
JUN :END_DEBUGGING

; Initialize index register pair 0 to read solution
; (intentionally different from pair used to set solution)
CLB
XCH #0
LDM #12
XCH #1

:PRINT_SOLUTION_DIGIT
SRC #0

; Write character set to tape
LDM #1
WMP

; output digit
RDM
WMP

ISZ #1 :PRINT_SOLUTION_DIGIT
; End loop to print solution digits

; Print newline
LDM #0
WMP
LDM #1
WMP

; Uncomment to halt program after debugging output
; :HALT_AFTER_DEBUG
; JUN :HALT_AFTER_DEBUG

; ========= END DEBUGGING - UNCOMMENT TO PRINT ANSWER TO TAPE ========
:END_DEBUGGING

; Main Loop
:MAIN

; Initialize register index pair 0 for RAM SRC command
LDM #1
XCH #0
LDM #12
XCH #1

; Initialize register index pair 1 for keyboard SRC command
; (keyboard connected to port on ROM 0)
CLB
XCH #2
XCH #3

; Loop to read digits from guess
:READ_GUESS

; Write character set to tape
LDM #1
WMP

; Wait for keyboard
:kb_wait1
JCN %0001 :kb_wait1

; Read digit
SRC #1
RDR ; Read from keyboard (port on ROM 0)
WMP ; Write to tape
SRC #0
WRM ; Write digit to RAM main memory character

; Keep looping until R1 is 0 (mod 16)
ISZ #1 :READ_GUESS
; End loop to read digits from guess

; Print newline
LDM #0
WMP
LDM #1
WMP

; Prepare for analysis
; At this point RAM register zero contains the solution and
; RAM register 1 contains the guess. Digits are stored in
; main memory characters 12-15. In the guess register,
; status characters 0, 1, and 2 are used for bull, cow, and
; sheep counts, respectively.

; Register map
; R0,R1: Index pair for SRCing guess digits
; R2,R3: Index pair for SRCing solution digits
; R4:    Copy of guess digit being analyzed in inner loop
; R5:    Count of matches for guess digit

; Clear bull, cow, and sheep counts
; (RAM register 1 already selected)
CLB
WR0
WR1
WR2

; Reset register index pairs
; (R0 already 0)
CLB
XCH #2
LDM #12
XCH #1
LDM #12
XCH #3

; Main loop to analyze guess (compute bulls, cows, and sheep)
:ANALYZE_GUESS

; Outer loop iterates through guess digits

; Copy guess digit to R4
SRC #0
RDM
XCH #4

; Reset solution digit index in R3 and match count in R5
LDM #12
XCH #3
CLB
XCH #5

; Inner loop iterates through solution digits
:BEGIN_ANALYSIS_INNER_LOOP

SRC #1 ; Set the solution digit to compare
LD  #4 ; Load guess digit to acc

; Compare digits
CLC
SBM

; No match. Skip to next iteration
JCN %1100 :CONTINUE_INNER_LOOP

; Match found
INC #5

; Is it a bull?
LD #1
CLC
SUB #3
JCN %1100 :CONTINUE_INNER_LOOP

; Bull found. Increment bull count and exit inner loop
SRC #0
RD0
IAC
WR0
JUN :CONTINUE_OUTER_LOOP

; Keep looping until R3 is 0 (mod 16)
:CONTINUE_INNER_LOOP
ISZ #3 :BEGIN_ANALYSIS_INNER_LOOP
; End inner loop

; Post-processing of inner loop. Digit is either a cow or
; sheep depending on if R5 > 0
SRC #0
LD #5
JCN %0100 :IS_A_SHEEP
RD1
IAC
WR1
JUN :CONTINUE_OUTER_LOOP

:IS_A_SHEEP
RD2
IAC
WR2

; Keep looping until R1 is 0 (mod 16)
:CONTINUE_OUTER_LOOP
ISZ #1 :ANALYZE_GUESS
; End outer loop

; Print the results before restarting main loop
:PRINT_BCS_RESULTS
LDM #1
WMP
RD0
WMP

LDM #1
WMP
RD1
WMP

LDM #1
WMP
RD2
WMP

; Print newline
LDM #0
WMP
LDM #1
WMP

JUN :MAIN
