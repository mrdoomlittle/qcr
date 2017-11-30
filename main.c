# include <stdio.h>
# include <mdl/qcr.h>
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
	if (qcr_init(&_qcr) != MDL_SUCCESS) {
		fprintf(stderr, "failed to init qcr.\n");
		return -1;
	}

	if (qcr_load(&_qcr, "test.qc") != MDL_SUCCESS) {
		fprintf(stderr, "failed to load file.\n");
		return -1;
	}

	if (qcr_read(&_qcr) != MDL_SUCCESS) {
		fprintf(stderr, "failed to read.\n");
		return -1;
	}

	void *arr = qcr_get(&_qcr, "info");
	if (!arr) return -1;

	print_val((struct qcr_val*)qcr_get_arr_elem(&_qcr, arr, 0));
	print_val((struct qcr_val*)qcr_get_arr_elem(&_qcr, arr, 1));
	print_val((struct qcr_val*)qcr_get(&_qcr, "git_url"));

//	print_var(qcr_get_var(&_qcr, "version"));
//	print_var(qcr_get_var(&_qcr, "created_by"));
	if (qcr_de_init(&_qcr) != MDL_SUCCESS) {
		fprintf(stderr, "failed to de init qcr.\n");
		return -1;
	}
	return 0;
}
