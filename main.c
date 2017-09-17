# include <stdio.h>
# include "qcr.h"

void print_var(struct qcr_var *__var) {
	if (__var->kind == _qc_vt_str)
		printf("name: %s, val: %s\n", __var->name, (char*)__var->val);
	else if (__var->kind == _qc_vt_int)
		printf("name: %s, val: %u\n", __var->name, *(mdl_uint_t*)__var->val);
	else if (__var->kind == _qc_vt_chr)
		printf("name: %s, val: %c\n", __var->name, *(char*)__var->val);
}

int main(void) {
	struct qcr _qcr;
	qcr_init(&_qcr);
	qcr_load(&_qcr, "test.qc");
	qcr_read(&_qcr);

	print_var(qcr_get_var(&_qcr, "version"));
	print_var(qcr_get_var(&_qcr, "created_by"));

	qcr_de_init(&_qcr);
}
