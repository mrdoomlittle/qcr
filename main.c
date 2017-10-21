# include <stdio.h>
# include "qcr.h"
void print_val(struct qcr_val *__val) {
	if (__val->kind == _qc_vt_str)
		printf("%s\n", (char*)__val->p);
	else if (__val->kind == _qc_vt_int)
		printf("%u\n", *(mdl_uint_t*)__val->p);
	else if (__val->kind == _qc_vt_chr)
		printf("%c\n", *(char*)__val->p);
}

int main(void) {
	struct qcr _qcr;
	qcr_init(&_qcr);
	qcr_load(&_qcr, "test.qc");
	qcr_read(&_qcr);
	void *arr = qcr_get(&_qcr, "hello");
	if (!arr) return -1;

	print_val((struct qcr_val*)qcr_get_arr_elem(&_qcr, arr, 0));
	print_val((struct qcr_val*)qcr_get_arr_elem(&_qcr, arr, 1));
//	print_var(qcr_get_var(&_qcr, "version"));
//	print_var(qcr_get_var(&_qcr, "created_by"));

	qcr_de_init(&_qcr);
}
