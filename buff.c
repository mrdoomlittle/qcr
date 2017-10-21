# include "qcr.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
mdl_err_t buff_init(struct buff *__buff, mdl_uint_t __size, mdl_uint_t __blk_size) {
	if ((__buff->p = (mdl_u8_t*)malloc(__size*__blk_size)) == NULL) {
		fprintf(stderr, "qcr, buff: failed to alloc memory.\n");
		return MDL_FAILURE;
	}

	__buff->itr = __buff->p;
	__buff->blk_size = __blk_size;
	__buff->size = __size;
	return MDL_SUCCESS;
}

mdl_uint_t buff_blk_c(struct buff *__buff){return ((__buff->itr-__buff->p)/__buff->blk_size);}
void buff_itr_reset(struct buff *__buff){__buff->itr = __buff->p;}
void buff_itr_mf(struct buff *__buff){__buff->itr+=__buff->blk_size;}
void buff_itr_mb(struct buff *__buff){__buff->itr-=__buff->blk_size;}
void buff_put(struct buff *__buff, void *__p) {memcpy(__buff->itr, __p, __buff->blk_size);}
void buff_get(struct buff *__buff, void *__p) {memcpy(__p, __buff->itr, __buff->blk_size);}
void buff_de_init(struct buff *__buff) {if (__buff->p != NULL) free(__buff->p);}
