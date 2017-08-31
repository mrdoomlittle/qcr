# include <stdio.h>
# include "qcr.h"
int main(void) {
	struct qcr _qcr;
	qcr_init(&_qcr);
	qcr_load(&_qcr, "test.qc");
	qcr_read(&_qcr);

	struct qcr_var *itr = (struct qcr_var*)vec_begin(&_qcr.vars);
	while(itr != (struct qcr_var*)vec_end(&_qcr.vars)) {
		if (itr->kind == _qc_vt_str) {
			printf("name: %s, val: %s\n", itr->name, (char*)itr->val);
		} else if (itr->kind == _qc_vt_int) {
			printf("name: %s, val: %u\n", itr->name, *(mdl_uint_t*)itr->val);
		} else if (itr->kind == _qc_vt_chr) {
			printf("name: %s, val: %c\n", itr->name, *(char*)itr->val);
		}

		itr++;
	}

	qcr_de_init(&_qcr);
}
