/* utilsV5.c
 * Copyright (C) Julien Rabinow <jnr305@nyu.edu>
 *
 * utilsV5.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * utilsV5.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"

/* -------------------- MEMORY MANAGEMENT -------------------- */
#ifdef MANAGE_MEM
static void *heap_bottom = NULL, *heap_top = NULL;
#ifdef USING_VALGRIND
static void *__initial_alloc__ = NULL;

static void __clean_initial_alloc(void)
{
	free(__initial_alloc__);
}
#endif /* #ifdef USING_VALGRIND */

void init_alloc(void)
{
	int count = 0;
	do {
		heap_bottom = heap_top = malloc(1);
		if(heap_bottom == NULL)
			switch(errno) {
				case ENOMEM:
					perror("Error allocating memory ");
					if(count++ < MAX_RETRIES_ALLOC) {
						fputs("Retrying in 100ms\n", stderr);
						usleep(100);
					} else {
						fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_ALLOC);
						exit(EXIT_FAILURE);
					}
					break;
				default:
					perror("Error allocating memory ");
					exit(EXIT_FAILURE);
			}
	} while(heap_bottom == NULL);
#ifdef USING_VALGRIND
	__initial_alloc__ = heap_bottom;
	if(atexit(&__clean_initial_alloc)) {
		perror("Error registering cleanup function ");
		exit(EXIT_FAILURE);
	}
#endif /* #ifdef USING_VALGRIND */
}

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_allocated(void *ptr)
{
	return heap_bottom <= ptr && ptr <= heap_top;
}

void xfree(void *ptr)
{
	if(heap_bottom <= ptr && ptr <= heap_top)
		free(ptr);
}
#endif /* #ifdef MANAGE_MEM */

