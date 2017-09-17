# include "qcr.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
void *mem_dupe(void *__src, mdl_uint_t __size) {
	void *ret = malloc(__size);
	memcpy(ret, __src, __size);
	return ret;
}

typedef struct {
	mdl_u32_t val;
	void *data;
	mdl_uint_t bc;
	mdl_u8_t *key;
} map_entry_t;

mdl_u32_t map_hash(mdl_u8_t const *__key, mdl_uint_t __bc) {
	mdl_u8_t const *itr = __key;
	mdl_u32_t ret_val = 2<<(__bc>>2);
	while(itr != __key+__bc) {
		ret_val = (((ret_val>>1)+1)*(ret_val<<2))+(*itr*(((itr-__key)+1)<<1));
		itr++;
	}

	return ret_val;
}

map_entry_t *map_find(struct map *__map, mdl_u8_t const *__key, mdl_uint_t __bc, mdl_u32_t __val) {
	struct vec **map_blk = __map->table+*__key;
	if (*map_blk == NULL) return NULL;

	map_entry_t *itr = (map_entry_t*)vec_begin(*map_blk);
	while(itr <= (map_entry_t*)vec_end(*map_blk)) {
		if (itr->val == __val && itr->bc == __bc) {
			if (!memcmp(itr->key, __key, __bc)) {
				return itr;
			}
		}

		itr++;
	}
	return NULL;
}

mdl_err_t map_get(struct map *__map, mdl_u8_t const *__key, mdl_uint_t __bc, void **__data) {
	struct vec **map_blk = __map->table+*__key;
	if (*map_blk == NULL) return MDL_FAILURE;
	mdl_u32_t val = map_hash(__key, __bc);
	*__data = map_find(__map, __key, __bc, val)->data;
	return MDL_SUCCESS;
}


mdl_err_t map_put(struct map *__map, mdl_u8_t const *__key, mdl_uint_t __bc, void *__data) {
	mdl_u32_t val = map_hash(__key, __bc);
	if (map_find(__map, __key, __bc, val) != NULL) return MDL_FAILURE;

	struct vec **map_blk = __map->table+*__key;
	if (*map_blk == NULL) {
		*map_blk = (struct vec*)malloc(sizeof(struct vec));
		vec_init(*map_blk, sizeof(map_entry_t));
	}

	map_entry_t *entry;
	vec_push_back(*map_blk, (void**)&entry);
	entry->val = val;
	entry->data = __data;
	entry->bc = __bc;
	entry->key = (mdl_u8_t*)mem_dupe((void*)__key, __bc);
	return MDL_SUCCESS;
}

mdl_err_t map_init(struct map *__map) {
	__map->table = (struct vec**)malloc(((mdl_u8_t)~0)*sizeof(struct vec*));
	struct vec **itr = __map->table;
	while(itr != __map->table+(mdl_u8_t)~0) *(itr++) = NULL;
}

mdl_err_t map_de_init(struct map *__map) {
	struct vec **itr = __map->table;
	while(itr != __map->table+(mdl_u8_t)~0) {
		if (*itr != NULL) vec_de_init(*itr);
		itr++;
	}

	free(__map->table);
}
