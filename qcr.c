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
};

# define COLON 0x0
struct token {
	mdl_u8_t kind, id;
	char *val, *str;
};

struct buff tmp_buff;
mdl_err_t qcr_init(struct qcr *__qcr) {
	vec_init(&__qcr->toks, sizeof(struct token));
	buff_init(&tmp_buff, 200, sizeof(mdl_u8_t));
}

mdl_u8_t static ignore_space(struct qcr *__qcr) {
	if (*__qcr->f_itr == ' ' || *__qcr->f_itr == '\n') return 1;
	return 0;
}

void build_token(struct token *__tok, struct token *__tmpl) {
	if (__tmpl != NULL) *__tok = *__tmpl;
}

void make_ident(struct token *__tok, char *__val) {
	build_token(__tok, &(struct token){.kind = TOK_IDENT, .val = __val});
}

void make_keyword(struct token *__tok, mdl_u8_t __id) {
	build_token(__tok, &(struct token){.kind = TOK_KEYWORD, .id = __id});
}

void make_str(struct token *__tok, char *__str) {
	build_token(__tok, &(struct token){.kind = TOK_STR, .str = __str});
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

struct token static* read_token(struct qcr *__qcr) {
	while(ignore_space(__qcr)) __qcr->f_itr++;

	struct token *tok = NULL;
	vec_push_back(&__qcr->toks, (void**)&tok);

	switch(*__qcr->f_itr) {
		case '"':
			make_str(tok, read_str(__qcr));
			__qcr->f_itr++;
		break;
		case ':':
			make_keyword(tok, COLON);
			__qcr->f_itr++;
		break;
		default:
			if ((*__qcr->f_itr >= 'a' && *__qcr->f_itr <= 'z') || *__qcr->f_itr == '_') {
				make_ident(tok, read_ident(__qcr));
				break;
			}

			printf("%x\n", *__qcr->f_itr);
			__qcr->f_itr++;
			vec_pop_back(&__qcr->toks, NULL);
			return NULL;
	}
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

mdl_err_t qcr_read(struct qcr *__qcr) {
	while(__qcr->f_itr < __qcr->fp+__qcr->fsize) {
		struct token *tok = read_token(__qcr);
		if (tok == NULL) continue;
		if (tok->kind == TOK_IDENT) {
			printf("val: %s\n", tok->val);
		} else if (tok->kind == TOK_KEYWORD) {
			printf("val: %c\n", tok->id == COLON? ':':' ');
		}
	}
}
