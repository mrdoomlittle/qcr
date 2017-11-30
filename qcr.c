# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdio.h>
# include <sys/stat.h>
# include "qcr.h"
# include <string.h>

enum {
	TOK_IDENT,
	TOK_KEYWORD,
	TOK_STR,
	TOK_NO,
	TOK_CHR
};

# define COLON 0x0
# define COMMA 0x1
# define L_BRACKET 0x2
# define R_BRACKET 0x3
struct token {
	mdl_u8_t kind, id;
	void *p;
};

void* qcr_get(struct qcr *__qcr, char *__name) {
	void *ret;
	map_get(&__qcr->env, (mdl_u8_t const*)__name, strlen(__name), (void**)&ret);
	return ret;
}

void* qcr_get_arr_elem(struct qcr *__qcr, void *__p, mdl_uint_t __no) {
	struct qcr_array *arr = (struct qcr_array*)__p;
	return *(void**)vec_get(&arr->data, __no);
}

struct qcr_buff tok_ib;
struct qcr_buff tmp_buff;
mdl_err_t qcr_init(struct qcr *__qcr) {
	vec_init(&__qcr->toks, sizeof(struct token));
	buff_init(&tmp_buff, 80, sizeof(mdl_u8_t));
	buff_init(&tok_ib, 40, sizeof(struct token*));
	vec_init(&__qcr->vars, sizeof(struct qcr_var));
	vec_init(&__qcr->arrays, sizeof(struct qcr_array));
	map_init(&__qcr->env);
	return MDL_SUCCESS;
}

mdl_u8_t static ignore_space(struct qcr *__qcr) {
	if (*__qcr->f_itr == ' ' || *__qcr->f_itr == '\n') return 1;
	return 0;
}

void build_token(struct token *__tok, struct token *__tmpl) {
	if (__tmpl != NULL) *__tok = *__tmpl;
}

void make_ident(struct token *__tok, char *__s) {
	build_token(__tok, &(struct token){.kind=TOK_IDENT, .p=(void*)__s});}
void make_keyword(struct token *__tok, mdl_u8_t __id) {
	build_token(__tok, &(struct token){.kind=TOK_KEYWORD, .id=__id});}
void make_str(struct token *__tok, char *__s) {
	build_token(__tok, &(struct token){.kind=TOK_STR, .p=(void*)__s});}
void make_no(struct token *__tok, char *__s) {
	build_token(__tok, &(struct token){.kind=TOK_NO, .p=(void*)__s});}
void make_chr(struct token *__tok, char *__s) {
    build_token(__tok, &(struct token){.kind=TOK_CHR, .p=(void*)__s});}

mdl_uint_t qcr_str_to_int(char *__s) {
	char *itr = __s;
	mdl_uint_t ret_val = 0, no_unit = 1;
	for (mdl_uint_t ic = 0;; ic++) {
		if (__s[ic+1] == '\0') break;
		no_unit = no_unit*10;
	}

	while(*itr != '\0') {
		switch(*itr) {
			case '0': break;
			case '1': ret_val += 1*no_unit; break;
			case '2': ret_val += 2*no_unit; break;
			case '3': ret_val += 3*no_unit; break;
			case '4': ret_val += 4*no_unit; break;
			case '5': ret_val += 5*no_unit; break;
			case '6': ret_val += 6*no_unit; break;
			case '7': ret_val += 7*no_unit; break;
			case '8': ret_val += 8*no_unit; break;
			case '9': ret_val += 9*no_unit; break;
		}

		if (no_unit != 1) no_unit = no_unit/10;
		itr++;
	}
	return ret_val;
}

char* read_ident(struct qcr *__qcr) {
	while((*__qcr->f_itr >= 'a' && *__qcr->f_itr <= 'z') || *__qcr->f_itr == '_' || (*__qcr->f_itr >= '0' && *__qcr->f_itr <= '9') || (*__qcr->f_itr >= 'A' && *__qcr->f_itr <= 'Z')) {
		buff_put(&tmp_buff, __qcr->f_itr++);
		buff_itr_incr(&tmp_buff);
	}

	buff_put(&tmp_buff, "\0");
	buff_itr_incr(&tmp_buff);

	mdl_uint_t size = buff_blk_c(&tmp_buff);
	char *p = (char*)malloc(size);
	buff_itr_reset(&tmp_buff);

	memcpy(p, tmp_buff.p, size);
	return p;
}

