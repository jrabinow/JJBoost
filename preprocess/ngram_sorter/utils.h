/* utils.h
 * Copyright (C) Julien Rabinow <jnr305@nyu.edu>
 *
 * utils.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * utils.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILS_H
#define UTILS_H

#if defined(__STDC__)
# define C89
# if defined(__STDC_VERSION__)
#  define C90
#  if (__STDC_VERSION__ >= 199901L) && ! defined C99
#   define C99
#  endif
# endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Read through this file to find out precisely what is enabled by the
 * following #defines */

/* define bool variable type. I recommend taking Torvalds advice and not using
 * this feature:
 * https://lkml.org/lkml/2013/8/31/138 */
/* #define ENABLE_BOOL_TYPE */
/* Enables memory management such as heap memory identification. Planned: leak
 * detection and related stuff */
/*#define MANAGE_MEM */
/* define error "squashing" functions. Program exits if error happens. Disable
 * if your application must meet certain robustness requirements */
#define ENABLE_ERROR_HANDLING
/* various string manipulation functions */
#define ENABLE_STRING_MANIPULATION
/* easily read data from streams/file descriptors */
#define ENABLE_READ_DATA
/* Bitset */
//#define ENABLE_BITSET
/* directory navigation functions */
//#define ENABLE_FILESYSTEM
/* Terminal manipulation functions, as well as color text printing */
/* Will not work on windows machine */
//#define ENABLE_TERMIOS_MANIPULATION
/* Threading functions */
//#define ENABLE_THREADING
/* Memory pool, to cache heap memory and avoid having to call malloc. Idea is that
 * we often ask for same memory sizes (nodes in a linked list or tree, data structures
 * each requiring the same amount of memory...). This memory pool reduces overhead caused
 * by memory allocation
 * This implementation is highly experimental and suffers from a few limitations. Proceed
 * with caution */
/*#define ENABLE_MEMPOOL */
/* miscellaneous functions */
//#define ENABLE_MISC

/* calling init_alloc() as enabled by MANAGE_MEM causes of a 1-byte allocation.
 * Although the overhead associated with freeing that single byte isn't worth the effort,
 * the allocation will appear in valgrind as a memory leak unless you enable
 * USING_VALGRIND */
#ifdef MANAGE_MEM
# define USING_VALGRIND
#endif /* ifdef MANAGE_MEM */

/* whether to quit when encountering an error, or report it back to the user.
 * Depends on ENABLE_ERROR_HANDLING being enabled */
#ifdef ENABLE_ERROR_HANDLING
# define INTERNAL_ERROR_HANDLING
#endif /* ifdef ENABLE_ERROR_HANDLING */

/* Library configuration done. You shouldn't need to modify anything below this
 * line.
 * -------------------------------------------------------------------------- */

extern int errno;

#include <stdint.h>
#ifdef _STDINT_H
 typedef uint8_t byte;
#else
 typedef unsigned char byte;
#endif /* #ifdef _STDINT_H */

#ifdef ENABLE_BOOL_TYPE
# define BOOL_FALSE false
# define BOOL_TRUE true
# ifdef C99
#  include <stdbool.h>
# else
   typedef enum { false = 0, true = 1 } bool;
# endif
#else
# define BOOL_FALSE 0
# define BOOL_TRUE 1
#endif /* #ifdef ENABLE_BOOL_TYPE */

#if defined(MANAGE_MEM) || defined(ENABLE_ERROR_HANDLING)
# define MAX_RETRIES_ALLOC	3
#endif /* #if defined(MANAGE_MEM) || defined(ENABLE_ERROR_HANDLING) */



/* -------------------- MEMORY MANAGEMENT -------------------- */
#ifdef MANAGE_MEM

#include <unistd.h>

/* To use the following functions, you must call init_alloc from your program before allocating any memory
 * WARNING: DO NOT MIX calls to malloc() with calls to xmalloc(). The functions will only work correctly
 * for memory allocated through xmalloc.
 * Finally, remember to delete memory structs allocated by other functions (getaddrinfo for example) */

