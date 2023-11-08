
#define o dlx->mems++
#define oo dlx->mems += 2
#define ooo dlx->mems += 3

#define O "%"
#define mod %

#define len itm
#define aux spare

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

void panic(dlxState_t *dlx, int p, char *m, lua_State *L)
{
    fprintf(dlx->stream_err, "" O "s!\n" O "d: " O ".99s\n", m, p, dlx->buf);
    luaL_error(L, "" O "s!\n" O "d: " O "s", m, p, dlx->buf);
}

void print_option(dlxState_t *dlx, int p, FILE *stream)
{
    int k, q;
    if (p < dlx->last_itm || p >= dlx->last_node || dlx->nd[p].itm <= 0)
    {
        fprintf(dlx->stream_err, "Illegal option " O "d!\n", p);
        return;
    }
    for (q = p;;)
    {
        fprintf(stream, " " O ".8s", dlx->cl[dlx->nd[q].itm].name);
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
}

void prow(dlxState_t *dlx, int p)
{
    print_option(dlx, p, dlx->stream_err);
}

void print_itm(dlxState_t *dlx, int c)
{
    int p;
    if (c < root || c >= dlx->last_itm)
    {
        fprintf(dlx->stream_err, "Illegal item " O "d!\n", c);
        return;
    }
    if (c < dlx->second)
        fprintf(dlx->stream_err, "Item " O ".8s, length " O "d, neighbors " O ".8s and " O ".8s:\n",
                dlx->cl[c].name, dlx->nd[c].len, dlx->cl[dlx->cl[c].prev].name, dlx->cl[dlx->cl[c].next].name);
    else
        fprintf(dlx->stream_err, "Item " O ".8s, length " O "d:\n", dlx->cl[c].name, dlx->nd[c].len);
    for (p = dlx->nd[c].down; p >= dlx->last_itm; p = dlx->nd[p].down)
        prow(dlx, p);
}

void sanity(dlxState_t *dlx)
{
    int k, p, q, pp, qq;
    for (q = root, p = dlx->cl[q].next;; q = p, p = dlx->cl[p].next)
    {
        if (dlx->cl[p].prev != q)
            fprintf(dlx->stream_err, "Bad prev field at itm " O ".8s!\n",
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
            fprintf(dlx->stream_err, "Bad len field in item " O ".8s!\n",
                    dlx->cl[p].name);
    }
}

void cover(dlxState_t *dlx, int c)
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

void uncover(dlxState_t *dlx, int c)
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

void print_state(dlxState_t *dlx)
{
    int l;
    fprintf(dlx->stream_err, "Current state (level " O "d):\n", dlx->level);
    for (l = 0; l < dlx->level; l++)
    {
        print_option(dlx, dlx->choice[l], dlx->stream_err);
        if (l >= dlx->show_levels_max)
        {
            fprintf(dlx->stream_err, " ...\n");
            break;
        }
    }
    fprintf(dlx->stream_err, " " O "lld solutions, " O "lld mems, and max level " O "d so far.\n",
            dlx->count, dlx->mems, dlx->maxl);
}

void print_progress(dlxState_t *dlx)
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

void dlx1_do(lua_State *L, dlxState_t *dlx, int argc, char **argv)
{
    int cc, i, j, k, p, pp, q, r, t, cur_node, best_itm;

    p = best_itm = -1;

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

    for (j = argc - 1, k = 0; j; j--)
        switch (argv[j][0])
        {
        case 'v':
            k |= (sscanf(argv[j] + 1, "" O "d", &(dlx->vbose)) - 1);
            break;
        case 'm':
            k |= (sscanf(argv[j] + 1, "" O "d", &(dlx->spacing)) - 1);
            break;
        case 's':
            k |= (sscanf(argv[j] + 1, "" O "d", &(dlx->random_seed)) - 1), dlx->randomizing = 1;
            break;
        case 'd':
            k |= (sscanf(argv[j] + 1, "" O "lld", &(dlx->delta)) - 1), dlx->thresh = dlx->delta;
            break;
        case 'c':
            k |= (sscanf(argv[j] + 1, "" O "d", &(dlx->show_choices_max)) - 1);
            break;
        case 'C':
            k |= (sscanf(argv[j] + 1, "" O "d", &(dlx->show_levels_max)) - 1);
            break;
        case 'l':
            k |= (sscanf(argv[j] + 1, "" O "d", &(dlx->show_choices_gap)) - 1);
            break;
        case 't':
            k |= (sscanf(argv[j] + 1, "" O "lld", &(dlx->maxcount)) - 1);
            break;
        case 'T':
            k |= (sscanf(argv[j] + 1, "" O "lld", &(dlx->timeout)) - 1);
            break;
        case 'S':
            dlx->shape_name = argv[j] + 1, dlx->shape_file = fopen(dlx->shape_name, "w");
            if (!dlx->shape_file)
                fprintf(dlx->stream_err, "Sorry, I can't open file `" O "s' for writing!\n",
                        dlx->shape_name);
            break;
        default:
            k = 1;
        }
    if (k)
    {
        fprintf(dlx->stream_err, "Usage: " O "s [v<n>] [m<n>] [s<n>] [d<n>]"
                                 " [c<n>] [C<n>] [l<n>] [t<n>] [T<n>] [S<bar>] < foo.dlx\n",
                argv[0]);

        luaL_error(L, "Usage: " O "s [v<n>] [m<n>] [s<n>] [d<n>]"
                      " [c<n>] [C<n>] [l<n>] [t<n>] [T<n>] [S<bar>] < foo.dlx",
                   argv[0]);
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
        if (o, dlx->buf[p = strlen(dlx->buf) - 1] != '\n')
            panic(dlx, p, "Input line way too long", L);
        for (p = 0; o, isspace(dlx->buf[p]); p++)
            ;
        if (dlx->buf[p] == '|' || !dlx->buf[p])
            continue;
        dlx->last_itm = 1;
        break;
    }
    if (!dlx->last_itm)
        panic(dlx, p, "No items", L);
    for (; o, dlx->buf[p];)
    {
        for (j = 0; j < max_name_length && (o, !isspace(dlx->buf[p + j])); j++)
        {
            if (dlx->buf[p + j] == ':' || dlx->buf[p + j] == '|')
                panic(dlx, p, "Illegal character in item name", L);
            o, dlx->cl[dlx->last_itm].name[j] = dlx->buf[p + j];
        }
        if (j == max_name_length && !isspace(dlx->buf[p + j]))
            panic(dlx, p, "Item name too long", L);

        for (k = 1; o, strncmp(dlx->cl[k].name, dlx->cl[dlx->last_itm].name, max_name_length); k++)
            ;
        if (k < dlx->last_itm)
            panic(dlx, p, "Duplicate item name", L);

        if (dlx->last_itm > max_cols)
            panic(dlx, p, "Too many items", L);
        oo, dlx->cl[dlx->last_itm - 1].next = dlx->last_itm, dlx->cl[dlx->last_itm].prev = dlx->last_itm - 1;

        o, dlx->nd[dlx->last_itm].up = dlx->nd[dlx->last_itm].down = dlx->last_itm;
        dlx->last_itm++;

        for (p += j + 1; o, isspace(dlx->buf[p]); p++)
            ;
        if (dlx->buf[p] == '|')
        {
            if (dlx->second != max_cols)
                panic(dlx, p, "Item name line contains | twice", L);
            dlx->second = dlx->last_itm;
            for (p++; o, isspace(dlx->buf[p]); p++)
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
        if (o, dlx->buf[p = strlen(dlx->buf) - 1] != '\n')
            panic(dlx, p, "Option line too long", L);
        for (p = 0; o, isspace(dlx->buf[p]); p++)
            ;
        if (dlx->buf[p] == '|' || !dlx->buf[p])
            continue;
        i = dlx->last_node;
        for (pp = 0; dlx->buf[p];)
        {
            for (j = 0; j < max_name_length && (o, !isspace(dlx->buf[p + j])); j++)
                o, dlx->cl[dlx->last_itm].name[j] = dlx->buf[p + j];
            if (j == max_name_length && !isspace(dlx->buf[p + j]))
                panic(dlx, p, "Item name too long", L);
            if (j < max_name_length)
                o, dlx->cl[dlx->last_itm].name[j] = '\0';

            for (k = 0; o, strncmp(dlx->cl[k].name, dlx->cl[dlx->last_itm].name, max_name_length); k++)
                ;
            if (k == dlx->last_itm)
                panic(dlx, p, "Unknown item name", L);
            if (o, dlx->nd[k].aux >= i)
                panic(dlx, p, "Duplicate item name in this option", L);
            dlx->last_node++;
            if (dlx->last_node == max_nodes)
                panic(dlx, p, "Too many nodes", L);
            o, dlx->nd[dlx->last_node].itm = k;
            if (k < dlx->second)
                pp = 1;
            o, t = dlx->nd[k].len + 1;

            o, dlx->nd[k].len = t;
            dlx->nd[k].aux = dlx->last_node;
            if (!dlx->randomizing)
            {
                o, r = dlx->nd[k].up;
                ooo, dlx->nd[r].down = dlx->nd[k].up = dlx->last_node, dlx->nd[dlx->last_node].up = r, dlx->nd[dlx->last_node].down = k;
            }
            else
            {
                dlx->mems += 4, t = gb_unif_rand(t);
                for (o, r = k; t; o, r = dlx->nd[r].down, t--)
                    ;
                ooo, q = dlx->nd[r].up, dlx->nd[q].down = dlx->nd[r].up = dlx->last_node;
                o, dlx->nd[dlx->last_node].up = q, dlx->nd[dlx->last_node].down = r;
            }

            for (p += j + 1; o, isspace(dlx->buf[p]); p++)
                ;
        }
        if (!pp)
        {
            if (dlx->vbose & show_warnings)
                fprintf(dlx->stream_err, "Option ignored (no primary items): " O "s", dlx->buf);
            while (dlx->last_node > i)
            {

                o, k = dlx->nd[dlx->last_node].itm;
                oo, dlx->nd[k].len--, dlx->nd[k].aux = i - 1;
                o, q = dlx->nd[dlx->last_node].up, r = dlx->nd[dlx->last_node].down;
                oo, dlx->nd[q].down = r, dlx->nd[r].up = q;

                dlx->last_node--;
            }
        }
        else
        {
            o, dlx->nd[i].down = dlx->last_node;
            dlx->last_node++;
            if (dlx->last_node == max_nodes)
                panic(dlx, p, "Too many nodes", L);
            dlx->options++;
            o, dlx->nd[dlx->last_node].up = i + 1;
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
        for (k = 1; k < dlx->last_itm; k++)
        {
            if (k == dlx->second)
                fprintf(dlx->stream_err, " |");
            fprintf(dlx->stream_err, " " O "d", dlx->nd[k].len);
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
            print_state(dlx);
        else
            print_progress(dlx);
    }
    if (dlx->mems >= dlx->timeout)
    {
        fprintf(dlx->stream_err, "TIMEOUT!\n");
        goto done;
    }

    dlx->tmems = dlx->mems, t = max_nodes;
    if ((dlx->vbose & show_details) &&
        dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
        fprintf(dlx->stream_err, "level " O "d:", dlx->level);
    for (o, k = dlx->cl[root].next; t && k != root; o, k = dlx->cl[k].next)
    {
        if ((dlx->vbose & show_details) &&
            dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
            fprintf(dlx->stream_err, " " O ".8s(" O "d)", dlx->cl[k].name, dlx->nd[k].len);
        if (o, dlx->nd[k].len <= t)
        {
            if (dlx->nd[k].len < t)
                best_itm = k, t = dlx->nd[k].len, p = 1;
            else
            {
                p++;
                if (dlx->randomizing && (dlx->mems += 4, !gb_unif_rand(p)))
                    best_itm = k;
            }
        }
    }
    if ((dlx->vbose & show_details) &&
        dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
        fprintf(dlx->stream_err, " branching on " O ".8s(" O "d)\n", dlx->cl[best_itm].name, t);
    if (t > dlx->maxdeg)
        dlx->maxdeg = t;
    if (dlx->shape_file)
    {
        fprintf(dlx->shape_file, "" O "d " O ".8s\n", t, dlx->cl[best_itm].name);
        fflush(dlx->shape_file);
    }
    dlx->cmems += dlx->mems - dlx->tmems;

    cover(dlx, best_itm);
    oo, cur_node = dlx->choice[dlx->level] = dlx->nd[best_itm].down;
advance:
    if (cur_node == best_itm)
        goto backup;
    if ((dlx->vbose & show_choices) && dlx->level < dlx->show_choices_max)
    {
        fprintf(dlx->stream_err, "L" O "d:", dlx->level);
        print_option(dlx, cur_node, dlx->stream_err);
    }

    for (pp = cur_node + 1; pp != cur_node;)
    {
        o, cc = dlx->nd[pp].itm;
        if (cc <= 0)
            o, pp = dlx->nd[pp].up;
        else
            cover(dlx, cc), pp++;
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
                printf("" O "lld:\n", dlx->count);
                for (k = 0; k <= dlx->level; k++)
                    print_option(dlx, dlx->choice[k], dlx->stream_out);
                fflush(dlx->stream_out);
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
    uncover(dlx, best_itm);
    if (dlx->level == 0)
        goto done;
    dlx->level--;
    oo, cur_node = dlx->choice[dlx->level], best_itm = dlx->nd[cur_node].itm;
recover:

    for (pp = cur_node - 1; pp != cur_node;)
    {
        o, cc = dlx->nd[pp].itm;
        if (cc <= 0)
            o, pp = dlx->nd[pp].down;
        else
            uncover(dlx, cc), pp--;
    }

    oo, cur_node = dlx->choice[dlx->level] = dlx->nd[cur_node].down;
    goto advance;

done:
    if (dlx->vbose & show_tots)
    {
        fprintf(dlx->stream_err, "Item totals:");
        for (k = 1; k < dlx->last_itm; k++)
        {
            if (k == dlx->second)
                fprintf(dlx->stream_err, " |");
            fprintf(dlx->stream_err, " " O "d", dlx->nd[k].len);
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
        dlx->bytes = dlx->last_itm * sizeof(dlxItem_t) + dlx->last_node * sizeof(dlxNode_t) + dlx->maxl * sizeof(int);
        fprintf(dlx->stream_err, " " O "llu updates, " O "llu bytes, " O "llu nodes,",
                dlx->updates, dlx->bytes, dlx->nodes);
        fprintf(dlx->stream_err, " ccost " O "lld%%.\n",
                (200 * dlx->cmems + dlx->mems) / (2 * dlx->mems));
    }

    if (dlx->shape_file)
        fclose(dlx->shape_file);
}

int coroutine(lua_State *L)
{
    dlxState_t *dlx = lua_touserdata(L, lua_upvalueindex(1));
    lua_Integer argc = lua_tointeger(L, lua_upvalueindex(2));
    char **argv = lua_touserdata(L, lua_upvalueindex(3));
    lua_Integer close_flags = lua_tointeger(L, lua_upvalueindex(4));

    dlx1_do(L, dlx, argc, argv);

    if (close_flags & 1)
    {
        fclose(dlx->stream_in);
    }

    if (close_flags & 2)
    {
        fclose(dlx->stream_out);
    }

    if (close_flags & 4)
    {
        fclose(dlx->stream_err);
    }

    free(dlx);
    free(argv);

    return 0;
}

/*
    This function consumes a table of strings that denotes the arguments to the solver.
*/
int l_create(lua_State *L)
{
    lua_Integer close_flags = 0;
    int type;

    lua_Integer argc = lua_tointeger(L, 1);

    char **argv = (char **)malloc(sizeof(char *) * argc);

    for (int i = 1; i <= argc; i++)
    {
        type = lua_rawgeti(L, 2, i);
        assert(type == LUA_TSTRING);

        argv[i - 1] = (char *)lua_tostring(L, -1);
        lua_pop(L, 1);
    }

    dlxState_t *dlx = (dlxState_t *)malloc(sizeof(dlxState_t));

    // stdin
    if (lua_isnoneornil(L, 3))
    {
        dlx->stream_in = stdin;
    }
    else
    {
        dlx->stream_in = fopen(lua_tostring(L, 3), "r");
        close_flags |= 1;
    }

    // stdout
    if (lua_isnoneornil(L, 4))
    {
        dlx->stream_out = stdout;
    }
    else
    {
        dlx->stream_out = fopen(lua_tostring(L, 4), "r");
        close_flags |= 2;
    }

    // stderr
    if (lua_isnoneornil(L, 5))
    {
        dlx->stream_err = stderr;
    }
    else
    {
        dlx->stream_err = fopen(lua_tostring(L, 5), "r");
        close_flags |= 4;
    }

    dlx->sanity_checking = lua_toboolean(L, 6);

    lua_State *S = lua_newthread(L);

    lua_pushlightuserdata(S, dlx);
    lua_pushinteger(S, argc);
    lua_pushlightuserdata(S, argv);
    lua_pushinteger(S, close_flags);
    lua_pushcclosure(S, &coroutine, 4);

    return 1;
}

const struct luaL_Reg libdlx1[] = {
    {"create", l_create},
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