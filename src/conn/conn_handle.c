/*-
 * Copyright (c) 2008-2014 WiredTiger, Inc.
 *	All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 */

#include "wt_internal.h"

/*
 * __wt_connection_init --
 *	Structure initialization for a just-created WT_CONNECTION_IMPL handle.
 */
int
__wt_connection_init(WT_CONNECTION_IMPL *conn)
{
	WT_SESSION_IMPL *session;
	u_int i;

	session = conn->default_session;

	SLIST_INIT(&conn->dhlh);		/* Data handle list */
	TAILQ_INIT(&conn->dlhqh);		/* Library list */
	TAILQ_INIT(&conn->dsrcqh);		/* Data source list */
	TAILQ_INIT(&conn->fhqh);		/* File list */
	TAILQ_INIT(&conn->collqh);		/* Collator list */
	TAILQ_INIT(&conn->compqh);		/* Compressor list */

	TAILQ_INIT(&conn->lsmqh);		/* WT_LSM_TREE list */

	/* Configuration. */
	WT_RET(__wt_conn_config_init(session));

	/* Statistics. */
	__wt_stat_init_connection_stats(&conn->stats);

	/* Locks. */
	WT_RET(__wt_spin_init(session, &conn->api_lock, "api"));
	WT_RET(__wt_spin_init(session, &conn->checkpoint_lock, "checkpoint"));
	WT_RET(__wt_spin_init(session, &conn->dhandle_lock, "data handle"));
	WT_RET(__wt_spin_init(session, &conn->fh_lock, "file list"));
	WT_RET(__wt_spin_init(session, &conn->hot_backup_lock, "hot backup"));
	WT_RET(__wt_spin_init(session, &conn->schema_lock, "schema"));
	WT_RET(__wt_calloc_def(session, WT_PAGE_LOCKS(conn), &conn->page_lock));
	for (i = 0; i < WT_PAGE_LOCKS(conn); ++i)
		WT_RET(
		    __wt_spin_init(session, &conn->page_lock[i], "btree page"));

	/*
	 * Generation numbers.
	 *
	 * Start split generations at one.  Threads publish this generation
	 * number before examining tree structures, and zero when they leave.
	 * We need to distinguish between threads that are in a tree before the
	 * first split has happened, and threads that are not in a tree.
	 */
	conn->split_gen = 1;

	/*
	 * Block manager.
	 * XXX
	 * If there's ever a second block manager, we'll want to make this
	 * more opaque, but for now this is simpler.
	 */
	WT_RET(__wt_spin_init(session, &conn->block_lock, "block manager"));
	TAILQ_INIT(&conn->blockqh);		/* Block manager list */

	return (0);
}

/*
 * __wt_connection_destroy --
 *	Destroy the connection's underlying WT_CONNECTION_IMPL structure.
 */
int
__wt_connection_destroy(WT_CONNECTION_IMPL *conn)
{
	WT_DECL_RET;
	WT_SESSION_IMPL *session;
	u_int i;

	/* Check there's something to destroy. */
	if (conn == NULL)
		return (0);

	session = conn->default_session;

	/*
	 * Close remaining open files (before discarding the mutex, the
	 * underlying file-close code uses the mutex to guard lists of
	 * open files.
	 */
	if (conn->lock_fh != NULL)
		WT_TRET(__wt_close(session, conn->lock_fh));

	/* Remove from the list of connections. */
	__wt_spin_lock(session, &__wt_process.spinlock);
	TAILQ_REMOVE(&__wt_process.connqh, conn, q);
	__wt_spin_unlock(session, &__wt_process.spinlock);

	/* Configuration */
	__wt_conn_config_discard(session);		/* configuration */

	__wt_conn_foc_discard(session);			/* free-on-close */

	__wt_spin_destroy(session, &conn->api_lock);
	__wt_spin_destroy(session, &conn->block_lock);
	__wt_spin_destroy(session, &conn->checkpoint_lock);
	__wt_spin_destroy(session, &conn->dhandle_lock);
	__wt_spin_destroy(session, &conn->fh_lock);
	__wt_spin_destroy(session, &conn->hot_backup_lock);
	__wt_spin_destroy(session, &conn->schema_lock);
	for (i = 0; i < WT_PAGE_LOCKS(conn); ++i)
		__wt_spin_destroy(session, &conn->page_lock[i]);
	__wt_free(session, conn->page_lock);

	/* Free allocated memory. */
	__wt_free(session, conn->home);
	__wt_free(session, conn->error_prefix);
	__wt_free(session, conn->sessions);

	__wt_free(NULL, conn);
	return (ret);
}
