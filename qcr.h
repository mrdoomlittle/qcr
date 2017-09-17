# ifndef __qcr__h
# define __qcr__h
# include <mdlint.h>
# include <err.h>

enum {
	_qc_vt_str,
	_qc_vt_int,
	_qc_vt_chr
};

struct qcr_var {
	mdl_u8_t kind;
	char *name;
	mdl_u8_t *val;
};

struct buff {
	mdl_u8_t *p, *itr;
	mdl_uint_t blk_size;
	mdl_uint_t size;
};

struct vec {
	mdl_u8_t *p;
	mdl_uint_t blk_size;
	mdl_uint_t elem_c, page_c;
};

struct map {
	struct vec **table;
};

struct qcr {
	struct vec vars;
	struct map env;
	mdl_uint_t fsize;
	mdl_u8_t *fp, *f_itr;
	struct vec toks;
};

// map.c
mdl_err_t map_init(struct map*);
mdl_err_t map_de_init(struct map*);
mdl_err_t map_get(struct map*, mdl_u8_t const*, mdl_uint_t, void**);
mdl_err_t map_put(struct map*, mdl_u8_t const*, mdl_uint_t, void*);

// buff.c
mdl_err_t buff_init(struct buff*, mdl_uint_t, mdl_uint_t);
void buff_put(struct buff*, void*);
void buff_get(struct buff*, void*);
void buff_itr_mf(struct buff*);
void buff_itr_mb(struct buff*);
void buff_itr_reset(struct buff*);
mdl_uint_t buff_blk_c(struct buff*);
mdl_err_t buff_de_init(struct buff*);
// vec.c
mdl_err_t vec_init(struct vec*, mdl_uint_t);
void vec_push_back(struct vec*, void**);
void vec_pop_back(struct vec*, void*);
mdl_err_t vec_de_init(struct vec*);
void* vec_begin(struct vec*);
void* vec_end(struct vec*);

// qcr.c
struct qcr_var* qcr_get_var(struct qcr*, char*);
mdl_err_t qcr_init(struct qcr*);
mdl_err_t qcr_de_init(struct qcr*);
mdl_err_t qcr_load(struct qcr*, char*);
mdl_err_t qcr_read(struct qcr*);
# endif /*__qcr__h*/
