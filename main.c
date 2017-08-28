# include <stdio.h>
# include "qcr.h"
int main(void) {
	struct qcr _qcr;
	qcr_init(&_qcr);
	qcr_load(&_qcr, "test.qc");
	qcr_read(&_qcr);

	qcr_de_init(&_qcr);
}
