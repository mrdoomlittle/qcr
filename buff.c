# include "qcr.h"
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
mdl_err_t buff_init(struct qcr_buff *__buff, mdl_uint_t __size, mdl_uint_t __blk_size) {
	if ((__buff->p = (mdl_u8_t*)malloc(__size*__blk_size)) == NULL) {
		fprintf(stderr, "qcr, buff: failed to alloc memory.\n");
		return MDL_FAILURE;
	}

	__buff->itr = __buff->p;
	__buff->blk_size = __blk_size;
	__buff->size = __size;
	return MDL_SUCCESS;
}

mdl_uint_t buff_blk_c(struct qcr_buff *__buff){return ((__buff->itr-__buff->p)/__buff->blk_size);}
void buff_itr_reset(struct qcr_buff *__buff){__buff->itr = __buff->p;}
void buff_itr_incr(struct qcr_buff *__buff){__buff->itr+=__buff->blk_size;}
void buff_itr_decr(struct qcr_buff *__buff){__buff->itr-=__buff->blk_size;}
void buff_put(struct qcr_buff *__buff, void *__p) {memcpy(__buff->itr, __p, __buff->blk_size);}
void buff_get(struct qcr_buff *__buff, void *__p) {memcpy(__p, __buff->itr, __buff->blk_size);}
void buff_de_init(struct qcr_buff *__buff) {if (__buff->p != NULL) free(__buff->p);}