/* -------------------- ERROR HANDLING -------------------- */
#ifdef ENABLE_ERROR_HANDLING
void *xmalloc(size_t size)
{
	void *ptr = NULL;
	int count = 0;

	do {
		ptr = malloc(size);
		if(ptr != NULL)
			break;
#ifdef __unix
		switch(errno) {
			case ENOMEM:
				perror("Error allocating memory ");
				if(count++ < MAX_RETRIES_ALLOC) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix */
				perror("Error allocating memory ");
				exit(EXIT_FAILURE);
#ifdef __unix
		}
#endif /* #ifdef __unix */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if(ptr < heap_bottom)
		heap_bottom = ptr;
	if(ptr > heap_top)
		heap_top = ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
	void *ptr = NULL;
	int count = 0;

	do {
		ptr = calloc(nmemb, size);
		if(ptr != NULL)
			break;
#ifdef __unix
		switch(errno) {
			case ENOMEM:
				perror("Error allocating memory ");
				if(count++ < MAX_RETRIES_ALLOC) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix */
				perror("Error allocating memory ");
				exit(EXIT_FAILURE);
#ifdef __unix
		}
#endif /* #ifdef __unix */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if(ptr < heap_bottom)
		heap_bottom = ptr;
	if(ptr > heap_top)
		heap_top = ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

char *xstrdup(const char *str)
{
	char *ptr = (char*) NULL;
	int count = 0;

	do {
		ptr = strdup(str);
		if(ptr != (char*) NULL)
			break;
#ifdef __unix
		switch(errno) {
			case ENOMEM:
				perror("Error allocating memory ");
				if(count++ < MAX_RETRIES_ALLOC) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix */
				perror("Error allocating memory ");
				exit(EXIT_FAILURE);
#ifdef __unix
		}
#endif /* #ifdef __unix */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if((void*) ptr < heap_bottom)
		heap_bottom = (void*) ptr;
	if((void*) ptr > heap_top)
		heap_top = (void*) ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
	int count = 0;

	do {
		ptr = realloc(ptr, size);
		if(ptr != NULL)
			break;
#ifdef __unix
		switch(errno) {
			case ENOMEM:
				perror("Error allocating memory ");
				if(count++ < MAX_RETRIES_ALLOC) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_ALLOC);
					exit(EXIT_FAILURE);
				}
				break;
			default:
#endif /* #ifdef __unix */
				perror("Error allocating memory ");
				exit(EXIT_FAILURE);
#ifdef __unix
		}
#endif /* #ifdef __unix */
	} while(BOOL_TRUE);

#ifdef MANAGE_MEM
	if(ptr < heap_bottom)
		heap_bottom = ptr;
	if(ptr > heap_top)
		heap_top = ptr;
#endif /* #ifdef MANAGE_MEM */
	return ptr;
}

FILE *xfopen(const char *path, const char *mode)
{
	FILE *f = (FILE*) NULL;
	int count = 0;

	do {
		f = fopen(path, mode);
		if(f != (FILE*) NULL)
			break;
#ifdef __unix
		switch(errno) {
			case ENOMEM:
				perror("Error opening file ");
				if(count++ < MAX_RETRIES_OPEN) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			case EINTR:
				perror("Error opening file ");
				fputs("Retrying\n", stderr);
				count++;
				break;
			default:
#endif /* #ifdef __unix */
				perror("Error opening file ");
				exit(EXIT_FAILURE);
#ifdef __unix
		}
#endif /* #ifdef __unix */
	} while(BOOL_TRUE);

	return f;
}

FILE *xfdopen(int fd, const char *mode)
{
	FILE *f = (FILE*) NULL;
	int count = 0;

	do {
		f = fdopen(fd, mode);
		if(f != (FILE*) NULL)
			break;
#ifdef __unix
		switch(errno) {
			case ENOMEM:
				perror("Error opening file ");
				if(count++ < MAX_RETRIES_OPEN) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
			case EINTR:
				perror("Error opening file ");
				fputs("Retrying\n", stderr);
				count++;
				break;
			default:
#endif /* #ifdef __unix */
				perror("Error opening file ");
				exit(EXIT_FAILURE);
#ifdef __unix
		}
#endif /* #ifdef __unix */
	} while(BOOL_TRUE);

	return f;
}

#ifndef __unix
int xopen(const char *path, int flags)
{
	int fd, count = 0;

	do {
		fd = open(path, flags);
		if(fd != -1)
			break;
		switch(errno) {
			case ENOMEM:
				perror("Error opening file ");
				if(count++ < MAX_RETRIES_OPEN) {
					fputs("Retrying in 100ms\n", stderr);
					usleep(100);
				} else {
					fprintf(stderr, "Giving up after %d tries\n", MAX_RETRIES_OPEN);
					exit(EXIT_FAILURE);
				}
				break;
#ifdef _GNU_SOURCE
			case EINVAL:
				flags &= ~O_DIRECT;
#endif /* #ifdef _GNU_SOURCE */
			case EINTR:
				perror("Error opening file ");
				fputs("Retrying\n", stderr);
				count++;
				break;
			default:
				perror("Error opening file ");
				exit(EXIT_FAILURE);
		}
	} while(1);

	return fd;
}
#endif /* ifndef __unix */

#endif /* #ifdef ENABLE_ERROR_HANDLING */

/* -------------------- STRING MANIPULATION -------------------- */
#ifdef ENABLE_STRING_MANIPULATION

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif
is_valid_int(const char *str)
{
	if( ! isdigit(*str) && *str != '-')
		return BOOL_FALSE;

	for(str++; *str != '\0'; str++)
		if( ! isdigit(*str))
			return BOOL_FALSE;
	return BOOL_TRUE;
}

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_valid_float(const char *str)
{
	int period = BOOL_FALSE;

	if(*str == '.')
		period = BOOL_TRUE;
	else if( ! isdigit(*str) && *str != '-')
		return BOOL_FALSE;

	for(str++; *str != '\0'; str++)
		if(*str == '.' && period == BOOL_FALSE)
			period = BOOL_TRUE;
		else if( ! isdigit(*str))
			return BOOL_FALSE;
	return BOOL_TRUE;
}

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
is_valid_hex(const char *str)
{
	size_t i;

	for(i = 0; str[i] != '\0'; i++)
		if( ! isxdigit(str[i]))
			return BOOL_FALSE;
	return BOOL_TRUE;
}

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
startswith(const char *str, const char *prefix)
{
	while(*str == *prefix) {
		str++, prefix++;
		if(*str == '\0')
			break;
	}
	return *prefix == '\0';
}

#ifdef ENABLE_BOOL_TYPE
bool
#else
int
#endif /* #ifdef ENABLE_BOOL_TYPE */
endswith(const char *str, const char *suffix)
{
	int i, j;
	for(i = 0; str[i] != '\0'; i++);
	for(j = 0; suffix[j] != '\0'; j++);

	if(i < j)
		return BOOL_FALSE;

	for(; j >= 0; i--, j--)
		if(str[i] != suffix[j])
			return BOOL_FALSE;
	return BOOL_TRUE;
}

void str_tolower(char *str)
{
	for(; *str != '\0'; str++)
		if(*str >= 'A' && *str <= 'Z')
			*str = *str + 32;	/* - 'A' + 'a' */
}

void str_toupper(char *str)
{
	for(; *str != '\0'; str++)
		if(*str >= 'a' && *str <= 'z')
			*str = *str - 32;	/* + 'A' - 'a' */
}

char *const_append(const char *str1, const char *str2)
{
	char *new_str = (char*) NULL;
	size_t len1, len2;

	len1 = strlen(str1);
	len2 = strlen(str2);
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	new_str = (char*) xmalloc(len1 + len2 + 1);
#else
	new_str = (char*) malloc(len1 + len2 + 1);
	if(new_str != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		memcpy(new_str, str1, len1);
		memcpy(new_str + len1, str2, len2 + 1);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
	}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */

	return new_str;
}

char *append(char *str1, const char *str2)
{
	size_t len1, len2;
#ifdef MANAGE_MEM
	if( ! is_allocated(str1))
		return const_append(str1, str2);
#endif /* #ifdef MANAGE_MEM */
	len1 = strlen(str1);
	len2 = strlen(str2);
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	str1 = (char*) xrealloc(str1, len1 + len2 + 1);
#else
	str1 = (char*) realloc(str1, len1 + len2 + 1);
	if(str1 != (char*) NULL)
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		memcpy(str1 + len1, str2, len2 + 1);

	return str1;
}

char *extract(const char *str, char start, char end)
{
	char *extracted = (char*) NULL;
	unsigned i;

	for(; *str != start && *str != '\0'; str++);

	if(*str != '\0') {
		for(str++, i = 0; str[i] != end && str[i] != '\0'; i++);
		if(str[i] != '\0' || end == '\0') {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
			extracted = (char*) xmalloc(i + 1);
#else
			extracted = (char*) malloc(i + 1);
			if(extracted != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
				memcpy(extracted, str, i);
				extracted[i] = '\0';
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
			}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
		}
	}
	return extracted;
}

char *trim(const char *str)
{
	const char *start_str, *end_str;
	char *ptr;
	size_t len;

	for(start_str = str; isspace(*start_str); start_str++);
	for(end_str = start_str; ! isspace(*end_str) && *end_str != '\0'; end_str++);

	for(ptr = (char*) end_str; *ptr != '\0'; ptr++)
		if(isspace(*ptr) && ! isspace(*(ptr - 1)))
			end_str = ptr;
	len = (size_t) (end_str - start_str);
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	ptr = (char*) xmalloc(len + 1);
#else
	ptr = (char*) malloc(len + 1);
	if(ptr != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		memcpy(ptr, start_str, len);
		ptr[len] = '\0';
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
	}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */

	return ptr;
}

char *insert(const char *str, char c, size_t pos)
{
	char *new_str = (char*) NULL;
	size_t len = strlen(str);

	if(pos <= len) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		new_str = (char*) xmalloc(len + 2);
#else
		new_str = (char*) malloc(len + 2);
		if(new_str != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
			memcpy(new_str, str, pos);
			new_str[pos] = c;
			memcpy(new_str + pos + 1, str + pos, len - pos + 1);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
		}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
	}
	return new_str;
}

char *insert_str(const char *str, const char *ins, size_t pos)
{
	size_t len1, len2;
	char *new_str = (char*) NULL;

	len1 = strlen(str);
	if(pos <= len1) {
		len2 = strlen(ins);
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		new_str = (char*) xmalloc(len1 + len2 + 1);
#else
		new_str = (char*) malloc(len1 + len2 + 1);
		if(new_str != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
			memcpy(new_str, str, pos);
			memcpy(new_str + pos, ins, len2);
			memcpy(new_str + pos + len2, str + pos, len1 - pos + 1);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
		}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
	}
	return new_str;
}

char *erase(const char *str, size_t pos)
{
	char *new_str = (char*) NULL;
	size_t len = strlen(str);

	if(pos < len) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		new_str = (char*) xmalloc(len);
#else
		new_str = (char*) malloc(len);
		if(new_str != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
			memcpy(new_str, str, pos);
			memcpy(new_str + pos, str + pos + 1, len - pos);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
		}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
	}
	return new_str;
}

