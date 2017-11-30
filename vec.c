# include "qcr.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# define PAGE_SIZE 86
mdl_err_t vec_init(struct qcr_vec *__vec, mdl_uint_t __blk_size) {
	*__vec = (struct qcr_vec) {
		.blk_size = __blk_size,
		.elem_c = 0,
		.page_c = 0
	};

	if ((__vec->p = (mdl_u8_t*)malloc((++__vec->page_c)*(PAGE_SIZE*__blk_size))) == NULL) {
		fprintf(stderr, "qcr, vec: failed to alloc memory.\n");
		return MDL_FAILURE;
	}
}

void vec_push_back(struct qcr_vec *__vec, void **__p) {
	if (((__vec->page_c*(PAGE_SIZE*__vec->blk_size))-((__vec->elem_c+1)*__vec->blk_size)) > (PAGE_SIZE*__vec->blk_size))
		__vec->p = (mdl_u8_t*)realloc(__vec->p, (++__vec->page_c)*(PAGE_SIZE*__vec->blk_size));
	if (__p != NULL) *__p = (void*)(__vec->p+(__vec->elem_c*__vec->blk_size));
	__vec->elem_c++;
}

void vec_pop_back(struct qcr_vec *__vec, void *__p) {
	if (__p != NULL) memcpy(__p, __vec->p+(__vec->elem_c*__vec->blk_size), __vec->blk_size);
	if (((__vec->page_c*(PAGE_SIZE*__vec->blk_size))-((__vec->elem_c-1)*__vec->blk_size)) < (PAGE_SIZE*__vec->blk_size) && __vec->page_c > 1)
		__vec->p = (mdl_u8_t*)realloc(__vec->p, (--__vec->page_c)*(PAGE_SIZE*__vec->blk_size));
	__vec->elem_c--;
}

void* vec_get(struct qcr_vec *__vec, mdl_uint_t __off) {
	return (void*)(__vec->p+(__off*__vec->blk_size));
}

void* vec_begin(struct qcr_vec *__vec) {return __vec->p;}
void* vec_end(struct qcr_vec *__vec) {return __vec->p+(__vec->elem_c*__vec->blk_size);}
mdl_err_t vec_de_init(struct qcr_vec *__vec) {
	if (__vec->p != NULL) free(__vec->p);
}