char* read_str(struct qcr *__qcr) {
	__qcr->f_itr++;
	while(*__qcr->f_itr != '"') {
		buff_put(&tmp_buff, __qcr->f_itr++);
		buff_itr_incr(&tmp_buff);
	}

	buff_put(&tmp_buff, "\0");
	buff_itr_incr(&tmp_buff);

	mdl_uint_t size = buff_blk_c(&tmp_buff);
	char *p = (char*)malloc(size);
	buff_itr_reset(&tmp_buff);

	memcpy(p, tmp_buff.p, size);
	return p;
}

char* read_no(struct qcr *__qcr) {
	while(*__qcr->f_itr >= '0' && *__qcr->f_itr <= '9') {
		buff_put(&tmp_buff, __qcr->f_itr++);
		buff_itr_incr(&tmp_buff);
	}

	buff_put(&tmp_buff, "\0");
	buff_itr_incr(&tmp_buff);

	mdl_uint_t size = buff_blk_c(&tmp_buff);
	char *p = (char*)malloc(size);
	buff_itr_reset(&tmp_buff);

	memcpy(p, tmp_buff.p, size);
	return p;
}

char* read_chr(struct qcr *__qcr) {
	__qcr->f_itr++;
	__qcr->f_itr++;
	return __qcr->f_itr-1;
}

struct token* _read_token(struct qcr *__qcr) {
	while(ignore_space(__qcr)) __qcr->f_itr++;

	struct token *tok = NULL;
	vec_push_back(&__qcr->toks, (void**)&tok);

	switch(*__qcr->f_itr) {
		case '"':
			make_str(tok, read_str(__qcr));
			__qcr->f_itr++;
		break;
		case 0x27:
			make_chr(tok, read_chr(__qcr));
			__qcr->f_itr++;
		break;
		case ':':
			make_keyword(tok, COLON);
			__qcr->f_itr++;
		break;
		case ',':
			make_keyword(tok, COMMA);
			__qcr->f_itr++;
		break;
		case '[':
			make_keyword(tok, L_BRACKET);
			__qcr->f_itr++;
		break;
		case ']':
			make_keyword(tok, R_BRACKET);
			__qcr->f_itr++;
		break;
		default:
			if ((*__qcr->f_itr >= 'a' && *__qcr->f_itr <= 'z') || *__qcr->f_itr == '_') {
				make_ident(tok, read_ident(__qcr));
				break;
			} else if ((*__qcr->f_itr >= '0' && *__qcr->f_itr <= '9')) {
				make_no(tok, read_no(__qcr));
				break;
			}
			__qcr->f_itr++;
			vec_pop_back(&__qcr->toks, NULL);
			return NULL;
	}
	return tok;
}

struct token* read_token(struct qcr *__qcr) {
	if (buff_blk_c(&tok_ib) > 0) {
		struct token *tok;
		buff_itr_decr(&tok_ib);
		buff_get(&tok_ib, &tok);
		return tok;
	}
	return _read_token(__qcr);
}

void uread_token(struct qcr *__qcr, struct token *__tok) {
	buff_put(&tok_ib, &__tok);
	buff_itr_incr(&tok_ib);
}

struct token* peek_token(struct qcr *__qcr) {
	struct token *tok = read_token(__qcr);
	uread_token(__qcr, tok);
	return tok;
}

mdl_err_t qcr_load(struct qcr *__qcr, char *__fpth) {
	if (access(__fpth, 0) < 0) {
		fprintf(stderr, "qcr, file does not exist or access was denied.\n");
		return MDL_FAILURE;
	}

	int fd;
	if ((fd = open(__fpth, O_RDONLY)) < 0) {
		fprintf(stderr, "qcr, failed to open file at '%s'.\n", __fpth);
		return MDL_FAILURE;
	}

	struct stat st;
	if (stat(__fpth, &st) < 0) {
		fprintf(stderr, "qcr, failed to stat file at '%s'.\n", __fpth);
		return MDL_FAILURE;
	}

	__qcr->f_itr = __qcr->fp = (mdl_u8_t*)malloc(st.st_size);
	read(fd, __qcr->fp, (__qcr->fsize = st.st_size));
	close(fd);
	return MDL_SUCCESS;
}

mdl_err_t qcr_de_init(struct qcr *__qcr) {
	if (__qcr->fp != NULL) free(__qcr->fp);
	vec_de_init(&__qcr->toks);
	buff_de_init(&tmp_buff);
	map_de_init(&__qcr->env);
	return MDL_SUCCESS;
}

mdl_u8_t qcr_expect_tok(struct qcr *__qcr, mdl_u8_t __kind, mdl_u8_t __id) {
	struct token *tok = read_token(__qcr);
	return (tok->kind == __kind && tok->id == __id);
}

