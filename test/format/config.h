/*-
 * Public Domain 2008-2014 WiredTiger, Inc.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Configuration for the wts program is an array of string-based parameters.
 * This is the structure used to declare them.
 */
typedef struct {
	const char	*name;			/* Configuration item */
	const char	*desc;			/* Configuration description */

	/* Value is a boolean, yes if roll of 1-to-100 is <= CONFIG->min. */
#define	C_BOOL		0x001

	/* Not a simple randomization, handle outside the main loop. */ 
#define	C_IGNORE	0x002

	/* Operation, only set if doing operations. */
#define	C_OPS		0x004

	/* Value was set from command-line or file, ignore for all runs. */
#define	C_PERM		0x008

	/* Value isn't random for this run, ignore just for this run. */
#define	C_TEMP		0x010

	/* Value is a string. */
#define	C_STRING	0x020
	u_int	 	flags;

	uint32_t	min;		/* Minimum value */
	uint32_t	maxrand;	/* Maximum value randomly chosen */
	uint32_t	maxset;		/* Maximum value explicitly set */
	uint32_t	*v;		/* Value for this run */
	char		**vstr;		/* Value for string options */
} CONFIG;

/*
 * Get a random value between a config min/max pair (inclusive for both min
 * and max).
 */
#define	CONF_RAND(cp)	MMRAND((cp)->min, (cp)->maxrand)

