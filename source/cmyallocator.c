
CMYALLOCATOR_DEF void *m_allocate_(allocator_t allocator, char *file, int line, size_t alignment, size_t size)
{
	return (allocator.vtable->allocate)(allocator.data, file, line, alignment, size);
}

CMYALLOCATOR_DEF void *m_reallocate_(allocator_t allocator, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size)
{
	if (ptr == NULL) {
		return m_allocate_(allocator, file, line, alignment, new_size);
	}
	return (allocator.vtable->reallocate)(allocator.data, file, line, old_size, ptr, alignment, new_size);
}

CMYALLOCATOR_DEF void m_deallocate_(allocator_t allocator, char *file, int line, size_t size, void *ptr)
{
	(allocator.vtable->deallocate)(allocator.data, file, line, size, ptr);
}

CMYALLOCATOR_DEF void *m_allocate_with_value_(allocator_t allocator, char *file, int line, size_t alignment, size_t size, void *value)
{
	void *result = m_allocate_(allocator, file, line, alignment, size);
	memcpy(result, value, size);
	return result;
}
