
#define o dlx->mems++
#define oo dlx->mems += 2
#define ooo dlx->mems += 3

#define O "%"
#define mod %

#define len itm
#define aux spare

#define invalid_sol_pos 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <gb_flip.h>

#include <lua.h>
#include <lauxlib.h>

#include "dlx1.h"

/*
    this algorithm (k=33) was first reported by dan bernstein many years ago in comp.lang.c.
    another version of this algorithm (now favored by bernstein) uses xor:
        hash(i) = hash(i - 1) * 33 ^ str[i]; the magic of number 33
    (why it works better than many other constants, prime or not) has never been adequately explained.
*/
lua_Integer
djb2_string_hash(char *str)
{

    lua_Integer hash = 5381;
    int c;

    while ((c = *str++) != 0)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void panic(dlx1State_t *dlx, int p, char *m, lua_State *L)
{
    fprintf(dlx->stream_err, "" O "s!\n" O "d: " O "s\n", m, p, dlx->buf);
    luaL_error(L, "" O "s!\n" O "d: " O "s", m, p, dlx->buf);
}

void print_option(lua_State *L, dlx1State_t *dlx, int p, FILE *stream, int sol_pos)
{
    int k, q;
    if (p < dlx->last_itm || p >= dlx->last_node || dlx->nd[p].itm <= 0)
    {
        fprintf(dlx->stream_err, "Illegal option " O "d!\n", p);
        return;
    }

    char *itm_name = NULL;

    lua_newtable(L);

    for (q = p;;)
    {
        itm_name = dlx->cl[dlx->nd[q].itm].name;

        lua_pushboolean(L, 1); // for now just use the Lua `true` as value for every key.
        lua_setfield(L, -2, itm_name);

        fprintf(stream, " " O "s", itm_name);
        q++;
        if (dlx->nd[q].itm <= 0)
            q = dlx->nd[q].up;
        if (q == p)
            break;
    }
    for (q = dlx->nd[dlx->nd[p].itm].down, k = 1; q != p; k++)
    {
        if (q == dlx->nd[p].itm)
        {
            fprintf(stream, " (?)\n");
            return;
        }
        else
            q = dlx->nd[q].down;
    }
    fprintf(stream, " (" O "d of " O "d)\n", k, dlx->nd[dlx->nd[p].itm].len);

    if (sol_pos != invalid_sol_pos)
    {
        lua_seti(L, -2, sol_pos);
    }
    else
    {
        lua_pop(L, 1);
    }
}

void prow(lua_State *L, dlx1State_t *dlx, int p)
{
    print_option(L, dlx, p, dlx->stream_err, invalid_sol_pos);
}

void print_itm(lua_State *L, dlx1State_t *dlx, int c)
{
    int p;
    if (c < root || c >= dlx->last_itm)
    {
        fprintf(dlx->stream_err, "Illegal item " O "d!\n", c);
        return;
    }
    if (c < dlx->second)
        fprintf(dlx->stream_err, "Item " O "s, length " O "d, neighbors " O "s and " O "s:\n",
                dlx->cl[c].name, dlx->nd[c].len, dlx->cl[dlx->cl[c].prev].name, dlx->cl[dlx->cl[c].next].name);
    else
        fprintf(dlx->stream_err, "Item " O "s, length " O "d:\n", dlx->cl[c].name, dlx->nd[c].len);
    for (p = dlx->nd[c].down; p >= dlx->last_itm; p = dlx->nd[p].down)
        prow(L, dlx, p);
}

void sanity(dlx1State_t *dlx)
{
    int k, p, q, pp, qq;
    for (q = root, p = dlx->cl[q].next;; q = p, p = dlx->cl[p].next)
    {
        if (dlx->cl[p].prev != q)
            fprintf(dlx->stream_err, "Bad prev field at itm " O "s!\n",
                    dlx->cl[p].name);
        if (p == root)
            break;

        for (qq = p, pp = dlx->nd[qq].down, k = 0;; qq = pp, pp = dlx->nd[pp].down, k++)
        {
            if (dlx->nd[pp].up != qq)
                fprintf(dlx->stream_err, "Bad up field at node " O "d!\n", pp);
            if (pp == p)
                break;
            if (dlx->nd[pp].itm != p)
                fprintf(dlx->stream_err, "Bad itm field at node " O "d!\n", pp);
        }
        if (dlx->nd[p].len != k)
            fprintf(dlx->stream_err, "Bad len field in item " O "s!\n",
                    dlx->cl[p].name);
    }
}

void cover(dlx1State_t *dlx, int c)
{
    int cc, l, r, rr, nn, uu, dd, t;
    o, l = dlx->cl[c].prev, r = dlx->cl[c].next;
    oo, dlx->cl[l].next = r, dlx->cl[r].prev = l;
    dlx->updates++;
    for (o, rr = dlx->nd[c].down; rr >= dlx->last_itm; o, rr = dlx->nd[rr].down)
        for (nn = rr + 1; nn != rr;)
        {
            o, uu = dlx->nd[nn].up, dd = dlx->nd[nn].down;
            o, cc = dlx->nd[nn].itm;
            if (cc <= 0)
            {
                nn = uu;
                continue;
            }
            oo, dlx->nd[uu].down = dd, dlx->nd[dd].up = uu;
            dlx->updates++;
            o, t = dlx->nd[cc].len - 1;
            o, dlx->nd[cc].len = t;
            nn++;
        }
}

void uncover(dlx1State_t *dlx, int c)
{
    int cc, l, r, rr, nn, uu, dd, t;
    for (o, rr = dlx->nd[c].down; rr >= dlx->last_itm; o, rr = dlx->nd[rr].down)
        for (nn = rr + 1; nn != rr;)
        {
            o, uu = dlx->nd[nn].up, dd = dlx->nd[nn].down;
            o, cc = dlx->nd[nn].itm;
            if (cc <= 0)
            {
                nn = uu;
                continue;
            }
            oo, dlx->nd[uu].down = dlx->nd[dd].up = nn;
            o, t = dlx->nd[cc].len + 1;
            o, dlx->nd[cc].len = t;
            nn++;
        }
    o, l = dlx->cl[c].prev, r = dlx->cl[c].next;
    oo, dlx->cl[l].next = dlx->cl[r].prev = c;
}

void print_state(lua_State *L, dlx1State_t *dlx)
{
    int l;
    fprintf(dlx->stream_err, "Current state (level " O "d):\n", dlx->level);
    for (l = 0; l < dlx->level; l++)
    {
        print_option(L, dlx, dlx->choice[l], dlx->stream_err, invalid_sol_pos);
        if (l >= dlx->show_levels_max)
        {
            fprintf(dlx->stream_err, " ...\n");
            break;
        }
    }
    fprintf(dlx->stream_err, " " O "lld solutions, " O "lld mems, and max level " O "d so far.\n",
            dlx->count, dlx->mems, dlx->maxl);
}

void print_progress(dlx1State_t *dlx)
{
    int l, k, d, c, p;
    double f, fd;
    fprintf(dlx->stream_err, " after " O "lld mems: " O "lld sols,", dlx->mems, dlx->count);
    for (f = 0.0, fd = 1.0, l = 0; l < dlx->level; l++)
    {
        c = dlx->nd[dlx->choice[l]].itm, d = dlx->nd[c].len;
        for (k = 1, p = dlx->nd[c].down; p != dlx->choice[l]; k++, p = dlx->nd[p].down)
            ;
        fd *= d, f += (k - 1) / fd;
        fprintf(dlx->stream_err, " " O "c" O "c",
                k < 10 ? '0' + k : k < 36 ? 'a' + k - 10
                               : k < 62   ? 'A' + k - 36
                                          : '*',
                d < 10 ? '0' + d : d < 36 ? 'a' + d - 10
                               : d < 62   ? 'A' + d - 36
                                          : '*');
        if (l >= dlx->show_levels_max)
        {
            fprintf(dlx->stream_err, "...");
            break;
        }
    }
    fprintf(dlx->stream_err, " " O ".5f\n", f + 0.5 / fd);
}

typedef struct
{
    int argc;
    char **argv;
    int close_flags;
    int cc, i, j, k, p, pp, q, r, t, cur_node, best_itm;
} dlx1KContext_t;

int dlx1_kfunction(lua_State *L, int status, lua_KContext ctx)
{
    dlx1State_t *dlx = lua_touserdata(L, lua_upvalueindex(1));
    dlx1KContext_t *kcontext = lua_touserdata(L, lua_upvalueindex(2));

    if (status == LUA_YIELD)
        goto resume;

    kcontext->p = kcontext->best_itm = -1;

    dlx->random_seed = 0;
    dlx->vbose = show_basics + show_warnings;
    dlx->show_choices_max = 1000000;
    dlx->show_choices_gap = 1000000;
    dlx->show_levels_max = 1000000;
    dlx->maxl = 0;
    dlx->thresh = 10000000000;
    dlx->delta = 10000000000;
    dlx->maxcount = 0xffffffffffffffff;
    dlx->timeout = 0x1fffffffffffffff;
    dlx->second = max_cols;

    for (kcontext->j = kcontext->argc - 1, kcontext->k = 0; kcontext->j; kcontext->j--)
        switch (kcontext->argv[kcontext->j][0])
        {
        case 'v':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "d", &(dlx->vbose)) - 1);
            break;
        case 'm':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "d", &(dlx->spacing)) - 1);
            break;
        case 's':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "d", &(dlx->random_seed)) - 1), dlx->randomizing = 1;
            break;
        case 'd':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "lld", &(dlx->delta)) - 1), dlx->thresh = dlx->delta;
            break;
        case 'c':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "d", &(dlx->show_choices_max)) - 1);
            break;
        case 'C':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "d", &(dlx->show_levels_max)) - 1);
            break;
        case 'l':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "d", &(dlx->show_choices_gap)) - 1);
            break;
        case 't':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "lld", &(dlx->maxcount)) - 1);
            break;
        case 'T':
            kcontext->k |= (sscanf(kcontext->argv[kcontext->j] + 1, "" O "lld", &(dlx->timeout)) - 1);
            break;
        case 'S':
            dlx->shape_name = kcontext->argv[kcontext->j] + 1, dlx->shape_file = fopen(dlx->shape_name, "w");
            if (!dlx->shape_file)
                fprintf(dlx->stream_err, "Sorry, I can't open file `" O "s' for writing!\n",
                        dlx->shape_name);
            break;
        default:
            kcontext->k = 1;
        }
    if (kcontext->k)
    {
        fprintf(dlx->stream_err, "Usage: " O "s [v<n>] [m<n>] [s<n>] [d<n>]"
                                 " [c<n>] [C<n>] [l<n>] [t<n>] [T<n>] [S<bar>] < foo.dlx\n",
                kcontext->argv[0]);

        luaL_error(L, "Usage: " O "s [v<n>] [m<n>] [s<n>] [d<n>]"
                      " [c<n>] [C<n>] [l<n>] [t<n>] [T<n>] [S<bar>] < foo.dlx",
                   kcontext->argv[0]);
    }
    if (dlx->randomizing)
        gb_init_rand(dlx->random_seed);

    if (max_nodes <= 2 * max_cols)
    {
        fprintf(dlx->stream_err, "Recompile me: max_nodes must exceed twice max_cols!\n");
        luaL_error(L, "Recompile me: max_nodes must exceed twice max_cols!");
    }
    while (1)
    {
        if (!fgets(dlx->buf, bufsize, dlx->stream_in))
            break;
        if (o, dlx->buf[kcontext->p = strlen(dlx->buf) - 1] != '\n')
            panic(dlx, kcontext->p, "Input line way too long", L);
        for (kcontext->p = 0; o, isspace(dlx->buf[kcontext->p]); kcontext->p++)
            ;
        if (dlx->buf[kcontext->p] == '|' || !dlx->buf[kcontext->p])
            continue;
        dlx->last_itm = 1;
        break;
    }
    if (!dlx->last_itm)
        panic(dlx, kcontext->p, "No items", L);
    for (; o, dlx->buf[kcontext->p];)
    {
        for (kcontext->j = 0; kcontext->j < max_name_length && (o, !isspace(dlx->buf[kcontext->p + kcontext->j])); kcontext->j++)
        {
            if (dlx->buf[kcontext->p + kcontext->j] == ':' || dlx->buf[kcontext->p + kcontext->j] == '|')
                panic(dlx, kcontext->p, "Illegal character in item name", L);
            o, dlx->cl[dlx->last_itm].name[kcontext->j] = dlx->buf[kcontext->p + kcontext->j];
        }
        if (kcontext->j == max_name_length && !isspace(dlx->buf[kcontext->p + kcontext->j]))
            panic(dlx, kcontext->p, "Item name too long", L);

        for (kcontext->k = 1; o, strncmp(dlx->cl[kcontext->k].name, dlx->cl[dlx->last_itm].name, max_name_length); kcontext->k++)
            ;
        if (kcontext->k < dlx->last_itm)
            panic(dlx, kcontext->p, "Duplicate item name", L);

        if (dlx->last_itm > max_cols)
            panic(dlx, kcontext->p, "Too many items", L);
        oo, dlx->cl[dlx->last_itm - 1].next = dlx->last_itm, dlx->cl[dlx->last_itm].prev = dlx->last_itm - 1;

        o, dlx->nd[dlx->last_itm].up = dlx->nd[dlx->last_itm].down = dlx->last_itm;
        dlx->last_itm++;

        for (kcontext->p += kcontext->j + 1; o, isspace(dlx->buf[kcontext->p]); kcontext->p++)
            ;
        if (dlx->buf[kcontext->p] == '|')
        {
            if (dlx->second != max_cols)
                panic(dlx, kcontext->p, "Item name line contains | twice", L);
            dlx->second = dlx->last_itm;
            for (kcontext->p++; o, isspace(dlx->buf[kcontext->p]); kcontext->p++)
                ;
        }
    }
    if (dlx->second == max_cols)
        dlx->second = dlx->last_itm;
    oo, dlx->cl[dlx->last_itm].prev = dlx->last_itm - 1, dlx->cl[dlx->last_itm - 1].next = dlx->last_itm;
    oo, dlx->cl[dlx->second].prev = dlx->last_itm, dlx->cl[dlx->last_itm].next = dlx->second;

    oo, dlx->cl[root].prev = dlx->second - 1, dlx->cl[dlx->second - 1].next = root;
    dlx->last_node = dlx->last_itm;

    while (1)
    {
        if (!fgets(dlx->buf, bufsize, dlx->stream_in))
            break;
        if (o, dlx->buf[kcontext->p = strlen(dlx->buf) - 1] != '\n')
            panic(dlx, kcontext->p, "Option line too long", L);
        for (kcontext->p = 0; o, isspace(dlx->buf[kcontext->p]); kcontext->p++)
            ;
        if (dlx->buf[kcontext->p] == '|' || !dlx->buf[kcontext->p])
            continue;
        kcontext->i = dlx->last_node;
        for (kcontext->pp = 0; dlx->buf[kcontext->p];)
        {
            for (kcontext->j = 0; kcontext->j < max_name_length && (o, !isspace(dlx->buf[kcontext->p + kcontext->j])); kcontext->j++)
                o, dlx->cl[dlx->last_itm].name[kcontext->j] = dlx->buf[kcontext->p + kcontext->j];
            if (kcontext->j == max_name_length && !isspace(dlx->buf[kcontext->p + kcontext->j]))
                panic(dlx, kcontext->p, "Item name too long", L);
            if (kcontext->j < max_name_length)
                o, dlx->cl[dlx->last_itm].name[kcontext->j] = '\0';

            for (kcontext->k = 0; o, strncmp(dlx->cl[kcontext->k].name, dlx->cl[dlx->last_itm].name, max_name_length); kcontext->k++)
                ;
            if (kcontext->k == dlx->last_itm)
                panic(dlx, kcontext->p, "Unknown item name", L);
            if (o, dlx->nd[kcontext->k].aux >= kcontext->i)
                panic(dlx, kcontext->p, "Duplicate item name in this option", L);
            dlx->last_node++;
            if (dlx->last_node == max_nodes)
                panic(dlx, kcontext->p, "Too many nodes", L);
            o, dlx->nd[dlx->last_node].itm = kcontext->k;
            if (kcontext->k < dlx->second)
                kcontext->pp = 1;
            o, kcontext->t = dlx->nd[kcontext->k].len + 1;

            o, dlx->nd[kcontext->k].len = kcontext->t;
            dlx->nd[kcontext->k].aux = dlx->last_node;
            if (!dlx->randomizing)
            {
                o, kcontext->r = dlx->nd[kcontext->k].up;
                ooo, dlx->nd[kcontext->r].down = dlx->nd[kcontext->k].up = dlx->last_node, dlx->nd[dlx->last_node].up = kcontext->r, dlx->nd[dlx->last_node].down = kcontext->k;
            }
            else
            {
                dlx->mems += 4, kcontext->t = gb_unif_rand(kcontext->t);
                for (o, kcontext->r = kcontext->k; kcontext->t; o, kcontext->r = dlx->nd[kcontext->r].down, kcontext->t--)
                    ;
                ooo, kcontext->q = dlx->nd[kcontext->r].up, dlx->nd[kcontext->q].down = dlx->nd[kcontext->r].up = dlx->last_node;
                o, dlx->nd[dlx->last_node].up = kcontext->q, dlx->nd[dlx->last_node].down = kcontext->r;
            }

            for (kcontext->p += kcontext->j + 1; o, isspace(dlx->buf[kcontext->p]); kcontext->p++)
                ;
        }
        if (!kcontext->pp)
        {
            if (dlx->vbose & show_warnings)
                fprintf(dlx->stream_err, "Option ignored (no primary items): " O "s", dlx->buf);
            while (dlx->last_node > kcontext->i)
            {

                o, kcontext->k = dlx->nd[dlx->last_node].itm;
                oo, dlx->nd[kcontext->k].len--, dlx->nd[kcontext->k].aux = kcontext->i - 1;
                o, kcontext->q = dlx->nd[dlx->last_node].up, kcontext->r = dlx->nd[dlx->last_node].down;
                oo, dlx->nd[kcontext->q].down = kcontext->r, dlx->nd[kcontext->r].up = kcontext->q;

                dlx->last_node--;
            }
        }
        else
        {
            o, dlx->nd[kcontext->i].down = dlx->last_node;
            dlx->last_node++;
            if (dlx->last_node == max_nodes)
                panic(dlx, kcontext->p, "Too many nodes", L);
            dlx->options++;
            o, dlx->nd[dlx->last_node].up = kcontext->i + 1;
            o, dlx->nd[dlx->last_node].itm = -dlx->options;
        }
    }

    if (dlx->vbose & show_basics)
        fprintf(dlx->stream_err,
                "(" O "lld options, " O "d+" O "d items, " O "d entries successfully read)\n",
                dlx->options, dlx->second - 1, dlx->last_itm - dlx->second, dlx->last_node - dlx->last_itm);

    if (dlx->vbose & show_tots)
    {
        fprintf(dlx->stream_err, "Item totals:");
        for (kcontext->k = 1; kcontext->k < dlx->last_itm; kcontext->k++)
        {
            if (kcontext->k == dlx->second)
                fprintf(dlx->stream_err, " |");
            fprintf(dlx->stream_err, " " O "d", dlx->nd[kcontext->k].len);
        }
        fprintf(dlx->stream_err, "\n");
    }

    dlx->imems = dlx->mems, dlx->mems = 0;

    dlx->level = 0;
