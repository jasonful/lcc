### What is ulpcc?
A C compiler targeting the ULP coprocessor on the ESP32 chip.  It is based on the lcc compiler written by David Hanson and Chris Fraser.  The code-generator for the ULP was written by Jason Fuller (me), and all bugs are mine.
### Installation
Currently only Windows is supported.  Lcc is compilable under Linux, but it didn’t “just work” and I didn’t spend more time on it. Instructions
1. set the ULPCCDIR environment variable to a directory of your choosing
1. Download http://github.com/jasonful/lcc/ulpcc/release.zip and extract to %ULPCCDIR%
1. set IDF_PATH to where you installed the ESP IDF
### Usage
1. “ulpcc foo.c” will generate foo.S
1. Build foo.S as you normally would a hand-written .S file.
1. Globals are exported with a ulp_ prefix as you would expect.
4. Not supported:
a. Floating point
b. Function calls. This may seem like a large restriction, and it is, but:
i. I did not want to sacrifice one of the four registers for a stack pointer.  
ii. Most ULP programs are relatively simple.
iii. If you find you need a function call, try using a macro, or gotos and globals like the old BASIC days (yes, I'm that old).
iv. If someone makes a compelling case to implement function calls, I’m open to considering it.
c. The ULP coprocessor does its arithmetic operations unsigned
d. Division, multiplication, mod
5. Supported:
a. Pseudo-functions
i. You can call the following ULP instructions as if they were functions: adc, halt, i2c_rd, i2c_wr, reg_rd, reg_wr, sleep, tsens, wait, wake.  Pseudo-functions that take parameters must take constants (not expressions).
   See ulp_c.h for details.
6. Notes:
a. The Stage_cnt register and its corresponding instructions are so idiosyncratic, I don't think they can be integrated with lcc.  So they are not used.  This shouldn’t hurt anything.
b. All data types are 2 bytes in size.
c. When something is not supported, I try to generate a friendly error message in the .S file.  But it’s possible you will hit a cryptic assert in lcc.
d. If you see generated assembly that could clearly be optimized, feel free to report it as a bug.
e. If you use the READ_RTC_REG family of macros, you will get warnings like: warning: overflow in constant `0x3ff48400'  This is because the value exceeds the size of a ULP register, but a subsequent subtraction in the macro will get the value down to 2 bytes, so the warning is harmless.
7. Bugs
a. Report bugs to https://github.com/jasonful/lcc/issues 