char *erase_str(const char *str, size_t pos, size_t len)
{
	char *new_str = (char*) NULL;
	size_t len2 = strlen(str), new_len = len2 - len + 1;

	if(pos + len <= len2) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		new_str = (char*) xmalloc(new_len);
#else
		new_str = (char*) malloc(new_len);
		if(new_str != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		memcpy(new_str, str, pos);
		memcpy(new_str + pos, str + pos + len, new_len - pos);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
		}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */

	}
	return new_str;
}

char *replace_str(const char *haystack, const char *needle, const char *replacement)
{
	char *new_str = (char*) NULL;
	char *ptr = strstr(haystack, needle);
	size_t len_replacement = strlen(replacement), len_needle = strlen(needle),
	       len_haystack = strlen(haystack);

	if(ptr != (char*) NULL) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		new_str = xmalloc(len_haystack - len_needle + len_replacement + 1);
#else
		new_str = malloc(len_haystack - len_needle + len_replacement + 1);
		if(new_str != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
			memcpy(new_str, haystack, ptr - haystack);
			memcpy((byte*)((intptr_t) new_str + (intptr_t) ptr -
						(intptr_t) haystack), replacement, len_replacement);
			memcpy((byte*)((intptr_t) new_str + (intptr_t) ptr -
						(intptr_t) haystack) + len_replacement, ptr + len_needle,
					len_haystack - (intptr_t) ptr + (intptr_t) haystack - len_needle);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
		}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
	}
	return new_str;
}

