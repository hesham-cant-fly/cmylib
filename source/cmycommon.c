CMYCOMMON_DEF void *m_xmalloc_(size_t size, int line, const char *file)
{
#ifdef CMYCOMMON_USE_CALLOC
	void *result = calloc(1, size);
#else
	void *result = malloc(size);
#endif
	if (result == NULL) {
		fprintf(stderr, "%s:%d: Not Enough Memory when calling malloc(%zu).\n", file, line, size);
		exit(1);
	}

	return result;
}

CMYCOMMON_DEF void *m_xrealloc_(void *ptr, size_t new_size, int line, const char *file)
{
	if (ptr == NULL) {
		return m_xmalloc_(new_size, line, file);
	}

	void *result = realloc(ptr, new_size);
	if (result == NULL) {
		fprintf(stderr, "%s:%d: Not Enough Memory when calling realloc.\n", file, line);
		exit(1);
	}

	return result;
}

CMYCOMMON_DEF void *m_xcalloc_(size_t number, size_t size, int line, const char *file)
{
	void *result = calloc(number, size);
	if (result == NULL) {
		fprintf(stderr, "%s:%d: Not Enough Memory when calling calloc(%zu, %zu).\n", file, line, number, size);
		exit(1);
	}

	return result;
}
