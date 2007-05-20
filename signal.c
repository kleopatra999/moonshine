#include <glib.h>
#include <stdio.h>
#include <slang.h>
#include "config.h"
#include "signal.h"
#include "lua.h"

static GHashTable *names;
static int sigin, sigout;
static GIOChannel *channel;
static lua_State *lua_state;

static gboolean on_input(GIOChannel *i, GIOCondition c, gpointer p);
static void on_signal(int sig);

void signal_init(lua_State *L)
{
	int fildes[2];
	if (pipe(fildes) != 0)
		perror("pipe");
	sigin   = fildes[0];
	sigout  = fildes[1];
	channel = g_io_channel_unix_new(sigin);
	names   = g_hash_table_new(g_direct_hash, g_direct_equal);
	lua_state = L;
	g_io_add_watch(channel, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL, on_input, NULL);
}

void signal_reset(void)
{
	g_io_channel_unref(channel);
	close(sigin);
	close(sigout);
}

void signal_catch_full(int sig, const char *name)
{
	g_hash_table_insert(names, GINT_TO_POINTER(sig), (gpointer) name);
	SLsignal(sig, on_signal);
}

static gboolean on_input(UNUSED GIOChannel *i, GIOCondition c, UNUSED gpointer p)
{
	int sig;
	switch (c) {
		case G_IO_IN:
			read(sigin, &sig, sizeof(sig));
			char *signame = g_hash_table_lookup(names, GINT_TO_POINTER(sig));
			g_assert(signame != NULL);
			lua_getglobal(lua_state, "on_signal");
			lua_pushstring(lua_state, signame);
			lua_call(lua_state, 1, 0);
			return TRUE;
		default:
			return FALSE;
	}
}

static void on_signal(int sig)
{
	write(sigout, &sig, sizeof(sig));
	SLsignal(sig, on_signal);
}