const char *rev_strpbrk(const char *str, const char *accept)
{
	const char *ptr = NULL, *iter = str;

	for(iter = str; *iter != '\0'; iter++)
		if(strchr(accept, *iter))
			ptr = iter;
	return ptr;
}

char *neg_strchr(const char *s, int c)
{
	while(*s++ == c);
	return (char*) (*s == '\0' ? NULL : s);
}

/* returnArray is set to NULL if all chars in str are separator
 * returnArray and all elements in returnArray are dynamically allocated -> free them all when done */
size_t split_str(const char *str, const char separator, char ***returnArray)
{
	int i;
	size_t count = 1;

	/* COMMENT NEXT 6 LINES TO NOT SKIP CONSECUTIVE separator
	 * CHARS AT START OF str */
	while(*str == separator)
		str++;
	if(*str == '\0') {
		*returnArray = (char**) NULL;
		return 0;
	}

	for(i = 0; str[i] != '\0'; i++)
		/*count += (str[i] == separator); */
		count += (str[i] == separator && str[i+1] != separator && str[i+1] != '\0');
	/* REPLACE PREVIOUS LINE WITH ABOVE COMMENTED LINE
	 * TO NOT SKIP OVER CONSECUTIVE SEPARATORS */

#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	*returnArray = (char**) xmalloc(count * sizeof(char*));
#else
	*returnArray = (char**) malloc(count * sizeof(char*));
	if(*returnArray == (char**) NULL)
		return 0;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			/* COMMENT NEXT 3 LINES TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			if(i == 0)
				str++;
			else {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
				(*returnArray)[count] = (char*) xmalloc(i + 1);
#else
				(*returnArray)[count] = (char*) malloc(i + 1);
				if((*returnArray)[count] == (char*) NULL) {
					for(; count > 0; count--)
						free((*returnArray)[count - 1]);
					free(*returnArray);
					*returnArray = (char**) NULL;
					return 0;
				}
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
				memcpy((*returnArray)[count], str, i);
				(*returnArray)[count++][i] = '\0';
				str += i+1;
			}	/* COMMENT THIS LINE TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			i = -1;
		}
	}
	if(i != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		(*returnArray)[count] = (char*) xmalloc(i + 1);
#else
		(*returnArray)[count] = (char*) malloc(i + 1);
		if((*returnArray)[count] == (char*) NULL) {
			for(; count > 0; count--)
				free((*returnArray)[count - 1]);
			free(*returnArray);
			*returnArray = (char**) NULL;
			return 0;
		}
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		strcpy((*returnArray)[count++], str);
	}
	return count;
}

/* returnArray is set to NULL if all chars in str are separator
 * returnArray is dynamically allocated -> free when done */
size_t split_str_lite(char *str, const char separator, char ***returnArray)
{
	int i;
	size_t count = 1;

	/* COMMENT NEXT 6 LINES TO NOT SKIP CONSECUTIVE separator
	 * CHARS AT START OF str */
	while(*str == separator)
		str++;
	if(*str == '\0') {
		*returnArray = (char**) NULL;
		return 0;
	}

	for(i = 0; str[i] != '\0'; i++)
		/*count += (str[i] == separator); */
		count += (str[i] == separator && str[i+1] != separator && str[i+1] != '\0');
	/* REPLACE PREVIOUS LINE WITH ABOVE COMMENTED LINE
	 * TO NOT SKIP OVER CONSECUTIVE SEPARATORS */

#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	*returnArray = (char**) xmalloc(count * sizeof(char*));
#else
	*returnArray = (char**) malloc(count * sizeof(char*));
	if(*returnArray == (char**) NULL)
		return 0;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */

	for(count = i = 0; str[i] != '\0'; i++) {
		if(str[i] == separator) {
			/* COMMENT NEXT 3 LINES TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			if(i == 0)
				str++;
			else {
				(*returnArray)[count] = str;
				(*returnArray)[count++][i] = '\0';
				str += i+1;
			}	/* COMMENT THIS LINE TO NOT SKIP OVER CONSECUTIVE SEPARATORS */
			i = -1;
		}
	}
	if(i != 0)
		(*returnArray)[count++] = str;
	return count;
}

#endif /* ifdef ENABLE_STRING_MANIPULATION */


/* -------------------- Reading data -------------------- */
#ifdef ENABLE_READ_DATA

char *read_line(FILE *stream)
{
	unsigned i = 0;
	unsigned current_size = 32;
	char c;
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	char *str = (char*) xmalloc(32);
#else
	char *str = (char*) malloc(32);
	if(str != (char*) NULL)
		return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */

#ifdef _POSIX_THREAD_SAFE_FUNCTIONS	/* locking and getc_unlocked functions */
	flockfile(stream);
	while((c = getc_unlocked(stream)) != EOF && c != '\n')
#else
		while((c = getc(stream)) != EOF && c != '\n')
#endif /* #ifdef _POSIX_THREAD_SAFE_FUNCTIONS */
		{
			if(i == current_size) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
				str = (char*) xrealloc(str, current_size <<= 1);
#else
				str = (char*) realloc(str, current_size <<= 1);
				if(str == (char*) NULL)
					return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
			}
			str[i++] = c;
		}
#ifdef _POSIX_THREAD_SAFE_FUNCTIONS
	funlockfile(stream);
#endif /* #ifdef _POSIX_THREAD_SAFE_FUNCTIONS */
	if(i == current_size) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		str = (char*) xrealloc(str, current_size += 1);
#else
		str = (char*) realloc(str, current_size += 1);
		if(str == (char*) NULL)
			return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	} else if(c == EOF && i == 0) {
		free(str);
		return (char*) NULL;
	} else {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		str = xrealloc(str, i + 1);
#else
		str = realloc(str, i + 1);
		if(str == (char*) NULL)
			return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
	str[i] = '\0';
	return str;
}

