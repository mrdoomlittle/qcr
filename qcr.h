# ifndef __mdl__qcr__h
# define __mdl__qcr__h
# include <mdlint.h>
# include <err.h>

enum {
	_qc_vt_str,
	_qc_vt_int,
	_qc_vt_chr
};

struct qcr_val {
	mdl_u8_t kind;
	mdl_u8_t *p;
};

struct qcr_var {
	struct qcr_val val;
	char *name;
};

struct qcr_buff {
	mdl_u8_t *p, *itr;
	mdl_uint_t blk_size;
	mdl_uint_t size;
};

struct qcr_vec {
	mdl_u8_t *p;
	mdl_uint_t blk_size;
	mdl_uint_t elem_c, page_c;
};

struct qcr_map {
	struct qcr_vec **table;
};

struct qcr_array {
	char *name;
	struct qcr_vec data;
};

struct qcr {
	struct qcr_vec arrays;
	struct qcr_vec vars;
	struct qcr_map env;
	mdl_uint_t fsize;
	mdl_u8_t *fp, *f_itr;
	struct qcr_vec toks;
};

// map.c
mdl_err_t map_init(struct qcr_map*);
mdl_err_t map_de_init(struct qcr_map*);
mdl_err_t map_get(struct qcr_map*, mdl_u8_t const*, mdl_uint_t, void**);
mdl_err_t map_put(struct qcr_map*, mdl_u8_t const*, mdl_uint_t, void*);

// buff.c
mdl_err_t buff_init(struct qcr_buff*, mdl_uint_t, mdl_uint_t);
void buff_put(struct qcr_buff*, void*);
void buff_get(struct qcr_buff*, void*);
void buff_itr_incr(struct qcr_buff*);
void buff_itr_decr(struct qcr_buff*);
void buff_itr_reset(struct qcr_buff*);
mdl_uint_t buff_blk_c(struct qcr_buff*);
void buff_de_init(struct qcr_buff*);
// vec.c
mdl_err_t vec_init(struct qcr_vec*, mdl_uint_t);
void vec_push_back(struct qcr_vec*, void**);
void vec_pop_back(struct qcr_vec*, void*);
mdl_err_t vec_de_init(struct qcr_vec*);
void* vec_begin(struct qcr_vec*);
void* vec_end(struct qcr_vec*);
void* vec_get(struct qcr_vec*, mdl_uint_t);

// qcr.c
void* qcr_get_arr_elem(struct qcr*, void*, mdl_uint_t);
void* qcr_get(struct qcr*, char*);
mdl_err_t qcr_init(struct qcr*);
mdl_err_t qcr_de_init(struct qcr*);
mdl_err_t qcr_load(struct qcr*, char*);
mdl_err_t qcr_read(struct qcr*);
# endif /*__mdl__qcr__h*/
