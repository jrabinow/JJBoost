#include "utils.h"

#define START_MEM	8192

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
	FILE *input;
	FILE *output;
	char *line = NULL, *ptr;
	int i, size = 0;
	Ngram *ngrams = (Ngram*) xmalloc(START_MEM * sizeof(Ngram));

	if(argc != 3) {
		puts("Usage: prog FILENAME");
		exit(0);
	}
	input = xfopen(argv[1], "r");
	output = xfopen(argv[2], "w");

	while((line = read_line(input)) != NULL) {
		ptr = strrchr(line, ' ');
		if(ptr != NULL) {
			if(size >= START_MEM && (size & (size - 1)) == 0) {
				ngrams = (Ngram*) xrealloc(ngrams, (size << 1) * sizeof(Ngram));
			}
			ngrams[size].num_occurences = atoi(ptr + 1);
			if(ngrams[size].num_occurences != 0) {
				ngrams[size++].data = line;
			} else
				fprintf(stderr, "Error: invalid line '%s' (%d)\n", line, size);
		} else
			fprintf(stderr, "Error: invalid line '%s' (%d)\n", line, size);
	}
	fclose(input);
	qsort(ngrams, size, sizeof(Ngram), (__compar_fn_t) &compare);

	for(i = 0; i < size; i++) {
		fprintf(output, "%s\n", ngrams[i].data);
		printf("%s\n", ngrams[i].data);
		free(ngrams[i].data);
	}
	free(ngrams);
	fclose(output);

	return 0;

}