char *read_file_descriptor(int fd)
{
	int current_size = 32;
	ssize_t i = 0;
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	char *str = (char*) xmalloc(32);
#else
	char *str = (char*) malloc(32);
	if(str != (char*) NULL)
		return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */

	/* read (current_size - i) chars at a time, double current_size and increment
	 * i by the number of characters read and repeat until no more characters
	 * are available */
	do {
		i += read(fd, str + i, current_size - i);
		if(i == current_size) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
			str = xrealloc(str, current_size <<= 1);
#else
			str = realloc(str, current_size <<= 1);
			if(str != (char*) NULL)
				return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		}
	} while(i << 1 == current_size);

	if(i == -1) {
		free(str);
		return (char*) NULL;
	} else {
		/* remove all non-printable characters from end of string
		 * see 'isprint' manpage for more details */
		while(! isprint(str[i-1]))
			i--;
		/* allocate precisely as much memory (not a single byte more)
		 * as is needed to contain the data */
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		if(i == current_size)
			str = (char*) xrealloc(str, current_size += 1);
		else
			str = (char*) xrealloc(str, i + 1);
#else
		if(i == current_size)
			str = (char*) realloc(str, current_size += 1);
		else
			str = (char*) realloc(str, i + 1);
		if(str == (char*) NULL)
			return (char*) NULL;
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
		str[i] = '\0';
	}
	return str;
}
#endif /* #ifdef ENABLE_READ_DATA */


/* -------------------- BITSETTING -------------------- */
#ifdef ENABLE_BITSET

