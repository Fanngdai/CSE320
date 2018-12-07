# Homework 4 Printer Spooler - CSE 320 - Fall 2018
#### Professor Eugene Stark

### **Due Date: Friday 11/16/2018 @ 11:59pm**

## Introduction

The goal of this assignment is to become familiar with low-level Unix/POSIX system
calls related to processes, signal handling, files, and I/O redirection.
You will implement a printer spooler program, called `imprimer`, that accepts user
requests to queue files for printing, cancel printing requests, pause and resume
print jobs, show the status of printers and print jobs, and set up pipelines to
convert queued files of one type to the type of file accepted by an available printer.

### Takeaways

After completing this assignment, you should:

* Understand process execution: forking, executing, and reaping.
* Understand signal handling.
* Understand the use of "dup" to perform I/O redirection.
* Have a more advanced understanding of Unix commands and the command line.
* Have gained experience with C libraries and system calls.
* Have enhanced your C programming abilities.

## Hints and Tips

* We **strongly recommend** that you check the return codes of **all** system calls
  and library functions.  This will help you catch errors.
* **BEAT UP YOUR OWN CODE!** Use a "monkey at a typewriter" approach to testing it
  and make sure that no sequence of operations, no matter how ridiculous it may
  seem, can crash the program.
* Your code should **NEVER** crash, and we will deduct points every time your
  program crashes during grading.  Especially make sure that you have avoided
  race conditions involving process termination and reaping that might result
  in "flaky" behavior.  If you notice odd behavior you don't understand:
  **INVESTIGATE**.
* You should use the `debug` macro provided to you in the base code.
  That way, when your program is compiled without `-DDEBUG`, all of your debugging
  output will vanish, preventing you from losing points due to superfluous output.

> :nerd: When writing your program, try to comment as much as possible and stay
> consistent with code formatting.  Keep your code organized, and don't be afraid
> to introduce new source files if/when appropriate.

### Reading Man Pages

This assignment will involve the use of many system calls and library functions
that you probably haven't used before.
As such, it is imperative that you become comfortable looking up function
specifications using the `man` command.

The `man` command stands for "manual" and takes the name of a function or command
(programs) as an argument.
For example, if I didn't know how the `fork(2)` system call worked, I would type
`man fork` into my terminal.
This would bring up the manual for the `fork(2)` system call.

