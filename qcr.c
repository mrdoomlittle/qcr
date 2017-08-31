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
struct token {
	mdl_u8_t kind, id;
	char *s_val, *s;
	mdl_uint_t i_val;
	char c_val;
};

struct buff tok_ib;
struct buff tmp_buff;
mdl_err_t qcr_init(struct qcr *__qcr) {
	vec_init(&__qcr->toks, sizeof(struct token));
	buff_init(&tmp_buff, 80, sizeof(mdl_u8_t));
	buff_init(&tok_ib, 20, sizeof(struct token*));
	vec_init(&__qcr->vars, sizeof(struct qcr_var));
}

mdl_u8_t static ignore_space(struct qcr *__qcr) {
	if (*__qcr->f_itr == ' ' || *__qcr->f_itr == '\n') return 1;
	return 0;
}

void build_token(struct token *__tok, struct token *__tmpl) {
	if (__tmpl != NULL) *__tok = *__tmpl;
}

void make_ident(struct token *__tok, char *__val) {
	build_token(__tok, &(struct token){.kind=TOK_IDENT, .s_val=__val});
}

void make_keyword(struct token *__tok, mdl_u8_t __id) {
	build_token(__tok, &(struct token){.kind=TOK_KEYWORD, .id=__id});
}

void make_str(struct token *__tok, char *__s) {
	build_token(__tok, &(struct token){.kind=TOK_STR, .s=__s});
}

void make_no(struct token *__tok, mdl_uint_t __val) {
	build_token(__tok, &(struct token){.kind=TOK_NO, .i_val=__val});
}

void make_chr(struct token *__tok, char __val) {
    build_token(__tok, &(struct token){.kind=TOK_CHR, .c_val=__val});
}

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
	while((*__qcr->f_itr >= 'a' && *__qcr->f_itr <= 'z') || *__qcr->f_itr == '_' || (*__qcr->f_itr >= '0' && *__qcr->f_itr <= '9')) {
		buff_put(&tmp_buff, __qcr->f_itr++);
		buff_itr_mf(&tmp_buff);
	}

	buff_put(&tmp_buff, "\0");
	buff_itr_mf(&tmp_buff);

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
		buff_itr_mf(&tmp_buff);
	}

	buff_put(&tmp_buff, "\0");
	buff_itr_mf(&tmp_buff);

	mdl_uint_t size = buff_blk_c(&tmp_buff);
	char *p = (char*)malloc(size);
	buff_itr_reset(&tmp_buff);

	memcpy(p, tmp_buff.p, size);
	return p;
}

mdl_uint_t read_no(struct qcr *__qcr) {
	while(*__qcr->f_itr >= '0' && *__qcr->f_itr <= '9') {
		buff_put(&tmp_buff, __qcr->f_itr++);
		buff_itr_mf(&tmp_buff);
	}

	buff_put(&tmp_buff, "\0");
	buff_itr_mf(&tmp_buff);
	mdl_uint_t ret = qcr_str_to_int(tmp_buff.p);
	buff_itr_reset(&tmp_buff);
	return ret;
}

char read_chr(struct qcr *__qcr) {
	__qcr->f_itr++;
	__qcr->f_itr++;
	return *(__qcr->f_itr-1);

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
		buff_itr_mb(&tok_ib);
		buff_get(&tok_ib, &tok);
		return tok;
	}
	return _read_token(__qcr);
}

void uread_token(struct qcr *__qcr, struct token *__tok) {
	buff_put(&tok_ib, &__tok);
	buff_itr_mf(&tok_ib);
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
}

mdl_err_t qcr_de_init(struct qcr *__qcr) {
	if (__qcr->fp != NULL) free(__qcr->fp);
	vec_de_init(&__qcr->toks);
	buff_de_init(&tmp_buff);
}

mdl_u8_t qcr_expect_tok(struct qcr *__qcr, mdl_u8_t __kind, mdl_u8_t __id) {
	struct token *tok = read_token(__qcr);
	return ((tok->kind == __kind) && (tok->id == __id));
}

mdl_u8_t* qcr_read_literal(struct qcr *__qcr, mdl_u8_t *__kind) {
	struct token *val = read_token(__qcr);
	switch((*__kind = val->kind)) {
		case TOK_STR:
			return (mdl_u8_t*)val->s;
		break;
		case TOK_NO:
			return (mdl_u8_t*)&val->i_val;
		break;
		case TOK_CHR:
			return (mdl_u8_t*)&val->c_val;
		break;
	}
	return NULL;
}

mdl_err_t qcr_read_decl(struct qcr *__qcr) {
	struct token *name = read_token(__qcr);
	if (!qcr_expect_tok(__qcr, TOK_KEYWORD, COLON)) {
		printf("expected colon.\n");
	}

	mdl_u8_t kind, var_kind;
	mdl_u8_t *val = qcr_read_literal(__qcr, &kind);
	switch(kind) {
		case TOK_STR: var_kind = _qc_vt_str; break;
		case TOK_NO: var_kind = _qc_vt_int; break;
		case TOK_CHR: var_kind = _qc_vt_chr; break;
	}

	struct qcr_var *var = NULL;
	vec_push_back(&__qcr->vars, (void**)&var);
	*var = (struct qcr_var){.kind=var_kind, .name=name->s_val, .val=val};

	if (!qcr_expect_tok(__qcr, TOK_KEYWORD, COMMA)) {
		printf("expected comma.\n");
	}
}

mdl_err_t _qcr_read(struct qcr *__qcr) {
	struct token *tok = peek_token(__qcr);
	if (tok == NULL) return MDL_SUCCESS;
	switch(tok->kind) {
		case TOK_IDENT:
			qcr_read_decl(__qcr);
		break;
	}
}

mdl_err_t qcr_read(struct qcr *__qcr) {
	while(__qcr->f_itr < __qcr->fp+__qcr->fsize) {
		_qcr_read(__qcr);
	}
}