int getbit(bitset set, int pos)
{
	return (set[pos / 8] & (1 << pos % 8)) >> (pos % 8);
}

void setbit(bitset set, int pos)
{
	set[pos / 8] |= (1 << pos % 8);
}

void unsetbit(bitset set, int pos)
{
	set[pos / 8] &= ~(1 << pos % 8);
}

int togglebit(bitset set, int pos)
{
	set[pos / 8] ^= 1 << pos % 8;
	return (set[pos / 8] & (1 << pos % 8)) >> (pos % 8);
}
#endif /* #ifdef ENABLE_BITSET */


/* -------------------- Filesystem functions -------------------- */
#ifdef ENABLE_FILESYSTEM
int is_dir(char *path)
{
	struct stat buf;

	if(lstat(path, &buf) != 0) {
		perror("Error statting file ");
		return -1;
	} else
		return S_ISDIR(buf.st_mode);
}

#ifdef	_WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif /* #ifdef _WIN32 */
char *make_path(const char *old_path, const char *dir_name)
{
	char *new_path = (char*) NULL;
	int len1, len2;

	len1 = strlen(old_path);
	len2 = strlen(dir_name);
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	new_path = (char*) xmalloc(len1 + len2 + 2);
#else
	new_path = (char*) malloc(len1 + len2 + 2);
	if(new_path != (char*) NULL) {
#endif /* if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	memcpy(new_path, old_path, len1);
	new_path[len1] = FILE_SEPARATOR;
	memcpy(new_path + len1 + 1, dir_name, len2 + 1);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
	}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
	return new_path;
}
#undef FILE_SEPARATOR

void *dirwalk(const char *path, void* (*func)(void*, char*), void *arg)
{
	DIR *dir;
	struct dirent *entry;
	char *new_path;

	dir = opendir(path);
	if(dir != (DIR*) NULL) {
		/* iterate over linked list. When we have examined all files in directory, readdir() returns NULL */
		while((entry = readdir(dir)) != (struct dirent*) NULL) {
			if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
				new_path = make_path(path, entry->d_name);
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
				if(new_path == (char*) NULL) {
					closedir(dir);
					return NULL;
				}
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
#ifdef _WIN32
				if(is_dir(new_path))
#else
					if(entry->d_type == DT_DIR)	/* windows sucks, this line works on unix-based OSs only */
#endif /* #ifdef _WIN32 */
						dirwalk(new_path, func, arg);
					else
						arg = func(arg, new_path);
				free(new_path);
			}
		}
		closedir(dir);
	} else
		arg = NULL;
	return arg;
}
#endif /* #ifdef ENABLE_FILESYSTEM */


/* -------------------- Terminal manipulation functions and color printing -------------------- */
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix)

typedef struct {
	Color c;
	Color bgc;
	Style s;
} ColorEnv;

static ColorEnv __session__;

void set_style(Color c, Color bgc, Style s)
{
	printf("\x1B[%d;%dm", c + 30, s);
	__session__.c = c;
	__session__.s = s;
	printf("\x1B[%dm", bgc + 40);
	__session__.bgc = bgc;
}

void stylish_print(char *str, Color c, Color bgc, Style s)
{
	printf("\x1B[%d;%dm", c + 30, s);
	printf("\x1B[%dm%s\x1B[%dm", bgc + 40, str, __session__.bgc);
	printf("\x1B[%d;%dm", __session__.c + 30, __session__.s);
	fflush(stdout);
}

void turn_echoing_off(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
	term.c_lflag &= ~ECHO;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
}

void turn_echoing_on(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
	term.c_lflag |= ECHO;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
}

void instant_getchar(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
	term.c_lflag &= ~ICANON;
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
}

void normal_getchar(void)
{
	struct termios term;

	if(tcgetattr(STDIN_FILENO, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
	term.c_lflag |= ICANON;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) != 0) {
#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
		perror("Error manipulating terminal ");
		exit(EXIT_FAILURE);
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	}
}

#endif /* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix) */


/* -------------------- Threading -------------------- */
#ifdef ENABLE_THREADING

pthread_t launch_thread(void* (*start_routine)(void*), void *arg, int detach)
{
	pthread_t th;
	if(pthread_create(&th, NULL, start_routine, arg) != 0) {
		log_message(LOG_ERROR, "Error creating thread!\n");
		th = 0;
	} else if(detach && pthread_detach(th) != 0)
		log_message(LOG_ERROR, "Error detaching thread\n");
	return th;
}