> :nerd: Navigating through a man page once it is open can be weird if you're not
> familiar with these types of applications.
> To scroll up and down, you simply use the **up arrow key** and **down arrow key**
> or **j** and **k**, respectively.
> To exit the page, simply type **q**.
> That having been said, long `man` pages may look like a wall of text.
> So it's useful to be able to search through a page.
> This can be done by typing the **/** key, followed by your search phrase,
> and then hitting **enter**.
> Note that man pages are displayed with a program known as `less`.
> For more information about navigating the `man` pages with `less`,
> run `man less` in your terminal.

Now, you may have noticed the `2` in `fork(2)`.
This indicates the section in which the `man` page for `fork(2)` resides.
Here is a list of the `man` page sections and what they are for.

| Section          | Contents                                |
| ----------------:|:--------------------------------------- |
| 1                | User Commands (Programs)                |
| 2                | System Calls                            |
| 3                | C Library Functions                     |
| 4                | Devices and Special Files               |
| 5                | File Formats and Conventions            |
| 6                | Games et. al                            |
| 7                | Miscellanea                             |
| 8                | System Administration Tools and Daemons |

From the table above, we can see that `fork(2)` belongs to the system call section
of the `man` pages.
This is important because there are functions like `printf` which have multiple
entries in different sections of the `man` pages.
If you type `man printf` into your terminal, the `man` program will start looking
for that name starting from section 1.
If it can't find it, it'll go to section 2, then section 3 and so on.
However, there is actually a Bash user command called `printf`, so instead of getting
the `man` page for the `printf(3)` function which is located in `stdio.h`,
we get the `man` page for the Bash user command `printf(1)`.
If you specifically wanted the function from section 3 of the `man` pages,
you would enter `man 3 printf` into your terminal.

> :scream: Remember this: **`man` pages are your bread and butter**.
> Without them, you will have a very difficult time with this assignment.

## Getting Started

Fetch and merge the base code for `hw4` as described in `hw0`.
You can find it at this link: https://gitlab02.cs.stonybrook.edu/cse320/hw4

**NOTE:** For this assignment, you need to run the following command in order
for your Makefile to work:

```sh
$ sudo apt-get install libreadline-dev
```

The `sudo` password for your VM is `cse320` unless you changed it.

The above command installs the GNU `readline` library, which is a software library
that provides line-editing and history capabilites for interactive programs with a
command-line interface.  It allows users to move the cursor, search the command history,
control a kill ring (which is just a more flexible version of a copy/paste clipboard)
and use tab completion on a text terminal.  We highly recommend that you use it for
this assignment.

Here is the structure of the base code:
<pre>
.
├── .gitlab-ci.yml
└── hw4
    ├── include
    │   ├── debug.h
    │   └── imprimer.h
    ├── lib
    │   └── imp_util.o
    ├── Makefile
    ├── rsrc
    │   └── imprimer.cmd
    ├── src
    │   └── main.c
    ├── tests
    └── util
        ├── printer
        ├── show_printers.sh
        └── stop_printers.sh
</pre>

If you run `make`, the code should compile correctly, resulting in an
executable `bin/imprimer`.  If you run this program, it doesn't do very
much, because there is very little code -- you have to write it!

## `Imprimer`: Functional Specification

### Command-Line Interface

When started, `imprimer` should present the user with a command-line
interface with the following prompt

```sh
imp>
```

Typing a blank line should should simply cause the prompt to be repeated,
without any other printout or action by the program.
Non-blank lines are interpreted as commands to be executed.
`Imprimer` commands have a simple syntax, in which each command consists
of a sequence of "words", which contain no whitespace characters,
separated by sequences of one or more whitespace characters.
The first word of each command is a keyword that names the command.
Any remaining words are the arguments to the command.
`Imprimer` should understand the following commands, with arguments as
indicated.
Square brackets are not part of the arguments; they merely identify arguments
that are optional.

  * Miscellaneous commands
    * `help`
    * `quit`

  * Configuration commands
    * `type` *file_type*
    * `printer` *printer_name* *file_type*
    * `conversion` *file_type1* *file_type2* *conversion_program* [*arg1* *arg2* ...]

  * Informational commands
    * `printers`
	* `jobs`

  * Spooling commands
    * `print` *file_name* [printer1 printer2 ...]
    * `cancel` *job_number*
	* `pause` *job_number*
	* `resume` *job_number*
	* `disable` *printer_name*
	* `enable` *printer_name*

The `help` command takes no arguments, and it responds by printing a message
that lists all of the types of commands understood by the program.

The `quit` command takes no arguments and causes execution to terminate.

The `type` command declares *file_type* to be a file type to be supported
by the program.  Possible examples (but not an exhaustive list) of file types
are: `pdf` (Adobe PDF), `ps` (Adobe Postscript), `txt` (text), `png` (PNG image files),
*etc*.  A file will be presumed to be of a particular type when it has an extension
that matches that type.  For example, `foo.txt` will be presumed to be a
text file, if `txt` has previously been declared using the `type` command.
Files whose names do not having an extension that matches a declared type
are considered of unknown type and are to be rejected if an attempt is made
to spool them for printing.
Essentially any identifier can be used as a file type -- they may (but aren't
required to) correspond to "known" file types that have standards, are supported
by other programs, *etc*.

The `printer` command declares the existence of a printer named *printer_name*,
which is capable of printing files of type *file_type*.  The *printer_name*
is just an identifier, such as `Alice`.  Each printer is only capable of printing
files of the (one) type that has been declared for it, and your program should take
care not to send a printer the wrong type of file.

The `conversion` command declares that files of type *file_type1* can be
converted into *file_type2* by running program *conversion_program* with any
arguments that have been indicated.  It is assumed that `conversion_program` reads
input of type *file_type1* from the standard input and writes output of type
*file_type2* to the standard output, so that it is suitable for use in a pipeline
consisting of possibly several such programs.  For example, on your Linux Mint VM:

 * The command `pdf2ps - -` can be used to convert PDF read from the standard input
   to Postscript on the standard output.

 * The command `pbmtext` can be used to convert text read from the standard input
   to a Portable Bitmap (pbm) file on the standard output.

 * The command `pbmtoascii` can be used to convert a Portable Bitmap (pbm) file read
   from the standard input to an ASCII graphics (i.e. text) file on the standard output.

 * The command `pbmtog3` can be used to convert a Portable Bitmap (pbm) file read
   from the standard input to a Group 3 FAX file (g3).

There are many others: some of them work well together and others do not.
For many of these commands there are also corresponding commands that convert formats
in the reverse direction.

The `printers` command prints a report on the current status of the declared printers,
one printer per line.  For example:

```
imp> printers
PRINTER, 0, alice, ps, disabled, idle
PRINTER, 1, bob, pcl, disabled, idle
```

The `jobs` command prints a similar status report for the print jobs that have
been queued.  For example:

```
imp> jobs
JOB, 0, 22 Oct 2018 16:08:11, pdf, queued, 22 Oct 2018 16:08:11, 0, , foo.pdf, ffffffff
JOB, 1, 22 Oct 2018 16:08:16, ps, queued, 22 Oct 2018 16:08:16, 0, , bar.ps, ffffffff
JOB, 2, 22 Oct 2018 16:08:34, txt, queued, 22 Oct 2018 16:08:34, 0, , mumble.txt, ffffffff
```

The formats of these status reports will be generated by functions that have been
provided for you, as discussed in more detail below.  You **must** use the provided
functions, which will make it easier for us to automate some of the testing of
your program.

The `print` command sets up a job for printing *file_name*.
The specified file name must have an extension that identifies it as one of the
file types that have previously been declared with the `type` command.
If optional printer names are specified, then these printers must previously
have been declared using the `printer` command, and they define the set of
*eligible printers* for this job.  Only a printer in the set of eligible printers
for a job should be used for printing that jobs.  Moreover, an eligible printer
can only be used to print a job if there is a way to convert the file in the
job to the type that can be printed by that printer.
If no printer name is specified in the `print` command, then any declared
printer is an eligible printer.

The `cancel` command cancels an existing job.  If the job is currently being
processed, then any processes in the conversion pipeline for that job
are terminated (by sending a `SIGTERM` signal to their process group).

The `pause` command pauses a job that is currently being processed.
Processes in the conversion pipeline for that job are stopped
(by sending a `SIGSTOP` signal to their process group).

The `resume` command resumes a job that was previously paused.
Processes in the conversion pipeline for that job are continued
(by sending a `SIGCONT` signal to their process group).

The `disable` command sets the state of a specified printer to "disabled".
This does not affect the status of any job currently being processed
by that printer, but a disabled printer is not eligible to accept any
further jobs until it has been re-enabled using the `enable` commnd.

The `enable` command sets the state of a specified printer to "enabled".
When a printer becomes enabled, if there is a pending job that can now be
processed by the newly enabled printer, then processing is immediately
started for one such job.

### Program Output

Your program **must** produce output in the following situations, which do
not necessarily occur in direct response to a user command:

  * Whenever the status of a printer has changed.  In this case, the output
	must consist of a status line showing the new status of that printer,
	formatted using the function (`imp_format_printer_status()`) we provide
    for this purpose, as described for the `printers` command above.

  * Whenever the status of a job has changed.  In this case, the output
	must consist of a status line showing the new status of that job,
	formatted using the function (`imp_format_job_status()`) we provide for
    this purpose, as described for the `jobs` command above.

  * Whenever an error occurs while executing a user command.  In this case,
    the output must consist of a single line containing an error message that
    has been formatted using the function (`imp_format_error_message()`)
	that we provide for this purpose.

Your program is permitted to emit output in addition to that specified above,
but any such output must occur on a line that does **not** start with
"`PRINTER`", "`JOB`", `ERROR`, or "`imp>`, so that we can filter it out if we need to.

### Batch Mode

The normal mode of operation of `imprimer` is as an interactive application.
However, it can also be run in batch mode, in which it reads commands
from a command file.  If `imprimer` is started as follows:

```sh
$ imprimer -i command_file
```

then it begins by reading and executing commands from `command_file` until EOF,
at which point it presents the normal prompt and executes commands
interactively.  Normally this feature would be used to cause configuration
commands (declarations of types, printers, and conversions) to be read from
a command file, rather than typed each time.  If a `quit` command appears
in the command file, then the program terminates without entering interactive
mode.  This can be used to run a series of commands completely automatically
without user intervention.

If `imprimer` is started with the "`-o` *output_file*" option, then any output
it produces that would normally appear on the terminal is to be redirected instead
to the specified output file.

### Reading Input

If the program is run in interactive mode (the default), then it should use
the `readline()` function to read commands from the user.
If the program is run in batch mode, then `readline` cannot be used, so in this
case the program will have to read commands using either the standard I/O library
or low-level Unix I/O.

### Processing Print Jobs

The purpose of `imprimer` is to process print jobs that are queued by the user.
Each time there is a change in status of a job or printer as a result of a user command
or the completion of a job being processed, `imprimer` must scan the set of queued jobs
to see if there are any that can now be processed, and if so, start them.
In order for a job to be processed, there must exist a printer that is enabled and
not busy, the printer must be in the `eligible_printers` set for that job,
and there must be a way to convert the type of file in the job to the type
of file the printer is capable of printing.  If these conditions hold, then the
job status is set to `RUNNING`, the chosen printer is set to "busy" and the
`chosen_printer` field of the `JOB` structure is set to point to the `PRINTER`
that has been selected.  A group of processes called *conversion pipeline*
is set up to run a series of programs that will convert the type of file in the job
to the type of file that the printer can print.  This is described further below.

A job will exist at any given time in one of various states, the possibilities
for which are defined by the `JOB_STATUS` enum in `imprimer.h`.
These states and their meanings are:

  * `QUEUED` -- The job has been created and is ready for processing.
	A job will persist in this state only as long as there are no printers in
	the set of eligible printers for that job that can be used to print the job.
    As soon as an eligible printer (of an appropriate type) becomes available,
    the job will transition to the `RUNNING` state.

  * `RUNNING` -- An eligible printer of an appropriate type has been chosen for
    the job and a conversion pipeline has been created to convert the file in the
    job to the type of file that the printer is capable of printing.
    The chosen printer must be among the printers in the `eligible_printers` set
    for that job.  For a job to be started on a printer, the printer must be "enabled"
	and "not busy".  The printer status is changed to "busy", and it stays that
	way as long as the job is `RUNNING`.

  * `PAUSED` -- A job that was previously `RUNNING` has temporarily been stopped
	by sending a `SIGSTOP` signal to the process group of the processes in the conversion
    pipeline.  A job in the `PAUSED` state will remain in that state until a `resume`
    command has been issued by the user.  This will cause a `SIGCONT` signal to be
	sent to the process group of the conversion pipeline.

	> :scream:  The state of a job should **not** be changed immediately when the
	> user issues a `pause` command.  Instead, the `SIGSTOP` signal should first be
    > sent and the state of the job changed from `RUNNING` to `PAUSED` only when a
    > `SIGCHLD` signal has subsequently been received and a call to the `waitpid()`
    > function returns showing `WIFSTOPPED` true of the process status.
    > Similarly, the state of a job should not be changed immediately when the
	> user issues a `resume` command, but only once a `SIGCHLD` signal has been
    > received and a subsequent call to `waitpid()` returns showing `WIFCONTINUED`
    > true of the process status.

  * `COMPLETED` -- A job enters this state from the `RUNNING` state once processing
	has completed and the processes in the conversion pipeline have terminated normally.
	Once in the `COMPLETED` state, a job will remain in the queue so that its status
	can be inspected, but it will be ineligible	for further processing.
    A job will remain in the queue until just after the execution of the first user command
    that is issued after the job has been completed for one minute.
	At that point the job will be deleted from the queue and freed.

  * `ABORTED` -- A job enters this state from the `RUNNING` state if one or more
	processes in the job pipeline terminate abnormally.  Once having entered the `ABORTED`
	state, a job is treated similarly to a `COMPLETED` job as described above.

The `imprimer` program must install a `SIGCHLD` handler so that it can be notified
immediately upon completion of a job being processed.  The handler must appropriately
update the job and printer status information and start any further jobs in the
queue that can now be processed by virtue of the printer having become available.

  > :nerd: Note that you will need to use `sigprocmask()` to block signals at appropriate times,
  > to avoid races between the handler and the main program, the occurrence of which which will
  > result in indeterminate behavior.

Each time the status of a job or printer changes, your program must immediately
print a corresponding status line (created using the `imp_format_job_status()`
or `imp_format_printer_status()` functions).  This is so that so that it is evident
(to those grading your program) what and when state changes have occurred.

### Conversion Pipelines

In order to determine whether a particular printer can be used to service a
particular job, it will be necessary to determine whether there is a sequence
of conversions that can be used to transform the type of the file in the job
to the type of file the printer can print.  To determine this, you will need
to maintain a suitable data structure (*e.g.* a matrix) to record the information
supplied by the user in the form of `conversion` commands, and you will need
to use a suitable algorithm (e.g. breadth-first search) to search for a path
of conversions between the two file types.  If it exists, then the path of
conversions (and the associated conversion commands) forms the basis for setting
up the *conversion pipeline* to process the job.

Creation of a conversion pipeline should be begun by the main program forking
a single process to serve as the "master" process for the pipeline.  This process
should use `setpgid()` to set its process group ID to its own process ID.
The master process will then fork one child process for each link in the
conversion path between the type of the file in the job and the type of file
that the chosen printer can print.  Redirection should be used so that
the standard input of the first process in the pipeline is the file to be printed
and the standard output of the last process in the pipeline is the chosen
printer (for which a file descriptor has been obtained using `imp_connect_to_printer()`).
In addition, the `pipe()` and `dup()` (or `dup2()`) system calls should be used
to arrange to connect the standard output of each intermediate process in the
pipeline to the standard input of the next process.
Each process in the pipeline will execute (using `execve()`) one of the conversion
commands (previously declared by the user using the `conversion` command) to convert
the file read on its standard input to the type required by the next process in the
pipeline.

  > :nerd: It is possible that the type of the queued file is the same as the
  > type of file the printer can print.  In this case, no conversion is required,
  > and the conversion program will consist of the master process and a single
  > child process, which should execute the program `/bin/cat` with no arguments.

The master process of a conversion pipeline is used to simplify the interaction
of the conversion pipeline with the main process.  Since the master process creates
its own process group before forking the child processes, all the child processes
will exist in that process group.  The processes in the pipeline can therefore
be paused and resumed by using `killpg()` to send a `SIGSTOP` or `SIGCONT` to
that process group.  Only the master process is a child of the main process,
so the main process only has to keep track of the process ID for the master process
of each conversion pipeline that it starts.
The master process of a conversion pipeline will need to keep track of its child
processes, and to use `waitpid()` to reap them and collect their exit status.
If any child process terminates by a signal or with a nonzero exit status,
then the conversion pipeline will be deemed to have failed and the master process
should exit with a nonzero exit status.
The main process should interpret the nonzero exit status as an indication that
the job has failed, and it should set the job to the `ABORTED` state.
If all of the child processes in a conversion pipeline terminate normally with
zero exit status, then the master process should also terminate normally with
zero exit status.  The main process should interpret this situation as an indication
that the job has succeeded, and it should set the job the the `COMPLETED` state.

  > **Important:**  You **must** create the processes in a conversion pipeline using
  > calls to `fork()` and `execve()`.  You **must not** use the `system()` function,
  > nor use any form of shell in order to create the pipeline, as the purpose of
  > the assignment is to giving you experience with using the system calls involved
  > in doing this.

## Provided Components

### The `imprimer.h` Header File

The `imprimer.h` header file that we have provided defines function prototypes
for the functions you are to use to format output for your program and to make
connections to printers.  It also contains definitions of some constants and data
types related to these functions.

  > :scream: **Do not make any changes to `imprimer.h`.  It will be replaced
  > during grading, and if you change it, you will get a zero!**

### The `imp_util.o` Library

We have provided you with an object file `imp_util.o` (in the `lib` directory),
which will be automatically linked with your program.  This contains several functions,
whose prototypes are given in the `imprimer.h` header file, which you should use
as follows:

  * char *imp_format_printer_status(PRINTER *printer, char *buf, size_t size);
  * char *imp_format_job_status(JOB *job, char *buf, size_t size);
  * char *imp_format_error_message(char *msg);

	You **must** use these functions to format required output by your program.
	The reason for this is so that everyone's program produces output in a uniform format
	that we might have a fighting chance to process automatically.
	The output looks a bit odd, but you will note that it is in comma-separated-value
    (CSV) format that can be readily parsed.
	These functions take a buffer that you supply, along with its size, and
	they return a pointer to that same buffer.

  * int imp_connect_to_printer(PRINTER *printer);

	This is the function you **must** use to connect to a printer.  If successful,
	it returns a file descriptor to be used to send data to the printer;
	if unsuccessful, -1 is returned.  If the printer is not currently "up",
	then it will be started (see about the `printer` program below).

In order to interface with the above functions, the header file `imprimer.h` defines
structure types `PRINTER` and `JOB`.  You *must* pass in instances of these structures
that have all fields properly initialized.  The meaning of each of the fields is
documented in the comments in the `imprimer.h` file.  Each of these structures also
has an additional `other` field, which can be used to point to arbitrary information
of your own choosing should you have a need to do so.  The functions above ignore the
value of this field, so there is no harm if you don't initialize it.

### The `printer` Program

The `printer` program we have provided (in the `util` directory) simulates a printer
that you can connect to and send data to.  It doesn't actually "print" anything,
but it does log any files you send to it in `spool` directory.  It also maintains
a debug log in that directory, in case it is necessary to get some idea of what the
printer has been doing.

A printer is automatically started when you try to connect to it using the
`imp_connect_to_printer()` function, if it is not already up.
You can also start a printer "manually" by a command of the following form:

```sh
$ util/printer [-d] [-f] PRINTER_NAME FILE_TYPE
```

This starts a printer with name `PRINTER_NAME`, which is capable of printing files of
type `FILE_TYPE`.  Each printer that is started must have a unique name; if you try
to start a second printer with the same name as an existing printer, the second
command will fail.  Once started, printers stay "up" until they are explicitly stopped,
You can stop all printers using the command `make stop_printers`.
The command `make show_printers` can be used to show you the printers that are currently up.

The optional `-d` and `-f` arguments to the `printer` command are used to cause the
printer to exhibit some random behavior.  If `-d` is specified, then random delays
might occur during "printing".  If `-f` is specified, then the printer will be "flaky",
which means that it might disconnect at random times, causing the conversion pipeline
to fail.  The `imp_connect_to_printer()` function has a `flags` argument that can
also be used to specify these flags.  The flags only take effect when the printer
is first started; once a printer is "up", the flags passed when connecting to it
have no further effect.  The flags should be the bitwise "or" of one or more of
`PRINTER_NORMAL`, `PRINTER_DELAY`, and `PRINTER_FLAKY`.

### The `show_printers.sh` and `stop_printers.sh` Shell Scripts

The `util` directory contains shell scripts `show_printers.sh` and `stop_printers.sh`.
These are most easily invoked using `make show_printers` or `make stop_printers`,
though they can also be run directly.

### The `imprimer.cmd` File

The `rsrc` directory contains a file `imprimer.cmd`.  This is a sample
file that contains some `imprimer` configuration commands that can be read using
the `-i` option.  For example:

### The `spool` Directory

The `spool` directory is created by `make` in order to store various files created
by the "printers".  For example, if a printer is started with name `alice`, then
`spool/alice.log` will contain debug log information, `spool/alice.pid` will contain
the process ID of the printer process (for use by `stop_printers.sh`),
`spool/alice.sock` will be a "socket" that is used by `imp_connect_to_printer()`
to connect the printer.  Also, each time a file is "printed" the data that was received
is stored in a separately named file in this directory.
The `spool` directory is not removed by a normal `make clean`.
To remove the `spool` directory and all its contents, you can use `make clean_spool`.

```sh
$ imprimer -i rsrc/imprimer.cmd
```

## Other Notes

  * At some point after this assignment has initially been handed out, I will probably
    make available either a list of "recommended" commands to use in a conversion pipeline,
    or I will make available some "dummy" commands for testing purposes.
    Not having these commands available should not stop you from getting started;
    you can always test your program using `cat` as a "conversion" command.

  * I am considering making things more interesting and realistic by adding an additional
    `display` command to `imprimer` (similar to the `printer` command) together with a
    corresponding`display` program (similar to the `printer` program).  The purpose of this
    would be to provide a way for files to be actually "printed" by running a command that
    displays them graphically in a window.  If I decide to do this, I will make an announcement
    about it and update this document.

## Hand-in instructions
As usual, make sure your homework compiles before submitting.
Test it carefully to be sure that doesn't crash or exhibit "flaky" behavior
due to race conditions.
Use `valgrind` to check for memory errors and leaks.
Besides `--leak-check=full`, also use the option `--track-fds=yes`
to check whether your program is leaking file descriptors because
they haven't been properly closed.
You might also want to look into the `valgrind` `--trace-children` and related
options.

Submit your work using `git submit` as usual.
This homework's tag is: `hw4`.