/* TODO: get rid of all these limitations by exporting a hook to malloc */
void init_alloc(void);

/* returns true if ptr points to dynamically allocated memory, false otherwise. You can now do things like
 * if(is_allocated(myptr))
 *	free(myptr);
 */
#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_allocated(void *ptr);

/* if you're too lazy to do like in the previous example, this function does it for you */
void xfree(void *ptr);
#endif



/* -------------------- ERROR HANDLING -------------------- */
#ifdef ENABLE_ERROR_HANDLING

#ifdef __unix
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>
#endif /* #ifdef __unix */

/* exit program with failed status if malloc and consorts fail
 * -> no more error checking necessary
 * free() when done */
void *xmalloc(size_t size);
void *xcalloc(size_t nmemb, size_t size);
char *xstrdup(const char *str);
void *xrealloc(void *ptr, size_t size);

#define MAX_RETRIES_OPEN	3
/* attempt to open file file in mode mode. Calls exit() at failure */
FILE *xfopen(const char *file, const char *mode);
/* attempt to create a stream opened in mode mode from file descriptor fd.
 * Calls exit() at failure */
FILE *xfdopen(int fd, const char *mode);

#ifdef __unix
int xopen(const char *path, int flags);
#ifdef INTERNAL_ERROR_HANDLING
/* attempts to create a pipe. Calls exit() at failure */
#define xpipe(pipefd)\
	if(pipe(pipefd) != 0) {\
		perror("Error creating pipe ");\
		exit(EXIT_FAILURE);\
	}

/* attempts to duplicate file descriptor oldfd to newfd. Calls exit() at failure */
#define xdup2(oldfd, newfd)\
	if(dup2(oldfd, newfd) == -1) {\
		perror("Error redirecting output ");\
		exit(EXIT_FAILURE);\
	}
#endif /* #ifdef INTERNAL_ERROR_HANDLING */
#endif /* #ifdef __unix */

#endif /* #ifdef ENABLE_ERROR_HANDLING */



/* -------------------- STRING MANIPULATION -------------------- */
#ifdef ENABLE_STRING_MANIPULATION

#include <ctype.h>
/* returns true if isdigit() returns true for all chars in str, false otherwise */
#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_valid_int(const char *str);

/* Same idea as is_valid_int, except checks for float (allows for a single '.' in str) */
#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_valid_float(const char *str);

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_valid_hex(const char *str);

/* returns true if str starts with prefix */
#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
startswith(const char *str, const char *prefix);

/* returns true if str ends with suffix */
#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
endswith(const char *str, const char *suffix);

/* sets str to lower case */
void str_tolower(char *str);

/* sets str to upper case */
void str_toupper(char *str);

/* Creates a new string composed of str2 appended to str1
 * free() when done */
char *const_append(const char *str1, const char *str2);

/* Appends str2 to str1. str1 MUST be dynamically allocated and will be modified by calling
 * this function. It might also be moved to another memory location.
 * free() when done */
char *append(char *str1, const char *str2);

/* extracts portion of str contained between 1st occurence of start and first occurence (following start) of end.
 * If str does not contain start or end char, or there is no end character after start,
 * returns NULL.
 * Example:
 * extract("foo(bar)", '(', ')') will return a pointer to the string "bar"
 * free after usage */
char *extract(const char *str, char start, char end);

/* copy str while ignoring leading and trailing whitespace
 * free after usage */
char *trim(const char *str);

/* return a copy of str with c inserted at position pos
 * free after usage */
char *insert(const char *str, char c, size_t pos);

/* return a copy of str with ins inserted at position pos
 * free after usage */
char *insert_str(const char *str, const char *ins, size_t pos);

/* return a copy of str with character at position pos deleted
 * free after usage */
char *erase(const char *str, size_t pos);

/* return a copy of str with len characters at position pos deleted
 * free after usage */
char *erase_str(const char *str, size_t pos, size_t len);

/* return a copy of haystack with first occurence of needle replaced by replacement
 * free after usage */
char *replace_str(const char *haystack, const char *needle, const char *replacement);

