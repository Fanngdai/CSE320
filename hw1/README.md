# Homework 1 - CSE 320 - Fall 2018
#### Professor Eugene Stark

### **Due Date: Friday 09/21/2018 @ 11:59pm**

**Read the entire doc before you start**

## Introduction

In this assignment, you will write a command line utility to perform certain
transformations on audio files encoded in Sun audio (`.au`) format.
The goal of this homework is to familiarize yourself with C programming,
with a focus on input/output, strings in C, bitwise manipulations,
and the use of pointers.

You **MUST** write your helper functions in a file separate from `main.c`. The
`main.c` file **MUST ONLY** contain `#include`s, local `#define`s and the `main`
function. This is the only requirement for project structure. Beyond this, you
may have as many or as few additional `.c` files in the `src` directory as you
wish. Also, you may declare as many or as few headers as you wish. In this
document, we use `hw1.c` as our example file containing helper functions.

> :scream: Array indexing (**'A[]'**) is not allowed in this assignment. You
> **MUST USE** pointer arithmetic instead. All necessary arrays are declared in
> the `const.h` header file. You **MUST USE** these arrays. **DO NOT** create
> your own arrays. We **WILL** check for this.

# Getting Started

Fetch base code for `hw1` as described in `hw0`. You can find it at this link:
[https://gitlab02.cs.stonybrook.edu/cse320/hw1](https://gitlab02.cs.stonybrook.edu/cse320/hw1).

Both repos will probably have a file named `.gitlab-ci.yml` with different contents.
Simply merging these files will cause a merge conflict. To avoid this, we will
merge the repos using a flag so that the `.gitlab-ci.yml` found in the `hw1`
repo will be the file that is preserved.
To merge, use this command:

```
git merge -m "Merging HW1_CODE" HW1_CODE/master --strategy-option=theirs
```

Here is the structure of the base code:

<pre>
hw1
├── include
│   ├── audio.h
│   ├── const.h
│   ├── debug.h
│   ├── hw1.h
│   └── myrand.h
├── Makefile
├── rsrc
│   ├── sample.au
│   ├── sample_c_DeadBeef.au
│   ├── sample_d_f2.au
│   ├── sample_u_f2.au
│   ├── triangle_1kHz_2ch_8bit.au
│   └── triangle_1kHz_2ch_8bit_d_f10.au
├── src
│   ├── hw1.c
│   ├── main.c
│   └── myrand.c
└── tests
    └── hw1_tests.c
</pre>

> :nerd: Reference for pointers: [https://beej.us/guide/bgc/html/multi/pointers.html](https://beej.us/guide/bgc/html/multi/pointers.html).

> :nerd: Reference for command line arguments: [https://beej.us/guide/bgc/html/multi/morestuff.html#clargs](https://beej.us/guide/bgc/html/multi/morestuff.html#clargs).

**Note**: All commands from here on are assumed to be run from the `hw1` directory.

## A Note about Program Output

What a program does and does not print is VERY important.
In the UNIX world stringing together programs with piping and scripting is
commonplace. Although combining programs in this way is extremely powerful, it
means that each program must not print extraneous output. For example, you would
expect `ls` to output a list of files in a directory and nothing else.
Similarly, your program must follow the specifications for normal operation.
One part of our grading of this assignment will be to check whether your program
produces EXACTLY the specified output.  If your program produces output that deviates
from the specifications, even in a minor way, or if it produces extraneous output
that was not part of the specifications, it will adversely impact your grade
in a significant way, so pay close attention.

**Use the debug macro `debug` (described in the 320 reference document in the
Piazza resources section) for any other program output or messages you many need
while coding (e.g. debugging output).**

# Part 1: Program Operation and Argument Validation

In this part, you will write a function to validate the arguments passed to your
program via the command line. Your program will support the following flags:

- If no flags are provided, you will display the usage and return with an
`EXIT_FAILURE` return code

- If the `-h` flag is provided, you will display the usage for the program and
  exit with an `EXIT_SUCCESS` return code.

- If the `-u` flag is provided, you will perform "speed-up" conversion
  (aka "downsampling" or "decimation"); reading audio data from `stdin`
  and writing audio data to `stdout`.

- If the `-d` flag is provided, you will perform "slow_down" conversion
  (aka "upsampling" or "interpolation"); reading audio data from `stdin`
  and writing audio data to `stdout`.

- If the `-c` flag is provided, you will perform "crypt" conversion;
  reading audio data from `stdin` and writing audio data to `stdout`.

> If the `-h` flag is *not* specified, then exactly one of `-u`, `-d`, or `-c`
> must be specified.

> :nerd: `EXIT_SUCCESS` and `EXIT_FAILURE` are macros defined in `<stdlib.h>` which
> represent success and failure return codes respectively.

> :nerd: `stdin`, `stdout`, and `stderr` are special files that are opened upon
> execution for all programs and do not need to be reopened.

Some of these operations will also need other command line arguments which are
described in each part of the assignment. The three usages for this program are:

<pre>
Usage: bin/audible -h [any other number or type of arguments]
    -h       Help: displays this help menu
Usage: bin/audible -u|-d [-f FACTOR] [-p]
	-u       Speed Up: increase playback speed by deleting samples
    -d       Slow Down: decreate playback speed by inserting interpolated samples
             Optional additional parameters:
                -f           FACTOR is an integer factor for speed up or slow down
                -p           Preserve input annotation without modification.
Usage: bin/audible -c [-k KEY] [-p]
    -c       "(En/De)Crypt" the audio samples, using secret key KEY
             Required additional parameter:
                -k           KEY is a secret key
	         Optional additional parameter:
                -p           Preserve input annotation without modification.
</pre>

A valid invocation of the program implies that the following hold about
the command-line arguments:

- All positional arguments (`-h|-u|-d|-c`) come before any optional
arguments (`-f`, `-k`, and `-p`). The optional arguments may come in any order
after the positional ones.

- If the `-h` flag is provided, it is the first positional argument after
the program name.

- If an option requires a parameter, the corresponding parameter must be provided
(e.g. `-f` must always be followed by a FACTOR specification).

    - If `-f` is given, the FACTOR argument will be given as a decimal integer in
    the range [1, 1024].

    - If `-k` is given, the KEY argument must be a hexadecimal integer with at most 8
      digits.  Either upper-case letters (`'A'`-`'F'`) or lower-case letters (`'a'`-`'f'`)
      for the digits beyond 9.

> :scream: You may use only "raw" `argc` and `argv` for argument parsing and
> validation. Using any libraries that parse command line arguments (e.g.
> `getopt`) is prohibited.

> :scream: Any libraries that help you parse strings are prohibited as well
> (`string.h`, `ctype.h`, etc). *This is intentional and will help you
> practice parsing strings and manipulate pointers.*

> :scream: You **MAY NOT** use dynamic memory allocation in this assignment
> (i.e. `malloc`, `realloc`, `calloc`, `mmap`, etc)

For example, the following are a subset of the possible valid argument
combinations:

- `$ bin/audible -h ...`
- `$ bin/audible -u -f 2`
- `$ bin/audible -c -k DeadBeef`

Some examples of invalid combinations would be:

- `$ bin/audible -u -d -f 3`
- `$ bin/audible -f 3 -u`
- `$ bin/audible -c -k DeadBeef -f 3`

> :scream: The `...` means that all arguments, if any, are to be ignored; e.g.
> the usage `bin/audible -h -x -y D00D000 -z` is equivalent to `bin/audible -h`

**NOTE:** The `make` command compiles the `audible` executable into the `bin` folder.
Assume all commands in this doc are run from from the `hw1` directory of your
repo.

### **Required** Validate Arguments Function

In `const.h`, you will find the following function prototype (function
declaration) already declared for you. You **MUST** implement this function
as part of the assignment.

<pre>
/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 1 if validation succeeds and 0 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 1 if validation succeeds and 0 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv);
</pre>

> :scream: This function must be implemented as specified as it will be tested
> and graded independently. **It should always return -- the USAGE macro should
> never be called from validargs.**

The `validargs` function should return 0 if there is any form of failure.
This includes, but is not limited to:

- Invalid number of arguments (too few or too many)

- Invalid ordering of arguments

- A missing parameter to an option that requires one (e.g. `-c` with no
  KEY specification).

- Invalid `FACTOR` (if one is specified).  A `FACTOR` is invalid if it
contains characters other than the digits ('0'-'9), or if it denotes a
value not in the range [1, 1024].

- Invalid `KEY` (if one is specified).  A `KEY` is invalid if it is more than
eight characters long or if it contains characters other than digits in
the range '0' - '9', lower-case letters in the range `a` - `f`, or upper-case
letters in the range `A` - `F`

The `global_options` variable of type `unsigned long` is used to record the mode
of operation (i.e. speed up/slow down/crypt) of the program, as well as any
selected factor and secret key.  This is done as follows:

- If the `-h` flag is specified, the most significant bit (bit 63) is 1.

- The second-most-significant bit (bit 62) is 1 if `-u` is passed
(i.e. the user wants speed-up mode).

- The third-most-significant bit (bit 61) is 1 if `-d` is passed
(i.e. the user wants slow-down mode).

- The fourth-most-significant bit (bit 60) is 1 if `-c` is passed
(i.e. the user wants crypt mode).

- The fifth-most-significant bit (bit 59) is 1 if `-p` is passed
(i.e. the user wants annotations left unmodified).

- If the `-f` option was specified, then the factor (minus one) is recorded
in the seventh- through sixteenth-most-significant bits (bits 57 - 48)
If the `-f` option was not specified, then these bits of `global_options`
should all be zero (i.e. the default factor is 1).

- If the `-k` option was specified, then the secret key is recorded in the
thirty-two least-significant bits (bits 31 - 0).
If the `-k` option was not specified, then these bits of `global_options`
should all be zero.

If `validargs` returns 0 indicating failure, your program must call
`USAGE(program_name, return_code)` and return `EXIT_FAILURE`.
**Once again, `validargs` must always return, and therefore it must not
call the `USAGE(program_name, return_code)` macro itself.
That should be done in `main`.**

If `validargs` sets the most-significant bit of `global_options` to 1
(i.e. the `-h` flag was passed), your program must call `USAGE(program_name,
return_code)` and return `EXIT_SUCCESS`.

> :nerd: The `USAGE(program_name, return_code)` macro is already defined for you
> in `const.h`.

If validargs returns 1, then your program must read audio data from `stdin`,
transforming it as specified by the value of `global_options`, and writing the
result to `stdout`.  Upon successful completion, your program should exit
with exit status `EXIT_SUCCESS`; otherwise, in case of an error it should
exit with exit status `EXIT_FAILURE`.

If `-f` is provided, you must check to confirm that the specified factor
is valid.

If `-k` is provided, you must check that the specified key is valid.

> :nerd: Remember `EXIT_SUCCESS` and `EXIT_FAILURE` are defined in `<stdlib.h>`.
> Also note, `EXIT_SUCCESS` is 0 and `EXIT_FAILURE` is 1.

> :nerd: We suggest that you create functions for each of the operations defined
> in this document. Writing modular code will help you isolate and fix
> problems.

### Sample validargs Execution

The following are examples of `global_options` settings for given inputs.
Each input is a bash command that can be used to run the program.
In the examples, all don't care bits (bits 3-11, where the least significant
bit is numbered 0 and the most significant bit is numbered 31) have been set to 0.

- Input: `bin/audible -h`.  Setting: 0x8000000000000000 (`help` bit is set.
All other bits are don't cares.)

- Input: `bin/audible -u`.  Setting: 0x4000000000000000 (`speed up` bit is set;
`factor` bits are zero, representing `factor=1`).

- Input: `bin/audible -d -f 2`.  Setting: 0x2001000000000000 (`slow down` bit is set;
`factor` bits are 0x001, representing `factor=2`).

- Input: `bin/audible -c -k 5b5b5b5b`.  Setting: 0x100000005B5B5B5B (`crypt` bit is set;
`key` bits are 0x5b5b5b5b).

- Input: `bin/audible -k 0 -c`.  Setting: 0x0. This is an error
case because the specified argument ordering is invalid (`-k` is before `-c`).
In this case `validargs` returns 0, leaving `global_options` unset.

# Part 2: Digital Audio and the Sun Audio File Format

## Digital Audio

To understand the Sun audio file format and the transformations you
are to implement, you need to have some basic knowledge of how audio
is represented in digital form.  An analog audio signal is a continuous
*waveform*; *i.e.* a continuous function that maps
each point in an interval of real time to an instantaneous
*amplitude* which is a real number.  In order to represent
such a signal digitally, the interval of time is replaced by
a equally spaced sequence of *sampling points*,
and the audio signal is approximated by giving a *sample* of
its amplitude at each of the sampling points.  The original real
amplitude itself is digitized, and gets replaced by a value that can be
represented in a finite number of bits.
This sampling scheme is called *pulse-code modulation*, or PCM.

Typically, digital audio data will have multiple *channels*;
*e.g.* audio intended for stereo system will have two channels:
one for each speaker.  As the channels are independent, each channel
requires its own individual sample at each sampling point.
The collection of all the samples, one for each channel, at a particular
sampling point, is called a *frame*.  The number of frames per unit
time is called the *frame rate*.  For example, CD-quality audio signals
have a frame rate of 44,100 frames per second (44.1KHz).
Each sample is represented by 16 bits, and there are two samples per frame,
representing the amplitude of the left and right channels of a stereo signal.
Thus, each frame in a CD-quality audio signal consists of a total
of four bytes (32 bits) of data (only the amplitudes need be
represented explicitly, because the time interval between the samples
is always the same).  It would thus require (10)(44,100)(4) = 1,764,000 bytes
to represent a 10-second clip of such a signal.

When digital audio signals are played, either on the computer or
on a stereo system, the digital data is read from the storage medium
and fed on demand to the audio output device.  At regularly spaced
intervals corresponding to the frame rate of the signal, the audio
output device takes each frame and presents the samples to
*digital-to-analog converters*, one for each output channel,
which convert the digital amplitude samples into voltage levels
on output wires.  These voltage levels are then amplified and used
to drive speakers.  Since each sample determines the output voltage
on one channel over an entire sampling interval whose duration is
the reciprocal of the sampling rate (1/44,100Hz = 22.68 microseconds
for CD audio), if you were to look closely, say, using an oscilloscope,
at the shape of the final waveform coming out of a digital audio
system, you would see that it is not smooth but has a "step" shape
(see figure below, which shows two cycles of a sine wave that has
been sampled at a rate of 10 samples per cycle).
However, these steps occur rapidly enough that your ear cannot
tell the difference between this signal and a smoothly varying one.

<img src="sampled_sine_wave.png">

In the digital representation of audio signals, there are several
parameters that can be changed according to the intended application
and details of the computer system on which the signals will be
processed.  The most basic parameter is the frame rate.
A higher frame rate yields a higher-quality representation of the
audio signal, but requires a larger number of bytes to represent
the signal.  In general, the highest frequency that can be represented
by a digital audio signal is equal to one-half the frame rate
(this is called the <em>Nyquist frequency</em>).
For example, CD audio with a frame rate of 44,100Hz can represent
audio frequencies up to 22,050Hz.  This is a higher frequency than
your ear can detect, so when played back the original sound is
accurately reproduced as far as your ear is concerned.
On the other hand, for other applications like telephony for which
high-fidelity reproduction is not as important, lower frame rates
are used.  For example, 8000Hz is a common choice for voice signals,
because the Nyquist frequency of 4000Hz is high enough to cover
the voice frequencies that are necessary for speech understanding.

Another important parameter in the representation of digital
audio signals is the number of bits per sample.
Once again, there is a tradeoff between the fidelity of the
representation and the amount of data required.
As mentioned above, CD-quality audio uses 16 bits (or two bytes)
per sample.  These 16 bits are used to represent
(using two's complement encoding) a signed amplitude in the
range [-32768, 32767].  This is enough that
it is generally hard for your ear to detect the fact that the
signal does not have a smoothly varying amplitude but rather
"jumps" from one discrete value to the next.
For applications that are not as demanding, 8-bit (one byte)
samples are sometimes used.
For multi-byte samples, a choice exists as to the order in
which the individual bytes in a sample appear in the data stream.
If the bytes of a sample occur starting with the the least significant
byte and ending with the most significant byte the representation is
said to be *little-endian*.
The opposite ordering is said to be *big-endian*.
These two choices do not affect the fidelity of the represented
signal -- they are more or less arbitrary variations that have to do
with the way the designers of a particular computer system chose
to store multi-byte quantities in memory.

One other variation in the way audio signals are represented digitally
is the way in which samples are encoded in a fixed number of bits.
The particular encoding scheme used in CD audio is called
*linear* encoding, or more precisely, *signed linear* encoding.
In *unsigned linear* encoding, the bits in a sample
are used to represent an *unsigned* amplitude, rather than a
signed amplitude.  In this case, a 16-bit sample would represent an
amplitude in the range [0, 65535] instead of [-32768, 32767].
Although the choice between signed and unsigned encoding affects
the details of signal processing algorithms, it does not affect
the fidelity of the signal that is ultimately played back, because
the "DC level" of a signal is inaudible and is generally filtered
out by the amplifier and speakers.
A disadvantage of linear encoding is the limited dynamic range
(the range of "loudness") that can be represented.
Other encodings exist that map the amplitudes logarithmically,
rather than linearly, to the sample bits.

To summarize the above, the representation scheme used to encode
data on audio CDs is described as
*44.1KHz, 16-bit, two-channel (i.e. stereo), signed linear PCM encoding*.
It may be *little-endian* or *big-endian*, depending on the
computer system.

## The `.au` audio file format

The `.au` audio file format was originally introduced by Sun Microsystems.
It is a relatively simple format that is commonly supported on many systems,
especially on Unix and Linux systems.
The overall structure of a `.au` can be depicted as follows:

<pre>
  +------------------+--------------------------+-------+     +-------+
  + Header (24 bytes)| Annotation (optional) \0 | Frame | ... | Frame |
  +------------------+--------------------------+-------+     +-------+
                                                ^
  &lt;----------- data offset --------------------&gt;(8-byte boundary)
</pre>

The file begins with a 24-byte *header* consisting of six unsigned 32-bit
words.  These are presented in *big-endian* format, as is all other
multibyte data in a `.au` file.  The meanings of the header fields are
as follows:

- The first field in the header is the "magic number", which is always
  equal to 0x2e736e64.  This value represents the four ASCII characters
  ".snd".

- The second field in the header is the "data offset", which is the
  number of bytes from the beginning of the file that the audio sample
  data begins.  This value must be divisible by 8.  The minimum value
  is 24 (if there is no annotation field).  The minimum value if an
  annotation field exists is 32.

- The third field in the header is the "data size", which is the number
  of bytes of audio sample data.  The value 0xffffffff indicates that
  the size is unknown (this could occur, for example, if the format was
  used to transmit an audio stream of indefinite duration).

- The fourth field in the header specifies the encoding used for the
  audio samples.  We will only support the following values, which
  have the meanings shown.

    - 2  (specifies 8-bit linear PCM encoding)
    - 3  (specifies 16-bit linear PCM encoding)
    - 4  (specifies 24-bit linear PCM encoding)
    - 5  (specifies 32-bit linear PCM encoding)

- The fifth field in the header specifies the "sample rate", which is the
  number of frames per second.

- The sixth field in the header specifies the number of audio channels.
  We will only support 1 (*i.e.* mono) and 2 (*i.e.* stereo).

The header format can be defined by the following C code:

> <pre>
> #define AUDIO_MAGIC (0x2e736e64)
> 
> #define PCM8_ENCODING (2)
> #define PCM16_ENCODING (3)
> #define PCM24_ENCODING (4)
> #define PCM32_ENCODING (5)
> 
> typedef struct audio_header {
>     unsigned int magic_number;
>     unsigned int data_offset;
>     unsigned int data_size;
>     unsigned int encoding;
>     unsigned int sample_rate;
>     unsigned int channels;
> } AUDIO_HEADER;
> </pre>

Following the header is an optional "annotation field", which can be used
to store additional information (metadata) in the audio file.
The length of this field must be a multiple of eight bytes and it must be
terminated with at least one null ('\0') byte, but its format is otherwise
undefined.  We will impose a maximum size `ANNOTATION_MAX` on the annotation
field.  When reading an audio file, if the size of the annotation
(i.e. `input_header.data_offset - sizeof(AUDIO_HEADER)`) exceeds this maximum,
you should treat it as an error.  Similarly, if you would be required to write
an annotation area that exceeds the maximum size, then this should also be
treated as an error.

Audio data begins on an eight-byte boundary immediately following the
annotation field (or immediately following the header, if there is no
annotation field).  The audio data occurs as a sequence of *frames*,
where each frame contains data for one sample on each of the audio channels.
The size of a frame therefore depends both on the sample encoding and on
the number of channels.  For example, if the sample encoding is 16-bit PCM
(i.e. two bytes per sample) and the number of channels is two, then the number
of bytes in a frame will be 2 * 2 = 4.  If the sample encoding is 32-bit PCM
(i.e. four bytes per sample) and the number of channels is two, then the number
of bytes in a frame will be 2 * 4 = 8.  We will consider a partial frame at
the end of the file to be an error.  If the header specifies the data size,
then this size must be a multiple of the frame size, and there must be at least
that many bytes of audio data in the file; if not, we consider it as an error.
Extra data beyond the specified data size is not regarded as an error,
but any such data is to be ignored as if it were not present.

Within a frame, the sample data for each channel occurs in sequence.
For example, in case of 16-bit PCM encoded stereo, the first two bytes of each
frame represents a single sample for channel 0 and the second two bytes
represents a single sample for channel 1.  Samples are signed values encoded
in two's-complement and are presented in big-endian (most significant byte first)
byte order.

<pre>
 +-----------------------------------+-----------------------------------+
 | Sample 0 MSB | ... | Sample 0 LSB | Sample 1 MSB | ... | Sample 1 LSB |
 +-----------------------------------+-----------------------------------+
</pre>

# Part 3: **Required** Functions

In order to provide some additional structure for you, as well as to make it
possible for us to perform additional unit tests on your program,
you are required to implement the following functions as part of your program.
The prototypes for these functions are given in `const.h`.
Once again, you **MUST** implement these functions as part of the assignment,
as we will be testing them separately.

> <pre>
> /**
>  * @brief  Recodes a Sun audio (.au) format audio stream, reading the stream
>  * from standard input and writing the recoded stream to standard output.
>  * @details  This function reads a sequence of bytes from the standard
>  * input and interprets it as digital audio according to the Sun audio
>  * (.au) format.  A selected transformation (determined by the global variable
>  * "global_options") is applied to the audio stream and the transformed stream
>  * is written to the standard output, again according to Sun audio format.
>  * Besides the transformation applied to the audio sample data, unless the -p
>  * option was specified, the annotation data is transformed by prepending a
>  * representation of the command-line arguments given to "audible".
>  * If the transformed annotation would be longer than ANNOTATION_MAX,
>  * it shall be considered an error and this function shall return 0 without
>  * producing any output.  If the -p option was specified, then no transformation
>  * is performed on the annotation, and the output annotation is identical to
>  * the input annotation.
>  *
>  * @param  argv  Command-line arguments, for constructing modified annotation.
>  * @return 1 if the recoding completed successfully, 0 otherwise.
>  */
> int recode(char **argv);
> </pre>

The annotation for the output file is constructed by prepending to the
input annotation a representation of the command-line arguments given
to "audible".  This representation is produced by concatenating the
strings from the "argv" array, separating successive arguments by a
single space (' ') character, and terminating the result with a single
newline ('\n') character.  If the input audio file had no annotation field,
then the output file will have an annotation that consists only of the
audible command line, followed by at least one null ('\0') byte.
 If the input file did have a nonempty annotation field, then the annotation
field of the output file will consist of the ('\n'-terminated) audible
command line, immediately followed by the original annotation from the
input file, without any intervening null byte.  In all cases, the output
annotation shall conform to the Sun audio file format specifications.

> <pre>
> /**
>  * @brief Read the header of a Sun audio file and check it for validity.
>  * @details  This function reads 24 bytes of data from the standard input and
>  * interprets it as the header of a Sun audio file.  The data is decoded into
>  * six unsigned int values, assuming big-endian byte order.   The decoded values
>  * are stored into the AUDIO_HEADER structure pointed at by hp.
>  * The header is then checked for validity, which means:  no error occurred
>  * while reading the header data, the magic number is valid, the data offset
>  * is a multiple of 8, the value of encoding field is one of {2, 3, 4, 5},
>  * and the value of the channels field is one of {1, 2}.
>  *
>  * @param hp  A pointer to the AUDIO_HEADER structure that is to receive
>  * the data.
>  * @return  1 if a valid header was read, otherwise 0.
>  */
> int read_header(AUDIO_HEADER *hp);
> </pre>

> <pre>
> /**
>  * @brief  Write the header of a Sun audio file to the standard output.
>  * @details  This function takes the pointer to the AUDIO_HEADER structure passed
>  * as an argument, encodes this header into 24 bytes of data according to the Sun
>  * audio file format specifications, and writes this data to the standard output.
>  *
>  * @param  hp  A pointer to the AUDIO_HEADER structure that is to be output.
>  * @return  1 if the function is successful at writing the data; otherwise 0.
>  */
> int write_header(AUDIO_HEADER *hp);
> </pre>

> <pre>
> /**
>  * @brief  Read annotation data for a Sun audio file from the standard input,
>  * storing the contents in a specified buffer.
>  * @details  This function takes a pointer 'ap' to a buffer capable of holding at
>  * least 'size' characters, and it reads 'size' characters from the standard input,
>  * storing the characters read in the specified buffer.  It is checked that the
>  * data read is terminated by at least one null ('\0') byte.
>  *
>  * @param  ap  A pointer to the buffer that is to receive the annotation data.
>  * @param  size  The number of bytes of data to be read.
>  * @return  1 if 'size' bytes of valid annotation data were successfully read;
>  * otherwise 0.
>  */
> int read_annotation(char *ap, unsigned int size);
> </pre>

> <pre>
> /**
>  * @brief  Write annotation data for a Sun audio file to the standard output.
>  * @details  This function takes a pointer 'ap' to a buffer containing 'size'
>  * characters, and it writes 'size' characters from that buffer to the standard
>  * output.
>  *
>  * @param  ap  A pointer to the buffer containing the annotation data to be
>  * written.
>  * @param  size  The number of bytes of data to be written.
>  * @return  1 if 'size' bytes of data were successfully written; otherwise 0.
>  */
> int write_annotation(char *ap, unsigned int size);
> </pre>

> <pre>
> /**
>  * @brief Read, from the standard input, a single frame of audio data having
>  * a specified number of channels and bytes per sample.
>  * @details  This function takes a pointer 'fp' to a buffer having sufficient
>  * space to hold 'channels' values of type 'int', it reads
>  * 'channels * bytes_per_sample' data bytes from the standard input,
>  * interpreting each successive set of 'bytes_per_sample' data bytes as
>  * the big-endian representation of a signed integer sample value, and it
>  * stores the decoded sample values into the specified buffer.
>  *
>  * @param  fp  A pointer to the buffer that is to receive the decoded sample
>  * values.
>  * @param  channels  The number of channels.
>  * @param  bytes_per_sample  The number of bytes per sample.
>  * @return  1 if a complete frame was read without error; otherwise 0.
>  */
> int read_frame(int *fp, int channels, int bytes_per_sample);
> </pre>

> <pre>
> /**
>  * @brief  Write, to the standard output, a single frame of audio data having
>  * a specified number of channels and bytes per sample.
>  * @details  This function takes a pointer 'fp' to a buffer that contains
>  * 'channels' values of type 'int', and it writes these data values to the
>  * standard output using big-endian byte order, resulting in a total of
>  * 'channels * bytes_per_sample' data bytes written.
>  *
>  * @param  fp  A pointer to the buffer that contains the sample values to
>  * be written.
>  * @param  channels  The number of channels.
>  * @param  bytes_per_sample  The number of bytes per sample.
>  * @return  1 if the complete frame was written without error; otherwise 0.
>  */
> int write_frame(int *fp, int channels, int bytes_per_sample);
> </pre>

# Part 4: Transformations on the Audio Data

## "Speed Up" (Decimation)

If the "speed-up" transformation is selected (option `-u`), with a `FACTOR` equal
to `N`, then the output audio data is obtained from the input audio data simply
by retaining only every `N`th frame.  Specifically, if the input frames are numbered
0, 1, 2, ..., then the frames numbered 0, N, 2N, ... are passed through to the
output and the intervening frames are discarded.  This is a lossy transformation,
as it discards information from the original file.

## "Slow Down" (Interpolation)

If the "slow-down" transformation is selected (option `-d`), with a `FACTOR` equal
to `N`, then the output audio data is obtained from the input audio data by
using linear interpolation to insert `N-1` additional frames between each two
frames of the input.  Specifically, if the input frames are numbered 0, 1, 2, ...,
then these frames will occur in the output as frames 0, N, 2N, ..., and
for `i` equal to 0, 1, 2, ..., the frames `i*N+1`, `i*N+2`, ..., `i*N+(N-1)`
have sample values intermediate between the sample values in the frame `i*N`
and those in the frame `(i+1)*N`, in the sense that if S and T are
the sample values for channel C in frames `i*N` and `(i+1)*N`, then the
sample value for channel C in frame `i*N+k` is given by the formula
`S + (T - S)*k/N`.  This amounts to drawing a straight line between two sample
value in the original input and inserting `N-1` new sample values at evenly
spaced intervals along this line.

Note that in the "slow down" transformation, new frames are only interpolated
between frames that exist in the original input; in particular, the first frame
and the last frame of the output will always be frames that existed in the input.

## "Crypt" (Encryption/Decryption)

If the "crypt" transformation is selected (option `-c`), with a `KEY` equal
to the 32-bit value `K`, then samples in the output audio data are obtained
by performing a bitwise exclusive-OR (XOR) of each sample value in
the input audio data with successive 32-bit values obtained from a pseudorandom
number generator (PRNG) that has been initialized with `K` as its seed.
This transformation obfuscates the original audio so that if the result is
played through the computer speakers, it will sound like "white noise".
Due to the properties of the exclusive-OR operation, if the obfuscated data
is subjected again to the same transformation, using the same PRNG
initialized with the same seed, the original audio data will be
recovered exactly.  Note that we are only applying the XOR operator to the
audio sample data; the contents of the header and any annotation field are
left unencrypted.

To ensure that audio data encrypted on one system can be decrypted on another
system, we need to build the PRNG into our program, rather than relying on a
library version.  The file `myrand.c` provided with the base code contains the
PRNG you are to use.  It provides two functions: `mysrand()`, which is used
to set the seed, and `myrand32()` which when called returns the next 32-bit value
from the pseudorandom sequence.  The `myrand32()` should be called exactly once
for each successive sample value in the input data.


# Part 5: Running the Completed Program

In any of its operating modes, the `audible` program reads from `stdin` and writes
to `stdout`.  As the input and output of the program is binary data, it will not
be useful to enter input directly from the terminal or display output directly to
the terminal.  Instead, the program can be run using *input and output redirection*
as in the following example:

> <pre>
> $ bin/audible -u -f 2 < rsrc/sample.au > out.au
> $ echo $?
> 0
> </pre>

This will cause the input to the program to be redirected from the file
`rsrc/sample.au` and the output from the program to be redirected to the file
`out.au`.

> :nerd: The `>` symbol tells the shell to perform "output redirection":
> the file `hw1.out` is created (or truncated if it already existed -- be careful!)
> and the output produced by the program is sent to that file instead
> of to the terminal.

> :nerd: `$?` is an environment variable in bash which holds the return code of
> the previous program run.  In the above, the `echo` command is used to display
> the value of this variable.

For debugging purposes, the contents of `out.au` can be viewed using the
`od` ("octal dump") command:

> <pre>
> $ od -t x1 out.au
> 0000000 2e 73 6e 64 00 00 00 30 00 01 12 f4 00 00 00 03
> 0000020 00 00 1f 40 00 00 00 02 62 69 6e 2f 61 75 64 69
> 0000040 62 6c 65 20 2d 75 20 2d 66 20 32 0a 00 00 00 00
> 0000060 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
> *
> 0000720 00 00 00 00 00 00 00 00 00 00 00 00 01 00 01 00
> 0000740 02 00 02 00 fc 00 fc 00 00 00 00 00 f9 00 f9 00
> (etc.)
> </pre>

In this case, you can see that the file data starts with the sequence
`2e`, `73`, `6e`, `64`, which is the magic number `0x2e736e64` in big-endian form.
The values in the first column indicate the offsets from the beginning of the file,
specified as 7-digit octal (base 8) numbers.

> :nerd: The `-t x1` flag instructs `od` to interpret the file as a sequence of
> individual bytes (that is the meaning of the "`1`" in "`x1`"), which are printed as
> hexadecimal values (that is the meaning of the "`x`" in "`x1`").  The `od` program has
> many options for setting the output format; another useful version is `od -bc`,
> which shows individual bytes of data as both ASCII characters and their octal codes.
> Refer to the "man" page for `od` for other possiblities.

Actually, if you were to try the above command with `out.au` produced
as shown, there would be so much output that the first few lines would be
lost off of the top of the screen.  To avoid this, you can *pipe* the output to
a program called `less`:

> <pre>
> $ od -t x1 out.au | less
> </pre>

This will display only the first screenful of the output and give you the
ability to scan forward and backward to see different parts of the output.
Type `h` at the `less` prompt to get help information on what you can do
with it.  Type `q` at the prompt to exit `less`.

Alternatively, the output of the program can be redirected via a pipe to
the `od` command, without using any output file:

> <pre>
> $ bin/audible -u -f 2 < rsrc/sample.au | od -t x1 | less
> </pre>

You can also arrange for a valid Sun audio file to be played through
the computer speakers using (among other choices) the `paplay` command:

> <pre>
> $ paplay rsrc/sample.au
> </pre>

Once your program starts producing valid output files, you can play
them to find out quickly whether the results are as you would expect
them to be.  This can also be done using pipes; for example:

> <pre>
> $ bin/audible -u -f 2 < rsrc/sample.au | paplay
> </pre>

Finally, pipes can be used to compose a sequence of transformations performed
by `audible`.  For example, the following command will play the sample audio
at a speed 1.5x faster than the original:

> <pre>
> $ cat rsrc/sample.au | bin/audible -d -f 2 | bin/audible -u -f 3 | paplay
> </pre>

> :nerd: Note that here we have performed the information-preserving "slow down"
> (interpolation) transformation first, followed by the lossy "speed up" (decimation)
> transformation.  This results in minimal loss of audio quality.
> If you perform the transformations in the opposite order, you still get
> a speed up by a factor of 1.5, but there is a very noticable loss of audio quality.

## Testing Your Program

In testing your program, it is useful to be able to compare two files
to see if they have the same content.  The `diff` command (use `man diff`
to read the manual page) is useful for comparison of text files, but not particularly
useful for comparing binary files such as audio files.
However the `cmp` command can be used to perform a byte-by-byte comparison of two files,
regardless of their content:

> <pre>
> $ cmp file1 file2
> </pre>

If the files have identical content, `cmp` exits silently.
If one file is shorter than the other, but the content is otherwise identical,
`cmp` will report that it has reached `EOF` on the shorter file.
Finally, if the files disagree at some point, `cmp` will report the
offset of the first byte at which the files disagree.
If the `-l` flag is given, `cmp` will report all disagreements between the
two files.

We can take this a step further and run an entire test without using any files:

> <pre>
> $ cmp -l &lt;(cat rsrc/sample.au) &lt;(cat rsrc/sample.au | bin/audible -d -f 2 -p | bin/audible -u -f 2 -p)
> $ echo $?
> 0
> </pre>

This compares the original file `sample.au` with the result of taking that
file and first performing a slow-down transformation with a factor of 2 and
then a speed-up transformation, also with a factor of 2.

> :nerd: `<(...)` is known as process substitution. It is allows the output of the
> program(s) inside the parentheses to appear as a file for the outer program.

> :nerd: `cat` is a command that outputs a file to `stdout`.

Because both files are identical, `cmp` outputs nothing.
(Note that had we not specified the `-p` flag, the files would have differed
in their annotation fields.)

## Unit Testing

Unit testing is a part of the development process in which small testable
sections of a program (units) are tested individually to ensure that they are
all functioning properly. This is a very common practice in industry and is
often a requested skill by companies hiring graduates.

> :nerd: Some developers consider testing to be so important that they use a
> work flow called test driven development. In TDD, requirements are turned into
> failing unit tests. The goal is then to write code to make these tests pass.

This semester, we will be using a C unit testing framework called
[Criterion](https://github.com/Snaipe/Criterion), which will give you some
exposure to unit testing. We have provided a basic set of test cases for this
assignment.

The provided tests are in the `tests/hw1_tests.c` file. These tests do the
following:

- `validargs_help_test` ensures that `validargs` sets the help bit
correctly when the `-h` flag is passed in.

- `validargs_speedup_test` ensures that `validargs` sets the speed-up bit
correctly when the `-u` flag is passed in.

- `help_system_test` uses the `system` syscall to execute your program through
Bash and checks to see that your program returns with `EXIT_SUCCESS`.

### Compiling and Running Tests

When you compile your program with `make`, an `audible_tests` executable will be
created in your `bin` directory alongside the `audible` executable. Running this
executable from the `hw1` directory with the command `bin/audible_tests` will run
the unit tests described above and print the test outputs to `stdout`. To obtain
more information about each test run, you can use the verbose print option:
`bin/audible_tests --verbose=0`.

The tests we have provided are very minimal and are meant as a starting point
for you to learn about Criterion, not to fully test your homework. You may write
your own additional tests in `tests/hw1_tests.c`. However, this is not required
for this assignment. Criterion documentation for writing your own tests can be
found [here](http://criterion.readthedocs.io/en/master/).

## Sample Audio Files

To get you started testing your program, a few sample files have been provided
in the `rsrc` directory:

- [sample.au](rsrc/sample.au)
  A short sample audio clip from the
  Wikipedia [Au file format](https://en.wikipedia.org/wiki/Au_file_format) page.
  It has a sample rate of 8000Hz, two channels, and uses 16-bit PCM encoding.
- [sample_d_f2.au](rsrc/sample_d_f2.au)
  This is the output of running `audible -d -f 2` on `sample.au`.
- [sample_u_f2.au](rsrc/sample_u_f2.au)
  This is the output of running `audible -u -f 2` on `sample.au`.
- [sample_c_DeadBeef.au](rsrc/sample_c_DeadBeef.au)
  This is the output of running `audible -c -k DeadBeef` on `sample.au`.
- [triangle_1kHz_2ch_8bit.au](rsrc/triangle_1kHz_2ch_8bit.au)
  This artificially generated file consists of 10 frames of 2 channels,
  using 8-bit PCM encoding and a 1000Hz sample rate.
  The samples on one channel alternate 127, -127, 127, ..., and the samples
  on the other channel alternate -127, 127, -127, ... .
  When viewed as a waveform using the "Audacity" audio tool it looks like
  a (degenerate) "triangle wave", as shown in the screenshot below:

  <img src="triangle_orig.png">
  
- [triangle_1kHz_2ch_8bit_d_f10.au](rsrc/triangle_1kHz_2ch_8bit_d_f10.au)
  This is the result of running `audible -d -f 10` on
  `triangle_1kHz_2ch_8bit.au`.  In the screenshot below you can see how new
  samples have been interpolated between those in the original file,
  resulting in a less degenerate triangle waveform.
  This file has been provided to help you understand the interpolation
  process.

  <img src="triangle_x10.png">

There is a free program called `sox` that can perform many different kinds
of audio format conversions, which you can use to create your own test files.
You can install it using `sudo apt install sox`.


# Hand-in instructions
**TEST YOUR PROGRAM VIGOROUSLY!**

Make sure your directory tree looks like this (possibly with additional
files that you added) and that your homework compiles (you should be sure
to try compiling with both `make clean all` and `make clean debug`
because there are certain errors that can occur one way but not the other).

<pre>
hw1
├── include
│   ├── audio.h
│   ├── const.h
│   ├── debug.h
│   ├── hw1.h
│   └── myrand.h
├── Makefile
├── rsrc
│   ├── sample.au
│   ├── sample_c_DeadBeef.au
│   ├── sample_d_f2.au
│   ├── sample_u_f2.au
│   ├── triangle_1kHz_2ch_8bit.au
│   └── triangle_1kHz_2ch_8bit_d_f10.au
├── src
│   ├── hw1.c
│   ├── main.c
│   └── myrand.c
└── tests
    └── hw1_tests.c
</pre>

This homework's tag is: `hw1`

`$ git submit hw1`

> :nerd: When writing your program try to comment as much as possible. Try to
> stay consistent with your formatting. It is much easier for your TA and the
> professor to help you if we can figure out what your code does quickly!