#ifdef ENABLE_ERROR_HANDLING

pthread_t xlaunch_thread(void *(*start_routine)(void*), void *arg, int detach)
{
	pthread_t th = launch_thread(start_routine, arg, detach);
	if(th == 0) {
		perror("Thread not created");
		exit(EXIT_FAILURE);
	}
	return th;
}

void *xpthread_join(pthread_t thread)
{
	void *retval;

	if(pthread_join(thread, &retval) != 0) {
		perror("Error joining threads");
		exit(EXIT_FAILURE);
	}
	return retval;
}

#endif /* #ifdef ENABLE_ERROR_HANDLING */
#endif /* #ifdef ENABLE_THREADING */

/* -------------------- Memory pool -------------------- */
#ifdef ENABLE_MEMPOOL

void mempool_create(struct mempool *mp, size_t size, size_t nmemb)
{
	unsigned i;
	void *val;

#if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING)
	mp->mem = xmalloc((sizeof(unsigned) + size) * nmemb);
	mp->ptrs = xmalloc(sizeof(unsigned*) * nmemb);
#else
	mp->mem = malloc((sizeof(unsigned) + size) * nmemb);
	mp->ptrs = malloc(sizeof(unsigned*) * nmemb);
	if(mp->ptrs == NULL || mp->mem == NULL) {
		free(mp->mem);
		free(mp->ptrs);
		mp->mem = NULL;
		mp->ptrs = NULL;
		return;
	}
#endif /* #if defined(ENABLE_ERROR_HANDLING) && defined(INTERNAL_ERROR_HANDLING) */
	mp->size = size;
	mp->nmemb = nmemb;
	mp->index = 0;

	val = mp->mem;
	for(i = 0; i < nmemb; i++) {
		mp->ptrs[i] = val;
		val = (void*) ((byte*) val + sizeof(unsigned) + size);
	}
}

void *mempool_alloc(struct mempool *mp)
{
/*
 * Problem when enlarging memory pool: mp->mem will point to a different chunk of mem,
 * but the pointers pointing to the memory in the mempool will not be updated. Not much
 * we can do about that as of now => let's  disable pool enlarging
 *
 * WARNING: this means that if you call mempool_alloc more than is reasonable, you WILL
 * have problems
 *
	unsigned i;
	void *val;

	if(mp->index == mp->nmemb) {
		mp->mem = xrealloc(mp->mem, (sizeof(unsigned) + mp->size) * (mp->nmemb <<= 1));
		mp->ptrs = xrealloc(mp->ptrs, sizeof(unsigned*) * mp->nmemb);

		val = mp->mem;
		for(i = 0; i < mp->nmemb; i++) {
			mp->ptrs[i] = val;
			val += sizeof(unsigned) + mp->size;
		}
	}
*/
	*(unsigned*) mp->ptrs[mp->index] = mp->index;
	return (byte*) mp->ptrs[mp->index++] + sizeof(unsigned);
}

void mempool_free(struct mempool *mp, void *ptr)
{
	void *swap;
	unsigned idx = *(unsigned*) ((byte*) ptr - sizeof(unsigned));

	swap = mp->ptrs[idx];
	mp->ptrs[idx] = mp->ptrs[--mp->index];
	mp->ptrs[mp->index] = swap;
}

void mempool_delete(struct mempool *mp)
{
	free(mp->mem);
	free(mp->ptrs);
}
#endif /* #ifdef ENABLE_MEMPOOL */


/* -------------------- Misc functions -------------------- */
#ifdef ENABLE_MISC
Weekday get_day_of_week(int day, int month, int year)
{
	int k, j;
	Weekday d;

	if(month == 1) {
		month = 13;
		year--;
	}
	if(month == 2) {
		month = 14;
		year--;
	}
	k = year % 100;
	j = year / 100;
	d = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
	return d <= 1 ? d + 5 : d - 2;
}

char *itoa(int n, char *buffer)
{
	char *ptr = buffer;
	int log;

	if(n < 0) {
		*ptr++ = '-';
		n = 0 - n;
	}
	for(log = n; log != 0; log /= 10)
		ptr++;
	for(*ptr = '\0'; n != 0; n /= 10)
		*--ptr = n % 10 + '0';

	return buffer;
}

