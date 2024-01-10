#include "Helper.h"

void* readFile(const char* spvFileName, size_t* fileSize) {
	FILE* fp;

	fopen_s(&fp, spvFileName, "rb");

	if (!fp)
		return nullptr;

	size_t returnValue;
	long int size;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	void* spvShader = malloc(size + 1);
	memset(spvShader, 0, size + 1);
	
	returnValue = fread(spvShader, size, 1, fp);
	assert(returnValue == 1);

	*fileSize = size;
	fclose(fp);

	return spvShader;
}