mdl_u8_t* qcr_read_literal(struct qcr *__qcr, mdl_u8_t *__kind) {
	struct token *val = read_token(__qcr);
	*__kind = val->kind;
	if (val->kind == TOK_NO) {
		mdl_uint_t *i_val = (mdl_uint_t*)malloc(sizeof(mdl_uint_t));
		*i_val = qcr_str_to_int((char*)val->p);
		return (mdl_u8_t*)i_val;
	}
	return (mdl_u8_t*)val->p;
}

mdl_u8_t next_token_is(struct qcr *__qcr, mdl_u8_t __kind, mdl_u8_t __id) {
	struct token *tok = read_token(__qcr);
	if (tok->kind == __kind && tok->id == __id) return 1;
	uread_token(__qcr, tok);
	return 0;
}

mdl_err_t qcr_read_val(struct qcr *__qcr, struct qcr_val *__val) {
	__val->p = qcr_read_literal(__qcr, &__val->kind);
	switch(__val->kind) {
		case TOK_STR: __val->kind = _qc_vt_str; break;
		case TOK_NO: __val->kind = _qc_vt_int; break;
		case TOK_CHR: __val->kind = _qc_vt_chr; break;
		default:
			return MDL_FAILURE;
	}
	return MDL_SUCCESS;
}

mdl_err_t qcr_read_arr(struct qcr *__qcr, struct qcr_vec *__data) {
	vec_init(__data, sizeof(void*));
	_again:
	if (next_token_is(__qcr, TOK_KEYWORD, L_BRACKET)) {
		struct qcr_array **arr;
		vec_push_back(__data, (void**)&arr);

		*arr = (struct qcr_array*)malloc(sizeof(struct qcr_array));
		if (qcr_read_arr(__qcr, &(*arr)->data) != MDL_SUCCESS)
			return MDL_FAILURE;
		goto _again;
	}

	struct token *tok = peek_token(__qcr);
	if (tok->kind == TOK_STR || tok->kind == TOK_NO || tok->kind == TOK_CHR) {
		struct qcr_val **val;

		vec_push_back(__data, (void**)&val);
		*val = (struct qcr_val*)malloc(sizeof(struct qcr_val));
		if (qcr_read_val(__qcr, *val) != MDL_SUCCESS)
			return MDL_FAILURE;
	}

	if (next_token_is(__qcr, TOK_KEYWORD, R_BRACKET)) {
		return MDL_SUCCESS;
	}

	if (!qcr_expect_tok(__qcr, TOK_KEYWORD, COMMA)) {
		fprintf(stdout, "expected comma.\n");
		return MDL_FAILURE;
	}
	goto _again;
	return MDL_SUCCESS;
}

mdl_err_t qcr_read_decl(struct qcr *__qcr) {
	struct token *name = read_token(__qcr);
	if (!qcr_expect_tok(__qcr, TOK_KEYWORD, COLON)) {
		printf("expected colon.\n");
		return MDL_FAILURE;
	}

	void *p;
	if (next_token_is(__qcr, TOK_KEYWORD, L_BRACKET)) {
		struct qcr_array *arr;
		vec_push_back(&__qcr->arrays, (void**)&arr);
		qcr_read_arr(__qcr, &arr->data);

		arr->name = (char*)name->p;

		p = (void*)arr;
	} else {
		struct qcr_val val;
		qcr_read_val(__qcr, &val);

		struct qcr_var *var = NULL;
		vec_push_back(&__qcr->vars, (void**)&var);
		*var = (struct qcr_var){.name=(char*)name->p, .val=val};
		p = (void*)var;
	}

	map_put(&__qcr->env, (char*)name->p, strlen((char*)name->p), p);
	free(name->p);

	if (!qcr_expect_tok(__qcr, TOK_KEYWORD, COMMA)) {
		printf("expected comma.\n");
		return MDL_FAILURE;
	}
	return MDL_SUCCESS;
}

mdl_err_t _qcr_read(struct qcr *__qcr) {
	mdl_err_t any_err = MDL_SUCCESS;
	struct token *tok = peek_token(__qcr);
	if (tok == NULL) return MDL_SUCCESS;
	switch(tok->kind) {
		case TOK_IDENT:
			any_err = qcr_read_decl(__qcr);
		break;
		default:
			return MDL_FAILURE;
	}
	return any_err;
}

mdl_err_t qcr_read(struct qcr *__qcr) {
	while(__qcr->f_itr < __qcr->fp+__qcr->fsize) {
		if (_qcr_read(__qcr) != MDL_SUCCESS)
			return MDL_FAILURE;
	}
	return MDL_SUCCESS;
}