/* locates last occurence in str of any of the bytes in accept */
const char *rev_strcspn(const char *str, const char *accept);

/* return pointer to first occurence of char in s not equal to c. If str is made up entirely
 * of c characters, returns NULL */
char *neg_strchr(const char *s, int c);

/* separates str along separator chars into non-empty tokens. If str is composed only
 * of separator chars, returnArray will point to NULL.
 * Otherwise, returnArray will point to dynamically allocated array with one string token
 * per array element.
 * return value is size of array.
 * free() *returnArray, (*returnArray)[0], (*returnArray)[1] ... when done.
 * In case of error, *returnArray is NULL and errno is set appropriately */
size_t split_str(const char *str, const char separator, char ***returnArray);

/* Same thing as split_str, except that elements in returnArray are pointers to locations
 * in the original string. Use only if you have no more use for original data as it is
 * modified (each separator char is replaced by a '\0')
 * This function is more efficient as there is no data copying involved, and the only
 * allocated memory is *returnArray
 * In case of error, *returnArray is NULL and errno is set appropriately */
size_t split_str_lite(char *str, const char separator, char ***returnArray);

/* returns true if str1 and str2 are 2 the same strings. Helps make code more readable*/
#define equals(str1, str2)	(strcmp(str1, str2) == 0)

#endif /* #ifdef ENABLE_STRING_MANIPULATION */



/* -------------------- Reading data -------------------- */
#ifdef ENABLE_READ_DATA

/* reads a complete line (no length limit) from file.
 * free() buffer when done. As an alternative, look up getline(3) */
char *read_line(FILE *stream);
/* for portability with old code */
#define readLine	read_line

#ifdef __unix
/* read as much as possible from file descriptor.
 * free() buffer when done */
#include <ctype.h>
#include <unistd.h>
char *read_file_descriptor(int fd);
#endif /* #ifdef __unix */

/* Empties buffer till nothing left to read or hits end of line. Useful with scanf/fscanf */
#define empty_buffer(stream)	{\
	int __c__;\
	while((__c__ = getc(stream)) != EOF && __c__ != '\n');\
}

#endif /* #ifdef ENABLE_READ_DATA */



/* -------------------- BITSETTING -------------------- */
#ifdef ENABLE_BITSET

typedef byte *bitset;

/* obtain bit at position pos from set array */
int getbit(bitset set, int pos);

/* set bit at position pos to 1 */
void setbit(bitset set, int pos);

/* set bit at position pos to 0 */
void unsetbit(bitset set, int pos);

/* flip bit at position pos */
int togglebit(bitset set, int pos);

#endif /* #ifdef ENABLE_BITSET */



/* -------------------- Filesystem functions -------------------- */
#ifdef ENABLE_FILESYSTEM

#ifdef __unix
#include <sys/stat.h>
#endif /* #ifdef __unix */

#include <dirent.h>

int is_dir(char *path);

/* creates a new dynamically allocated string of the form "path/filename"
 * or "path\filename" if on windows
 * free() when done */
char *make_path(const char *path, const char *filename);

/* iterates over and calls func() on every file in filesystem under path. arg is
 * an argument passed as 1st argument to func.
 * Returns arg after iterating through every file under path
 * In case of an error, dirwalk will return NULL and errno will be set appropriately.
 * Please note that if func() returns NULL at every call, you will have to check
 * errno to determine if an error occured */
void *dirwalk(const char *path, void* (*func)(void* arg, char* path), void *arg);

#endif /* #ifdef ENABLE_FILESYSTEM */



/* -------------------- Terminal manipulation functions and color printing -------------------- */
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix)

#include <unistd.h>
#include <termios.h>

/* clear all content from terminal */
#define clear_screen()			printf("\x1B[2J\x1B[0;0H");

typedef enum { BLACK, RED, GREEN, YELLOW, BLUE, PINK, CYAN, WHITE } Color;

typedef enum { NORMAL, BOLD, DARK, ITALIC, UNDERLINED } Style;

/* set font color to c, background color to bgc and font style to s */
void set_style(Color c, Color bgc, Style s);

