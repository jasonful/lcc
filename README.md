### What is ulpcc?
A C compiler targeting the ULP coprocessor on the ESP32 chip.  It is based on the lcc compiler written by David Hanson and Chris Fraser.  The code-generator for the ULP was written by Jason Fuller (me), and all bugs are mine.

### Installation
Currently only Windows is supported.  (Lcc is compilable under Linux, but I haven't taken the time to get it to work.)

1. Download [https://github.com/jasonful/lcc/raw/master/ulpcc.zip](https://github.com/jasonful/lcc/raw/master/ulpcc.zip) 
and extract to a directory of your choosing
1. set the ULPCCDIR environment variable to the directory you extracted to.
1. set IDF_PATH to where you installed the ESP IDF.  (If you do not have it installed on your Windows machine, 
install it from [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/windows-setup.html).)
1. set PATH=%PATH%;%ULPCCDIR%\bin

### Usage
1. `ulpcc foo.c` will generate foo.S
1. Build foo.S as you normally would a hand-written .S file.
1. Globals in foo.c are exported with a ulp_ prefix just like .globals in a foo.S.

### Build from code
1. `mkdir build`
1. `make all`

### Not supported:
1. Floating point
1. Signed arithmetic.  Despite what some parts of the documentation may imply, the ULP coprocessor has no support for signed arithmetic.  
This means you should always use `unsigned int` (or `unsigned`) instead of `int`.
1. Division, multiplication, mod
1. Function calls. Without a stack pointer, and only having 4 registers to play with, 
	implementing this will take some thought.
	For now, hopefully this won't be a problem since most ULP programs are pretty short.
1. Strings.  The ULP coprocessor cannot access individual bytes.

### Pseudo-functions
You can call the following ULP instructions as if they were functions: 
`adc`, `halt`, `i2c_rd`, `i2c_wr`, `reg_rd`, `reg_wr`, `sleep`, `tsens`, `wait`, `wake`.  

Pseudo-functions that take parameters must take constants (not expressions) because the 
machine instructions take constants.

See %ULPCCDIR%\inc\ulp_c.h for details.

### Notes:
1. The compiler automatically inserts a `halt` at the end of entry().
1. When something is not supported, I try to generate a friendly error message in the .S file. 
1. If you get an error like ` ulp_main.elf section '.text' will not fit in region 'ram'` you will 
need to increase the space reserved for ULP code by modifying CONFIG_ULP_COPROC_RESERVE_MEM in
your sdkconfig file.  I believe the max is 8192.

### Bugs
Report bugs to [https://github.com/jasonful/lcc/issues](https://github.com/jasonful/lcc/issues) 


