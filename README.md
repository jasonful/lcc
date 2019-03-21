### What is ulpcc?
A C compiler targeting the ULP coprocessor on the ESP32 chip.  It is based on the lcc compiler written by David Hanson and Chris Fraser.  The code-generator for the ULP was written by Jason Fuller (me), and all bugs are mine.

### Installation
Currently only Windows is supported.  (Lcc is compilable under Linux, but I haven't taken the time to get it to work.)
1. set the ULPCCDIR environment variable to a directory of your choosing
1. Download [http://github.com/jasonful/lcc/ulpcc/release.zip](http://github.com/jasonful/lcc/ulpcc/release.zip) and extract to %ULPCCDIR%
1. set IDF_PATH to where you installed the ESP IDF
1. Add %ULPCCDIR%\bin to your PATH

### Usage
1. `ulpcc foo.c` will generate foo.S
1. Build foo.S as you normally would a hand-written .S file.
1. Globals in foo.c are exported with a ulp_ prefix just like .globals in a foo.S.

### Not supported:
1. Floating point
1. Signed arithmetic.  Despite what some parts of the documentation may imply, the ULP coprocessor has no support for signed arithmetic.  
This means you should always use `unsigned int` (or `unsigned`) instead of `int`.
1. Division, multiplication, mod
1. Function calls. This may seem like a large restriction, and it is, but:
	1. I did not want to sacrifice one of the four registers for a stack pointer.  
	1. Most ULP programs are relatively simple.
	1. If you find you need a function call, try using a macro, or gotos and globals like in the old BASIC days (yes, I'm that old).
	1. If someone makes a compelling case to implement function calls, I’m open to considering it.
1. Structures, pointers, and arrays.  The ULP coprocessor only lets you read the low 16 bits of a 32-bit word, and addresses are in units of 32-bits not bytes.  
This makes pointers, arrays, and structures problematic, and I would suggest avoiding them completely.  
Though, there are simple operations that will work, like taking and using the address of a single variable.  
Structures can also be padded with dummy fields to account for the unreadable high 16-bits.


### Pseudo-functions
You can call the following ULP instructions as if they were functions: `adc`, `halt`, `i2c_rd`, `i2c_wr`, `reg_rd`, `reg_wr`, `sleep`, `tsens`, `wait`, `wake`.  

Pseudo-functions that take parameters must take constants (not expressions) because the machine instructions take constants.

See ulp_c.h for details.

### Notes:
1. The compiler automatically inserts a `halt` at the end of entry().
1. The Stage_cnt register and its corresponding instructions are so idiosyncratic, I don't think they can be integrated with lcc.  So they are not used.  This shouldn’t hurt anything.
1. All data types are 2 bytes in size.
1. When something is not supported, I try to generate a friendly error message in the .S file.  But it’s possible you will hit a cryptic assert in lcc.
1. If you see generated assembly that could clearly be optimized, feel free to report it as a bug.
1. If you use the READ_RTC_REG family of macros, you will get warnings like: warning: overflow in constant `0x3ff48400'  This is because the value exceeds the size of a ULP register, but a subsequent subtraction in the macro will get the value down to 2 bytes, so the warning is harmless.
### Bugs
Report bugs to [https://github.com/jasonful/lcc/issues](https://github.com/jasonful/lcc/issues) 


