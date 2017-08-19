#include "file.h"

void WriteToFile(const char* path, char* value){
	FILE* file;

	file = fopen(path, "w");
	if (file == NULL) return;
	fprintf(file, "%s", value);
	fclose(file);
}