static CONFIG c[] = {
	{ "auto_throttle",
	  "if LSM inserts are throttled",			/* 90% */
	  C_BOOL, 90, 0, 0, &g.c_auto_throttle, NULL },

	{ "firstfit",
	  "if allocation is firstfit",				/* 10% */
	  C_BOOL, 10, 0, 0, &g.c_firstfit, NULL },

	{ "bitcnt",
	  "number of bits for fixed-length column-store files",
	  0x0, 1, 8, 8, &g.c_bitcnt, NULL },

	{ "bloom",
	  "if bloom filters are configured",			/* 95% */
	  C_BOOL, 95, 0, 0, &g.c_bloom, NULL },

	{ "bloom_bit_count",
	  "number of bits per item for LSM bloom filters",
	  0x0, 4, 64, 1000, &g.c_bloom_bit_count, NULL },

	{ "bloom_hash_count",
	  "number of hash values per item for LSM bloom filters",
	  0x0, 4, 32, 100, &g.c_bloom_hash_count, NULL },

	{ "bloom_oldest",
	  "if bloom_oldest=true",				/* 10% */
	  C_BOOL, 10, 0, 0, &g.c_bloom_oldest, NULL },

	{ "cache",
	  "size of the cache in MB",
	  0x0, 1, 100, 1024, &g.c_cache, NULL },

	{ "checkpoints",
	  "if periodic checkpoints are done",			/* 95% */
	  C_BOOL, 95, 0, 0, &g.c_checkpoints, NULL },

	{ "checksum",
	  "type of checksums (on | off | uncompressed)",
	  C_IGNORE|C_STRING, 1, 3, 3, NULL, &g.c_checksum },

	{ "chunk_size",
	  "LSM chunk size in MB",
	  0x0, 1, 10, 100, &g.c_chunk_size, NULL },

	{ "compaction",
	  "if compaction is running",				/* 10% */
	  C_BOOL, 10, 0, 0, &g.c_compact, NULL },

	{ "compression",
	  "type of compression (none | bzip | bzip-raw | lzo | snappy | zlib)",
	  C_IGNORE|C_STRING, 1, 5, 5, NULL, &g.c_compression },

	{ "data_extend",
	  "if data files are extended",				/* 5% */
	  C_BOOL, 5, 0, 0, &g.c_data_extend, NULL },

	{ "data_source",
	  "data source (file | helium | kvsbdb | lsm | table)",
	  C_IGNORE | C_STRING, 0, 0, 0, NULL, &g.c_data_source },

	{ "delete_pct",
	  "percent operations that are deletes",
	  C_OPS, 0, 45, 90, &g.c_delete_pct, NULL },

	{ "dictionary",
	  "if values are dictionary compressed",		/* 20% */
	  C_BOOL, 20, 0, 0, &g.c_dictionary, NULL },

	{ "file_type",
	  "type of store to create (fix | var | row)",
	  C_IGNORE|C_STRING, 1, 3, 3, NULL, &g.c_file_type },

	{ "hot_backups",
	  "if hot backups are enabled",				/* 5% */
	  C_BOOL, 5, 0, 0, &g.c_hot_backups, NULL },

	{ "huffman_key",
	  "if keys are huffman encoded",			/* 20% */
	  C_BOOL, 20, 0, 0, &g.c_huffman_key, NULL },

	{ "huffman_value",
	  "if values are huffman encoded",			/* 20% */
	  C_BOOL, 20, 0, 0, &g.c_huffman_value, NULL },

	{ "insert_pct",
	  "percent operations that are inserts",
	  C_OPS, 0, 45, 90, &g.c_insert_pct, NULL },

	{ "internal_key_truncation",
	  "if internal keys are truncated",			/* 95% */
	  C_BOOL, 95, 0, 0, &g.c_internal_key_truncation, NULL },

	{ "internal_page_max",
	  "maximum size of Btree internal nodes",
	  0x0, 9, 17, 27, &g.c_intl_page_max, NULL },

	{ "isolation",
	  "isolation level "
	  "(random | read-uncommitted | read-committed | snapshot)",
	  C_IGNORE|C_STRING, 1, 4, 4, NULL, &g.c_isolation },

	{ "key_gap",
	  "gap between instantiated keys on a Btree page",
	  0x0, 0, 20, 20, &g.c_key_gap, NULL },

	{ "key_max",
	  "maximum size of keys",
	  0x0, 20, 128, MEGABYTE(10), &g.c_key_max, NULL },

	{ "key_min",
	  "minimum size of keys",
	  0x0, 10, 32, 256, &g.c_key_min, NULL },

	{ "leak_memory",
	  "if memory should be leaked on close",
	  C_BOOL, 0, 0, 0, &g.c_leak_memory, NULL },

	{ "leaf_page_max",
	  "maximum size of Btree leaf nodes",
	  0x0, 9, 17, 27, &g.c_leaf_page_max, NULL },

	{ "logging",
	  "if logging configured",				/* 30% */
	  C_BOOL, 30, 0, 0, &g.c_logging, NULL },

	{ "merge_max",
	  "the maximum number of chunks to include in a merge operation",
	  0x0, 4, 20, 100, &g.c_merge_max, NULL },

	{ "merge_threads",
	  "the number of threads to perform merge operations",
	  0x0, 1, 4, 10, &g.c_merge_threads, NULL },

	{ "mmap",
	  "configure for mmap operations",			/* 90% */
	  C_BOOL, 90, 0, 0, &g.c_mmap, NULL },

	{ "ops",
	  "the number of modification operations done per run",
	  0x0, 0, M(2), M(100), &g.c_ops, NULL },

	{ "prefix_compression",
	  "if keys are prefix compressed",			/* 80% */
	  C_BOOL, 80, 0, 0, &g.c_prefix_compression, NULL },

	{ "prefix_compression_min",
	  "minimum gain before prefix compression is used",
	  0x0, 0, 8, 256, &g.c_prefix_compression_min, NULL },

	{ "repeat_data_pct",
	  "percent duplicate values in row- or var-length column-stores",
	  0x0, 0, 90, 90, &g.c_repeat_data_pct, NULL },

	{ "reverse",
	  "collate in reverse order",				/* 10% */
	  C_BOOL, 10, 0, 0, &g.c_reverse, NULL },

	{ "rows",
	  "the number of rows to create",
	  0x0, 10, M(1), M(100), &g.c_rows, NULL },

	{ "runs",
	  "the number of runs",
	  C_IGNORE, 0, UINT_MAX, UINT_MAX, &g.c_runs, NULL },

	{ "split_pct",
	  "page split size as a percentage of the maximum page size",
	  0x0, 40, 85, 85, &g.c_split_pct, NULL },

	{ "statistics",
	  "maintain statistics",				/* 20% */
	  C_BOOL, 20, 0, 0, &g.c_statistics, NULL },

	{ "threads",
	  "the number of threads",
	  C_IGNORE, 1, 32, 128, &g.c_threads, NULL },

	{ "value_max",
	  "maximum size of values",
	  0x0, 32, 4096, MEGABYTE(10), &g.c_value_max, NULL },

	{ "value_min",
	  "minimum size of values",
	  0x0, 0, 20, 4096, &g.c_value_min, NULL },

	{ "wiredtiger_config",
	  "configuration string used to wiredtiger_open",
	  C_IGNORE|C_STRING, 0, 0, 0, NULL, &g.c_config_open },

	{ "write_pct",
	  "percent operations that are writes",
	  C_OPS, 0, 90, 90, &g.c_write_pct, NULL },

	{ NULL, NULL, 0x0, 0, 0, 0, NULL, NULL }
};
