#include "utils.h"

#define START_MEM	256

typedef struct {
	int num_occurences;
	char *data;
} Ngram;

int compare(Ngram *n1, Ngram *n2)
{
	if(n1->num_occurences < n2->num_occurences)
		return -1;
	else
		return n1->num_occurences > n2->num_occurences;
}

int main(int argc, char *argv[])
{
	if(argc != 2) {
		puts("Usage: prog FILENAME");
		exit(0);
	}
	FILE *input = xfopen(argv[1], "r");
	FILE *output = xfopen(argv[2], "w");
	char *line = NULL, *ptr;
	int i, size = 0;
	Ngram *ngrams = (Ngram*) xmalloc(START_MEM * sizeof(Ngram));

	while((line = read_line(input)) != NULL) {
		ptr = strrchr(line, ' ');
		if(ptr != NULL) {
			if(size >= START_MEM && (size & (size - 1)) == 0)
				ngrams = (Ngram*) xrealloc(ngrams, (size << 1) * sizeof(Ngram));
			ngrams[size].num_occurences = atoi(ptr);
			if(ngrams[size].num_occurences != 0) {
				size++;
				ngrams[size].data = line;
			} else
				fputs("Error: invalid line!", stderr);
		} else
			fputs("Error: invalid line!", stderr);
	}
	fclose(input);
	qsort(ngrams, size, sizeof(Ngram), (__compar_fn_t) &compare);

	for(i = 0; i < size; i++) {
		fprintf(output, "%s\n", ngrams->data);
		printf("%s\n", ngrams->data);
		free(ngrams->data);
	}
	free(ngrams);
	fclose(output);

	return 0;

}