forward:
    dlx->nodes++;
    if (dlx->vbose & show_profile)
        dlx->profile[dlx->level]++;
    if (dlx->sanity_checking)
        sanity(dlx);

    if (dlx->delta && (dlx->mems >= dlx->thresh))
    {
        dlx->thresh += dlx->delta;
        if (dlx->vbose & show_full_state)
            print_state(L, dlx);
        else
            print_progress(dlx);
    }
    if (dlx->mems >= dlx->timeout)
    {
        fprintf(dlx->stream_err, "TIMEOUT!\n");
        goto done;
    }

    dlx->tmems = dlx->mems, kcontext->t = max_nodes;
    if ((dlx->vbose & show_details) &&
        dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
        fprintf(dlx->stream_err, "level " O "d:", dlx->level);
    for (o, kcontext->k = dlx->cl[root].next; kcontext->t && kcontext->k != root; o, kcontext->k = dlx->cl[kcontext->k].next)
    {
        if ((dlx->vbose & show_details) &&
            dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
            fprintf(dlx->stream_err, " " O "s(" O "d)", dlx->cl[kcontext->k].name, dlx->nd[kcontext->k].len);
        if (o, dlx->nd[kcontext->k].len <= kcontext->t)
        {
            if (dlx->nd[kcontext->k].len < kcontext->t)
                kcontext->best_itm = kcontext->k, kcontext->t = dlx->nd[kcontext->k].len, kcontext->p = 1;
            else
            {
                kcontext->p++;
                if (dlx->randomizing && (dlx->mems += 4, !gb_unif_rand(kcontext->p)))
                    kcontext->best_itm = kcontext->k;
            }
        }
    }
    if ((dlx->vbose & show_details) &&
        dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
        fprintf(dlx->stream_err, " branching on " O "s(" O "d)\n", dlx->cl[kcontext->best_itm].name, kcontext->t);
    if (kcontext->t > dlx->maxdeg)
        dlx->maxdeg = kcontext->t;
    if (dlx->shape_file)
    {
        fprintf(dlx->shape_file, "" O "d " O "s\n", kcontext->t, dlx->cl[kcontext->best_itm].name);
        fflush(dlx->shape_file);
    }
    dlx->cmems += dlx->mems - dlx->tmems;

    cover(dlx, kcontext->best_itm);
    oo, kcontext->cur_node = dlx->choice[dlx->level] = dlx->nd[kcontext->best_itm].down;
advance:
    if (kcontext->cur_node == kcontext->best_itm)
        goto backup;
    if ((dlx->vbose & show_choices) && dlx->level < dlx->show_choices_max)
    {
        fprintf(dlx->stream_err, "L" O "d:", dlx->level);
        print_option(L, dlx, kcontext->cur_node, dlx->stream_err, invalid_sol_pos);
    }

    for (kcontext->pp = kcontext->cur_node + 1; kcontext->pp != kcontext->cur_node;)
    {
        o, kcontext->cc = dlx->nd[kcontext->pp].itm;
        if (kcontext->cc <= 0)
            o, kcontext->pp = dlx->nd[kcontext->pp].up;
        else
            cover(dlx, kcontext->cc), kcontext->pp++;
    }

    if (o, dlx->cl[root].next == root)
    {
        dlx->nodes++;
        if (dlx->level + 1 > dlx->maxl)
        {
            if (dlx->level + 1 >= max_level)
            {
                fprintf(dlx->stream_err, "Too many levels!\n");
                luaL_error(L, "Too many levels!");
            }
            dlx->maxl = dlx->level + 1;
        }
        if (dlx->vbose & show_profile)
            dlx->profile[dlx->level + 1]++;
        if (dlx->shape_file)
        {
            fprintf(dlx->shape_file, "sol\n");
            fflush(dlx->shape_file);
        }

        {
            dlx->count++;
            if (dlx->spacing && (dlx->count mod dlx->spacing == 0))
            {
                fprintf(dlx->stream_out, "" O "lld:\n", dlx->count);

                lua_newtable(L);

                lua_pushinteger(L, dlx->updates);
                lua_setfield(L, -2, "updates");

                lua_newtable(L);

                for (kcontext->k = 0; kcontext->k <= dlx->level; kcontext->k++)
                    print_option(L, dlx, dlx->choice[kcontext->k], dlx->stream_out, kcontext->k + 1);
                fflush(dlx->stream_out);

                lua_setfield(L, -2, "options");

                lua_yieldk(L, 1, ctx, &dlx1_kfunction);
            resume:
            }
            if (dlx->count >= dlx->maxcount)
                goto done;
            goto recover;
        }
    }

    if (++dlx->level > dlx->maxl)
    {
        if (dlx->level >= max_level)
        {
            fprintf(dlx->stream_err, "Too many levels!\n");
            luaL_error(L, "Too many levels!");
        }
        dlx->maxl = dlx->level;
    }
    goto forward;
backup:
    uncover(dlx, kcontext->best_itm);
    if (dlx->level == 0)
        goto done;
    dlx->level--;
    oo, kcontext->cur_node = dlx->choice[dlx->level], kcontext->best_itm = dlx->nd[kcontext->cur_node].itm;
recover:

    for (kcontext->pp = kcontext->cur_node - 1; kcontext->pp != kcontext->cur_node;)
    {
        o, kcontext->cc = dlx->nd[kcontext->pp].itm;
        if (kcontext->cc <= 0)
            o, kcontext->pp = dlx->nd[kcontext->pp].down;
        else
            uncover(dlx, kcontext->cc), kcontext->pp--;
    }

    oo, kcontext->cur_node = dlx->choice[dlx->level] = dlx->nd[kcontext->cur_node].down;
    goto advance;

done:
    if (dlx->vbose & show_tots)
    {
        fprintf(dlx->stream_err, "Item totals:");
        for (kcontext->k = 1; kcontext->k < dlx->last_itm; kcontext->k++)
        {
            if (kcontext->k == dlx->second)
                fprintf(dlx->stream_err, " |");
            fprintf(dlx->stream_err, " " O "d", dlx->nd[kcontext->k].len);
        }
        fprintf(dlx->stream_err, "\n");
    }

    if (dlx->vbose & show_profile)
    {
        fprintf(dlx->stream_err, "Profile:\n");
        for (dlx->level = 0; dlx->level <= dlx->maxl; dlx->level++)
            fprintf(dlx->stream_err, "" O "3d: " O "lld\n",
                    dlx->level, dlx->profile[dlx->level]);
    }

    if (dlx->vbose & show_max_deg)
        fprintf(dlx->stream_err, "The maximum branching degree was " O "d.\n", dlx->maxdeg);
    if (dlx->vbose & show_basics)
    {
        fprintf(dlx->stream_err, "Altogether " O "llu solution" O "s, " O "llu+" O "llu mems,",
                dlx->count, dlx->count == 1 ? "" : "s", dlx->imems, dlx->mems);
        dlx->bytes = dlx->last_itm * sizeof(dlx1Item_t) + dlx->last_node * sizeof(dlx1Node_t) + dlx->maxl * sizeof(int);
        fprintf(dlx->stream_err, " " O "llu updates, " O "llu bytes, " O "llu nodes,",
                dlx->updates, dlx->bytes, dlx->nodes);
        fprintf(dlx->stream_err, " ccost " O "lld%%.\n",
                (200 * dlx->cmems + dlx->mems) / (2 * dlx->mems));
    }

    if (dlx->shape_file)
        fclose(dlx->shape_file);

    if (kcontext->close_flags & 1)
    {
        fclose(dlx->stream_in);
    }

    if (kcontext->close_flags & 2)
    {
        fclose(dlx->stream_out);
    }

    if (kcontext->close_flags & 4)
    {
        fclose(dlx->stream_err);
    }

    free(dlx);
    free(kcontext->argv);
    free(kcontext);

    return 0;
}

int dlx1_closure(lua_State *L)
{
    // call the continuation function
    return dlx1_kfunction(L, LUA_OK, 0);
}

char *string_trim(size_t size, const char *orig, size_t *n)
{
    size_t i, j;

    for (i = 0; isspace(*(orig + i)); i++)
        ;

    for (j = size - 1; j > i && isspace(*(orig + j)); j--)
        ;

    *n = j - i + 1;
    char *dest = (char *)malloc(sizeof(char) * (*n + 1));
    dest[*n] = '\0';
    return strncpy(dest, orig + i, *n);
}

/*
    This function consumes a table of strings that denotes the arguments to the solver.
*/
int l_coroutine(lua_State *L)
{
    int type;

    dlx1KContext_t *kcontext = (dlx1KContext_t *)malloc(sizeof(dlx1KContext_t));
    kcontext->close_flags = 0;
    kcontext->argc = lua_tointeger(L, 1);
    kcontext->argv = (char **)malloc(sizeof(char *) * kcontext->argc);

    for (int i = 1; i <= kcontext->argc; i++)
    {
        type = lua_rawgeti(L, 2, i);
        assert(type == LUA_TSTRING);

        kcontext->argv[i - 1] = (char *)lua_tostring(L, -1);
        lua_pop(L, 1);
    }

    dlx1State_t *dlx = (dlx1State_t *)malloc(sizeof(dlx1State_t));

    // stdin
    if (lua_isnoneornil(L, 3))
    {
        dlx->stream_in = stdin;
    }
    else if (lua_type(L, 3) == LUA_TSTRING)
    {
        dlx->stream_in = fopen(lua_tostring(L, 3), "r");
        kcontext->close_flags |= 1;
    }
    else if (lua_type(L, 3) == LUA_TTABLE)
    {
        type = lua_getfield(L, 3, "literal");

        if (type == LUA_TSTRING)
        {
            size_t literal_size, trimmed_size;
            const char *literal_content = lua_tolstring(L, -1, &literal_size);

            char *trimmed = string_trim(literal_size, literal_content, &trimmed_size);

            char *tmp_filename = tmpnam(NULL);

            dlx->stream_in = fopen(tmp_filename, "w+");
            assert(fwrite(trimmed, sizeof(char), trimmed_size, dlx->stream_in) == trimmed_size);
            assert(fwrite("\n", sizeof(char), 1, dlx->stream_in) == 1);
            assert(fflush(dlx->stream_in) == 0);
            rewind(dlx->stream_in);

            kcontext->close_flags |= 1;

            free(trimmed);
        }
        else
        {
            luaL_error(L, "Not valid argument for tabled stdin.");
        }

        lua_pop(L, 1);
    }
    else
    {
        luaL_error(L, "Not valid argument for stdin.");
    }

    // stdout
    if (lua_isnoneornil(L, 4))
    {
        dlx->stream_out = stdout;
    }
    else if (lua_type(L, 4) == LUA_TSTRING)
    {
        dlx->stream_out = fopen(lua_tostring(L, 4), "w");
        kcontext->close_flags |= 2;
    }
    else
    {
        luaL_error(L, "Not valid argument for stdout.");
    }

    // stderr
    if (lua_isnoneornil(L, 5))
    {
        dlx->stream_err = stderr;
    }
    else if (lua_type(L, 5) == LUA_TSTRING)
    {
        dlx->stream_err = fopen(lua_tostring(L, 5), "w");
        kcontext->close_flags |= 4;
    }
    else
    {
        luaL_error(L, "Not valid argument for stderr.");
    }

    dlx->sanity_checking = lua_toboolean(L, 6);

    lua_State *S = lua_newthread(L);

    lua_pushlightuserdata(S, dlx);
    lua_pushlightuserdata(S, kcontext);
    lua_pushcclosure(S, &dlx1_closure, 2);

    return 1;
}

const struct luaL_Reg libdlx1[] = {
    {"coroutine", l_coroutine},
    {NULL, NULL} /* sentinel */
};

void push_show_constants(lua_State *L)
{

    lua_newtable(L);

    lua_pushinteger(L, show_basics);
    lua_setfield(L, -2, "basics");

    lua_pushinteger(L, show_choices);
    lua_setfield(L, -2, "choices");

    lua_pushinteger(L, show_details);
    lua_setfield(L, -2, "details");

    lua_pushinteger(L, show_full_state);
    lua_setfield(L, -2, "full_state");

    lua_pushinteger(L, show_max_deg);
    lua_setfield(L, -2, "max_deg");

    lua_pushinteger(L, show_profile);
    lua_setfield(L, -2, "profile");

    lua_pushinteger(L, show_tots);
    lua_setfield(L, -2, "tots");

    lua_pushinteger(L, show_warnings);
    lua_setfield(L, -2, "warnings");

    lua_setfield(L, -2, "show"); // store the new table into the final module.
}

int luaopen_libdlx1(lua_State *L) // the initialization function of the module.
{
    luaL_newlib(L, libdlx1);

    push_show_constants(L);

    return 1;
}