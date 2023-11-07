
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
#include <ctype.h>
#include <gb_flip.h>

#include "dlx1.h"

void default_panic(dlxState_t *dlx, int p, char *m, void *ud)
{
    fprintf(stderr, "" O "s!\n" O "d: " O ".99s\n", m, p, dlx->buf);
    exit(-1);
}

void print_option(dlxState_t *dlx, int p, FILE *stream)
{
    int k, q;
    if (p < dlx->last_itm || p >= dlx->last_node || dlx->nd[p].itm <= 0)
    {
        fprintf(stderr, "Illegal option " O "d!\n", p);
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
    print_option(dlx, p, stderr);
}

void print_itm(dlxState_t *dlx, int c)
{
    int p;
    if (c < root || c >= dlx->last_itm)
    {
        fprintf(stderr, "Illegal item " O "d!\n", c);
        return;
    }
    if (c < dlx->second)
        fprintf(stderr, "Item " O ".8s, length " O "d, neighbors " O ".8s and " O ".8s:\n",
                dlx->cl[c].name, dlx->nd[c].len, dlx->cl[dlx->cl[c].prev].name, dlx->cl[dlx->cl[c].next].name);
    else
        fprintf(stderr, "Item " O ".8s, length " O "d:\n", dlx->cl[c].name, dlx->nd[c].len);
    for (p = dlx->nd[c].down; p >= dlx->last_itm; p = dlx->nd[p].down)
        prow(dlx, p);
}

void sanity(dlxState_t *dlx)
{
    int k, p, q, pp, qq;
    for (q = root, p = dlx->cl[q].next;; q = p, p = dlx->cl[p].next)
    {
        if (dlx->cl[p].prev != q)
            fprintf(stderr, "Bad prev field at itm " O ".8s!\n",
                    dlx->cl[p].name);
        if (p == root)
            break;

        for (qq = p, pp = dlx->nd[qq].down, k = 0;; qq = pp, pp = dlx->nd[pp].down, k++)
        {
            if (dlx->nd[pp].up != qq)
                fprintf(stderr, "Bad up field at node " O "d!\n", pp);
            if (pp == p)
                break;
            if (dlx->nd[pp].itm != p)
                fprintf(stderr, "Bad itm field at node " O "d!\n", pp);
        }
        if (dlx->nd[p].len != k)
            fprintf(stderr, "Bad len field in item " O ".8s!\n",
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
    fprintf(stderr, "Current state (level " O "d):\n", dlx->level);
    for (l = 0; l < dlx->level; l++)
    {
        print_option(dlx, dlx->choice[l], stderr);
        if (l >= dlx->show_levels_max)
        {
            fprintf(stderr, " ...\n");
            break;
        }
    }
    fprintf(stderr, " " O "lld solutions, " O "lld mems, and max level " O "d so far.\n",
            dlx->count, dlx->mems, dlx->maxl);
}

void print_progress(dlxState_t *dlx)
{
    int l, k, d, c, p;
    double f, fd;
    fprintf(stderr, " after " O "lld mems: " O "lld sols,", dlx->mems, dlx->count);
    for (f = 0.0, fd = 1.0, l = 0; l < dlx->level; l++)
    {
        c = dlx->nd[dlx->choice[l]].itm, d = dlx->nd[c].len;
        for (k = 1, p = dlx->nd[c].down; p != dlx->choice[l]; k++, p = dlx->nd[p].down)
            ;
        fd *= d, f += (k - 1) / fd;
        fprintf(stderr, " " O "c" O "c",
                k < 10 ? '0' + k : k < 36 ? 'a' + k - 10
                               : k < 62   ? 'A' + k - 36
                                          : '*',
                d < 10 ? '0' + d : d < 36 ? 'a' + d - 10
                               : d < 62   ? 'A' + d - 36
                                          : '*');
        if (l >= dlx->show_levels_max)
        {
            fprintf(stderr, "...");
            break;
        }
    }
    fprintf(stderr, " " O ".5f\n", f + 0.5 / fd);
}

void dlx1_do(dlxInput_t *input, dlxState_t *dlx)
{
    panic_t panic = input->panic == NULL ? &default_panic : input->panic;

    int cc, i, j, k, p, pp, q, r, t, cur_node, best_itm;

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

    for (j = input->argc - 1, k = 0; j; j--)
        switch (input->argv[j][0])
        {
        case 'v':
            k |= (sscanf(input->argv[j] + 1, "" O "d", &(dlx->vbose)) - 1);
            break;
        case 'm':
            k |= (sscanf(input->argv[j] + 1, "" O "d", &(dlx->spacing)) - 1);
            break;
        case 's':
            k |= (sscanf(input->argv[j] + 1, "" O "d", &(dlx->random_seed)) - 1), dlx->randomizing = 1;
            break;
        case 'd':
            k |= (sscanf(input->argv[j] + 1, "" O "lld", &(dlx->delta)) - 1), dlx->thresh = dlx->delta;
            break;
        case 'c':
            k |= (sscanf(input->argv[j] + 1, "" O "d", &(dlx->show_choices_max)) - 1);
            break;
        case 'C':
            k |= (sscanf(input->argv[j] + 1, "" O "d", &(dlx->show_levels_max)) - 1);
            break;
        case 'l':
            k |= (sscanf(input->argv[j] + 1, "" O "d", &(dlx->show_choices_gap)) - 1);
            break;
        case 't':
            k |= (sscanf(input->argv[j] + 1, "" O "lld", &(dlx->maxcount)) - 1);
            break;
        case 'T':
            k |= (sscanf(input->argv[j] + 1, "" O "lld", &(dlx->timeout)) - 1);
            break;
        case 'S':
            dlx->shape_name = input->argv[j] + 1, dlx->shape_file = fopen(dlx->shape_name, "w");
            if (!dlx->shape_file)
                fprintf(stderr, "Sorry, I can't open file `" O "s' for writing!\n",
                        dlx->shape_name);
            break;
        default:
            k = 1;
        }
    if (k)
    {
        fprintf(stderr, "Usage: " O "s [v<n>] [m<n>] [s<n>] [d<n>]"
                        " [c<n>] [C<n>] [l<n>] [t<n>] [T<n>] [S<bar>] < foo.dlx\n",
                input->argv[0]);
        exit(-1);
    }
    if (dlx->randomizing)
        gb_init_rand(dlx->random_seed);

    if (max_nodes <= 2 * max_cols)
    {
        fprintf(stderr, "Recompile me: max_nodes must exceed twice max_cols!\n");
        exit(-999);
    }
    while (1)
    {
        if (!fgets(dlx->buf, bufsize, input->device_in))
            break;
        if (o, dlx->buf[p = strlen(dlx->buf) - 1] != '\n')
            panic(dlx, p, "Input line way too long", input->panic_ud);
        for (p = 0; o, isspace(dlx->buf[p]); p++)
            ;
        if (dlx->buf[p] == '|' || !dlx->buf[p])
            continue;
        dlx->last_itm = 1;
        break;
    }
    if (!dlx->last_itm)
        panic(dlx, p, "No items", input->panic_ud);
    for (; o, dlx->buf[p];)
    {
        for (j = 0; j < max_name_length && (o, !isspace(dlx->buf[p + j])); j++)
        {
            if (dlx->buf[p + j] == ':' || dlx->buf[p + j] == '|')
                panic(dlx, p, "Illegal character in item name", input->panic_ud);
            o, dlx->cl[dlx->last_itm].name[j] = dlx->buf[p + j];
        }
        if (j == max_name_length && !isspace(dlx->buf[p + j]))
            panic(dlx, p, "Item name too long", input->panic_ud);

        for (k = 1; o, strncmp(dlx->cl[k].name, dlx->cl[dlx->last_itm].name, max_name_length); k++)
            ;
        if (k < dlx->last_itm)
            panic(dlx, p, "Duplicate item name", input->panic_ud);

        if (dlx->last_itm > max_cols)
            panic(dlx, p, "Too many items", input->panic_ud);
        oo, dlx->cl[dlx->last_itm - 1].next = dlx->last_itm, dlx->cl[dlx->last_itm].prev = dlx->last_itm - 1;

        o, dlx->nd[dlx->last_itm].up = dlx->nd[dlx->last_itm].down = dlx->last_itm;
        dlx->last_itm++;

        for (p += j + 1; o, isspace(dlx->buf[p]); p++)
            ;
        if (dlx->buf[p] == '|')
        {
            if (dlx->second != max_cols)
                panic(dlx, p, "Item name line contains | twice", input->panic_ud);
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
        if (!fgets(dlx->buf, bufsize, input->device_in))
            break;
        if (o, dlx->buf[p = strlen(dlx->buf) - 1] != '\n')
            panic(dlx, p, "Option line too long", input->panic_ud);
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
                panic(dlx, p, "Item name too long", input->panic_ud);
            if (j < max_name_length)
                o, dlx->cl[dlx->last_itm].name[j] = '\0';

            for (k = 0; o, strncmp(dlx->cl[k].name, dlx->cl[dlx->last_itm].name, max_name_length); k++)
                ;
            if (k == dlx->last_itm)
                panic(dlx, p, "Unknown item name", input->panic_ud);
            if (o, dlx->nd[k].aux >= i)
                panic(dlx, p, "Duplicate item name in this option", input->panic_ud);
            dlx->last_node++;
            if (dlx->last_node == max_nodes)
                panic(dlx, p, "Too many nodes", input->panic_ud);
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
                fprintf(stderr, "Option ignored (no primary items): " O "s", dlx->buf);
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
                panic(dlx, p, "Too many nodes", input->panic_ud);
            dlx->options++;
            o, dlx->nd[dlx->last_node].up = i + 1;
            o, dlx->nd[dlx->last_node].itm = -dlx->options;
        }
    }

    if (dlx->vbose & show_basics)
        fprintf(stderr,
                "(" O "lld options, " O "d+" O "d items, " O "d entries successfully read)\n",
                dlx->options, dlx->second - 1, dlx->last_itm - dlx->second, dlx->last_node - dlx->last_itm);

    if (dlx->vbose & show_tots)
    {
        fprintf(stderr, "Item totals:");
        for (k = 1; k < dlx->last_itm; k++)
        {
            if (k == dlx->second)
                fprintf(stderr, " |");
            fprintf(stderr, " " O "d", dlx->nd[k].len);
        }
        fprintf(stderr, "\n");
    }

    dlx->imems = dlx->mems, dlx->mems = 0;

    dlx->level = 0;
forward:
    dlx->nodes++;
    if (dlx->vbose & show_profile)
        dlx->profile[dlx->level]++;
    if (sanity_checking)
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
        fprintf(stderr, "TIMEOUT!\n");
        goto done;
    }

    dlx->tmems = dlx->mems, t = max_nodes;
    if ((dlx->vbose & show_details) &&
        dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
        fprintf(stderr, "dlx->level " O "d:", dlx->level);
    for (o, k = dlx->cl[root].next; t && k != root; o, k = dlx->cl[k].next)
    {
        if ((dlx->vbose & show_details) &&
            dlx->level < dlx->show_choices_max && dlx->level >= dlx->maxl - dlx->show_choices_gap)
            fprintf(stderr, " " O ".8s(" O "d)", dlx->cl[k].name, dlx->nd[k].len);
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
        fprintf(stderr, " branching on " O ".8s(" O "d)\n", dlx->cl[best_itm].name, t);
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
        fprintf(stderr, "L" O "d:", dlx->level);
        print_option(dlx, cur_node, stderr);
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
                fprintf(stderr, "Too many levels!\n");
                exit(-5);
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
                    print_option(dlx, dlx->choice[k], stdout);
                fflush(stdout);
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
            fprintf(stderr, "Too many levels!\n");
            exit(-4);
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
        fprintf(stderr, "Item totals:");
        for (k = 1; k < dlx->last_itm; k++)
        {
            if (k == dlx->second)
                fprintf(stderr, " |");
            fprintf(stderr, " " O "d", dlx->nd[k].len);
        }
        fprintf(stderr, "\n");
    }

    if (dlx->vbose & show_profile)
    {
        fprintf(stderr, "Profile:\n");
        for (dlx->level = 0; dlx->level <= dlx->maxl; dlx->level++)
            fprintf(stderr, "" O "3d: " O "lld\n",
                    dlx->level, dlx->profile[dlx->level]);
    }

    if (dlx->vbose & show_max_deg)
        fprintf(stderr, "The maximum branching degree was " O "d.\n", dlx->maxdeg);
    if (dlx->vbose & show_basics)
    {
        fprintf(stderr, "Altogether " O "llu solution" O "s, " O "llu+" O "llu mems,",
                dlx->count, dlx->count == 1 ? "" : "s", dlx->imems, dlx->mems);
        dlx->bytes = dlx->last_itm * sizeof(dlxItem_t) + dlx->last_node * sizeof(dlxNode_t) + dlx->maxl * sizeof(int);
        fprintf(stderr, " " O "llu updates, " O "llu bytes, " O "llu nodes,",
                dlx->updates, dlx->bytes, dlx->nodes);
        fprintf(stderr, " ccost " O "lld%%.\n",
                (200 * dlx->cmems + dlx->mems) / (2 * dlx->mems));
    }

    if (dlx->shape_file)
        fclose(dlx->shape_file);
}

void dlx1(dlxInput_t *input)
{
    dlxState_t *dlx = (dlxState_t *)malloc(sizeof(dlxState_t));

    dlx1_do(input, dlx);

    free(dlx);
}