int hexatoi(const char *hex)
{
	size_t size;
	unsigned res, pow;

	for(size = 0; hex[size] != '\0'; size++);

	for(pow = 1, res = 0, size--; size > 0; pow <<= 4, size--)
		res += pow * (hex[size] >= 'a' ? hex[size] - 87 : (hex[size] >= 'A' ? hex[size] - 55 : hex[size] - '0'));

	if(hex[0] == '-')
		res = 0 - res;
	else
		res += pow * (hex[0] >= 'a' ? hex[0] - 87 : (hex[0] >= 'A' ? hex[0] - 55 : hex[0] - '0'));
	return res;
}

unsigned int gcd(unsigned int u, unsigned int v)
{
	int shift;

	if(u == 0)
		return v;
	if(v == 0)
		return u;
	/* Let shift := lg K, where K is the greatest power of 2
	   dividing both u and v. */
	for(shift = 0; ((u | v) & 1) == 0; shift++) {
		u >>= 1;
		v >>= 1;
	}

	/* while u is even */
	while((u & 1) == 0)
		u >>= 1;
	do {
		while ((v & 1) == 0)
			v >>= 1;
		/* Now u and v are both odd. Swap if necessary so u <= v,
		   then set v = v - u (which is even). */
		if (u > v)
			SWAP(u, v, unsigned int);
		v -= u;
	} while(v != 0);

	/* restore common factors of 2 */
	return u << shift;
}

void *initialize_vector(void *dest, const void *src, size_t size, size_t nmemb)
{
	size_t i;

	memcpy(dest, src, size);
	for(i = 1; i << 1 <= nmemb; i <<= 1)
		memcpy((byte*)dest + i * size, dest, i * size);
	memcpy((byte*)dest + i * size, dest, (nmemb - i) * size);
	return dest;
}

void register_signal_handler(int signum, void (*sighandler)(int))
{
	struct sigaction new_sigaction;

	memset(&new_sigaction, 0, sizeof(struct sigaction));
	new_sigaction.sa_handler = sighandler;
	if(sigaction(signum, &new_sigaction, (struct sigaction*) NULL) != 0) {
#if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING)
		perror("Error registering signal handler ");
		exit(EXIT_FAILURE);
#endif /* #if ! defined(ENABLE_ERROR_HANDLING) || ! defined(INTERNAL_ERROR_HANDLING) */
	}
}

/* From
http://www.emoticode.net/c/an-example-log-function-using-different-log-levels-and-variadic-macros.html
 */
static log_level_t __g_loglevel = LOG_DEBUG;
static FILE *__g_loghandle = NULL;

void init_log(FILE *stream, log_level_t loglevel)
{
	__g_loglevel = loglevel;
	__g_loghandle = stream;
}

void log_message(log_level_t level, const char *format, ...)
{
	char buffer[255] = { 0 }, timestamp[255] = { 0 }, *slevel;
	va_list ap;
	time_t rawtime;
	struct tm *timeinfo;
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix)
	Color color = WHITE, bgcolor = BLACK;
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix) */

	if(level >= __g_loglevel) {
		va_start(ap, format);
		vsnprintf(buffer, 255, format, ap);
		va_end(ap);
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timestamp, 255, "%d/%m/%Y %X", timeinfo);

		switch(level) {
			case LOG_DEBUG:
				slevel = "DEBUG";
				break;
			case LOG_INFO:
				slevel = "INFO";
				color = GREEN;
				break;
			case LOG_WARNING:
				slevel = "WARNING";
				color = YELLOW;
				break;
			case LOG_ERROR:
				slevel = "ERROR";
				color = RED;
				break;
			case LOG_FATAL:
			default:
				slevel = "FATAL";
				bgcolor = RED;
				break;
		}
#if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix)
		if(__g_loghandle == stdout || __g_loghandle == stderr) {
			fprintf(__g_loghandle, "\x1B[%d;0m", color + 30);
			fprintf(__g_loghandle, "\x1B[%dm[%s] [%s] %s", bgcolor + 40, timestamp, slevel, buffer);
			reset_style(__g_loghandle);
			putc('\n', __g_loghandle);
			fflush(__g_loghandle);
		} else
#endif	/* #if defined(ENABLE_TERMIOS_MANIPULATION) && defined(__unix) */
			fprintf(__g_loghandle, "[%s] [%s] %s\n", timestamp, slevel, buffer);
	}
}

void failwith(char *errmsg)
{
	fputs(errmsg, stderr);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

#endif /* #ifdef ENABLE_MISC */