/* print a single string str with font color c, background color bgc and style s
 * restores previous state of terminal after printing str */
void stylish_print(char *str, Color c, Color bgc, Style s);

/* reset everything to default */
#define reset_style(stream)		fprintf(stream, "\x1B[0m")

/* Do not display keyboard input on terminal */
void turn_echoing_off(void);

/* Display keyboard input on terminal */
void turn_echoing_on(void);

/* program reads input without user having to press enter */
void instant_getchar(void);

/* Restore default behaviour */
void normal_getchar(void);

#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix) */

/* -------------------- Threading -------------------- */
#ifdef ENABLE_THREADING

#include <pthread.h>

#define DETACH_THREAD		1
#define NO_DETACH_THREAD	0

/* launches start_routine in a new thread with arg as argument. detach is a flag
 * to tell if we should call pthread_detach on new thread or not.
 * Returns thread id variable */
pthread_t launch_thread(void *(start_routine)(void*), void *arg, int detach);

#ifdef ENABLE_ERROR_HANDLING
pthread_t xlaunch_thread(void *(*start_routine)(void*), void *arg, int detach);

void *xpthread_join(pthread_t thread);
#endif /* #ifdef ENABLE_ERROR_HANDLING */

#endif /* #ifdef ENABLE_THREADING */



/* -------------------- Memory pool -------------------- */
#ifdef ENABLE_MEMPOOL

struct mempool {
	void *mem, **ptrs;
	size_t size, nmemb, index;
};

/* create memory pool of nmemb elements, each of size size */
void mempool_create(struct mempool *mp, size_t size, size_t nmemb);

/* obtain one element from the mempool */
void *mempool_alloc(struct mempool *mp);

/* free the memory pointed to by ptr back into the memory pool */
void mempool_free(struct mempool *mp, void *ptr);

/* once done using the memory pool, delete it */
void mempool_delete(struct mempool *mp);

#endif /* #ifdef ENABLE_MEMPOOL */



/* -------------------- Misc functions -------------------- */
#ifdef ENABLE_MISC

#include <stdarg.h>
#include <time.h>

typedef enum {
	Monday = 0,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
} Weekday;

/* return day of week. 0 for Monday, 1 for Tuesday ... 6 for Sunday. Uses
 * Zeller's congruence for the algorithm */
Weekday get_day_of_week(int day, int month, int year);

/* returns pointer to buffer (must be preallocated and sufficiently large) containing base 10
 * string representation of val. */
char *itoa(int val, char *buffer);

/* convert from hexadecimal format string to integer */
int hexatoi(const char *hex);

/* return greatest common divisor of u and v */
unsigned int gcd(unsigned int u, unsigned int v);

/* Efficiently fill dest with contents of src. src is a single element of size size. dest is a
 * memory buffer of size size * nmemb
 * Example:
 * long unsigned int dest[100], value = 42;
 * initialize_vector(dest, &value, sizeof value, 100); fills all 100 elements of dest with the
 * value 42 */
void *initialize_vector(void *dest, const void *src, size_t size, size_t nmemb);

#ifdef __unix
#include <signal.h>
/* register sighandler as the signal handler function to be called when program receives
 * signal signum. As usual, signals SIGKILL and SIGSTOP cannot be ignored or intercepted */
void register_signal_handler(int signum, void (*sighandler)(int));
#endif /* #ifdef __unix */

typedef enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL
} log_level_t;

/* Initialize log system on file stream. You MUST call this before calling
 * the log_message function */
void init_log(FILE *stream, log_level_t loglevel);
/* logs a message following printf conventions */
void log_message(log_level_t level, const char *format, ... );

/* prints errmsg to stderr and calls exit(). Functions previously registered with atexit()
 * will be called */
void failwith(char *errmsg);

/* swap variables a and b. Both a and b must be of type TYPE */
#define SWAP(a, b, TYPE)	{\
	TYPE __tmp__ = a;\
	a = b;\
	b = __tmp__;\
}

#endif /* #ifdef ENABLE_MISC */
#endif	/* #ifndef UTILS_H */
