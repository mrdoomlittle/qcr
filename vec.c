# include "qcr.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# define PAGE_SIZE 20
mdl_err_t vec_init(struct vec *__vec, mdl_uint_t __blk_size) {
	*__vec = (struct vec) {
		.blk_size = __blk_size,
		.elem_c = 0,
		.page_c = 0
	};

	if ((__vec->p = (mdl_u8_t*)malloc((++__vec->page_c)*(PAGE_SIZE*__blk_size))) == NULL) {
		fprintf(stderr, "qcr, vec: failed to alloc memory.\n");
		return MDL_FAILURE;
	}
}

void vec_push_back(struct vec *__vec, void **__p) {
	if (((__vec->page_c*PAGE_SIZE)-(__vec->elem_c*__vec->blk_size)) > (PAGE_SIZE*__vec->blk_size)) {
		__vec->p = (mdl_u8_t*)realloc(__vec->p, (++__vec->page_c)*(PAGE_SIZE*__vec->blk_size));
	}

	if (__p != NULL)
		*__p = (void*)(__vec->p+(__vec->elem_c*__vec->blk_size));
	__vec->elem_c++;
}

void vec_pop_back(struct vec *__vec, void *__p) {
	if (__p != NULL)
		memcpy(__p, __vec->p+(__vec->elem_c*__vec->blk_size), __vec->blk_size);

	if (((__vec->page_c*PAGE_SIZE)-((__vec->elem_c-1)*__vec->blk_size)) < (PAGE_SIZE*__vec->blk_size)) {
		__vec->p = (mdl_u8_t*)realloc(__vec->p, (--__vec->page_c)*(PAGE_SIZE*__vec->blk_size));
	}

	__vec->elem_c--;
}

mdl_err_t vec_de_init(struct vec *__vec) {
	if (__vec->p != NULL) free(__vec->p);
}
