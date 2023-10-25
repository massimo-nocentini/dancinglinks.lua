#define o DLX.mems++
#define oo DLX.mems += 2
#define ooo DLX.mems += 3
#define O "%"
#define mod %

#define max_level 10000
#define max_cols 100000
#define max_nodes 25000000
#define bufsize (9 * max_cols + 3)

#define show_basics 1
#define show_choices 2
#define show_details 4
#define show_profile 128
#define show_full_state 256
#define show_tots 512
#define show_warnings 1024
#define show_max_deg 2048

#define len itm
#define aux spare

#define root 0

#define sanity_checking 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gb_flip.h"
typedef unsigned int uint;
typedef unsigned long long ullng;

typedef struct node_struct
{
    int up, down;
    int itm;
    int spare;
} node;

typedef struct itm_struct
{
    char name[8];
    int prev, next;
} item;

;

typedef struct dlxState_s
{
    int random_seed;
    int randomizing;
    int vbose;
    int spacing;
    int show_choices_max;
    int show_choices_gap;

    int show_levels_max;
    int maxl;
    char buf[bufsize];
    ullng count;
    ullng options;
    ullng imems, mems, cmems, tmems;
    ullng updates;
    ullng bytes;
    ullng nodes;
    ullng thresh;
    ullng delta;
    ullng maxcount;
    ullng timeout;
    FILE *shape_file;
    char *shape_name;
    int maxdeg;

    node nd[max_nodes];
    int last_node;
    item cl[max_cols + 2];
    int second;
    int last_itm;

    int level;
    int choice[max_level];
    ullng profile[max_level];
} dlxState_t;

void panic(dlxState_t DLX, char *m, int p)
{
    fprintf(stderr, "" O "s!\n" O "d: " O ".99s\n", m, p, DLX.buf);
    exit(-666);
}

;

void print_option(dlxState_t DLX, int p, FILE *stream)
{
    register int k, q;
    if (p < DLX.last_itm || p >= DLX.last_node || DLX.nd[p].itm <= 0)
    {
        fprintf(stderr, "Illegal option " O "d!\n", p);
        return;
    }
    for (q = p;;)
    {
        fprintf(stream, " " O ".8s", DLX.cl[DLX.nd[q].itm].name);
        q++;
        if (DLX.nd[q].itm <= 0)
            q = DLX.nd[q].up;
        if (q == p)
            break;
    }
    for (q = DLX.nd[DLX.nd[p].itm].down, k = 1; q != p; k++)
    {
        if (q == DLX.nd[p].itm)
        {
            fprintf(stream, " (?)\n");
            return;
        }
        else
            q = DLX.nd[q].down;
    }
    fprintf(stream, " (" O "d of " O "d)\n", k, DLX.nd[DLX.nd[p].itm].len);
}

void prow(dlxState_t DLX, int p)
{
    print_option(DLX, p, stderr);
}

void print_itm(dlxState_t DLX, int c)
{
    register int p;
    if (c < root || c >= DLX.last_itm)
    {
        fprintf(stderr, "Illegal item " O "d!\n", c);
        return;
    }
    if (c < DLX.second)
        fprintf(stderr, "Item " O ".8s, length " O "d, neighbors " O ".8s and " O ".8s:\n",
                DLX.cl[c].name, DLX.nd[c].len, DLX.cl[DLX.cl[c].prev].name, DLX.cl[DLX.cl[c].next].name);
    else
        fprintf(stderr, "Item " O ".8s, length " O "d:\n", DLX.cl[c].name, DLX.nd[c].len);
    for (p = DLX.nd[c].down; p >= DLX.last_itm; p = DLX.nd[p].down)
        prow(DLX, p);
}

void sanity(dlxState_t DLX)
{
    register int k, p, q, pp, qq;
    for (q = root, p = DLX.cl[q].next;; q = p, p = DLX.cl[p].next)
    {
        if (DLX.cl[p].prev != q)
            fprintf(stderr, "Bad prev field at itm " O ".8s!\n",
                    DLX.cl[p].name);
        if (p == root)
            break;

        for (qq = p, pp = DLX.nd[qq].down, k = 0;; qq = pp, pp = DLX.nd[pp].down, k++)
        {
            if (DLX.nd[pp].up != qq)
                fprintf(stderr, "Bad up field at node " O "d!\n", pp);
            if (pp == p)
                break;
            if (DLX.nd[pp].itm != p)
                fprintf(stderr, "Bad itm field at node " O "d!\n", pp);
        }
        if (DLX.nd[p].len != k)
            fprintf(stderr, "Bad len field in item " O ".8s!\n",
                    DLX.cl[p].name);

        ;
    }
}

void cover(dlxState_t DLX, int c)
{
    register int cc, l, r, rr, nn, uu, dd, t;
    o, l = DLX.cl[c].prev, r = DLX.cl[c].next;
    oo, DLX.cl[l].next = r, DLX.cl[r].prev = l;
    DLX.updates++;
    for (o, rr = DLX.nd[c].down; rr >= DLX.last_itm; o, rr = DLX.nd[rr].down)
        for (nn = rr + 1; nn != rr;)
        {
            o, uu = DLX.nd[nn].up, dd = DLX.nd[nn].down;
            o, cc = DLX.nd[nn].itm;
            if (cc <= 0)
            {
                nn = uu;
                continue;
            }
            oo, DLX.nd[uu].down = dd, DLX.nd[dd].up = uu;
            DLX.updates++;
            o, t = DLX.nd[cc].len - 1;
            o, DLX.nd[cc].len = t;
            nn++;
        }
}

void uncover(dlxState_t DLX, int c)
{
    register int cc, l, r, rr, nn, uu, dd, t;
    for (o, rr = DLX.nd[c].down; rr >= DLX.last_itm; o, rr = DLX.nd[rr].down)
        for (nn = rr + 1; nn != rr;)
        {
            o, uu = DLX.nd[nn].up, dd = DLX.nd[nn].down;
            o, cc = DLX.nd[nn].itm;
            if (cc <= 0)
            {
                nn = uu;
                continue;
            }
            oo, DLX.nd[uu].down = DLX.nd[dd].up = nn;
            o, t = DLX.nd[cc].len + 1;
            o, DLX.nd[cc].len = t;
            nn++;
        }
    o, l = DLX.cl[c].prev, r = DLX.cl[c].next;
    oo, DLX.cl[l].next = DLX.cl[r].prev = c;
}

void print_state(dlxState_t DLX)
{
    register int l;
    fprintf(stderr, "Current state (level " O "d):\n", DLX.level);
    for (l = 0; l < DLX.level; l++)
    {
        print_option(DLX, DLX.choice[l], stderr);
        if (l >= DLX.show_levels_max)
        {
            fprintf(stderr, " ...\n");
            break;
        }
    }
    fprintf(stderr, " " O "lld solutions, " O "lld mems, and max level " O "d so far.\n",
            DLX.count, DLX.mems, DLX.maxl);
}

void print_progress(dlxState_t DLX)
{
    register int l, k, d, c, p;
    register double f, fd;
    fprintf(stderr, " after " O "lld mems: " O "lld sols,", DLX.mems, DLX.count);
    for (f = 0.0, fd = 1.0, l = 0; l < DLX.level; l++)
    {
        c = DLX.nd[DLX.choice[l]].itm, d = DLX.nd[c].len;
        for (k = 1, p = DLX.nd[c].down; p != DLX.choice[l]; k++, p = DLX.nd[p].down)
            ;
        fd *= d, f += (k - 1) / fd;
        fprintf(stderr, " " O "c" O "c",
                k < 10 ? '0' + k : k < 36 ? 'a' + k - 10
                               : k < 62   ? 'A' + k - 36
                                          : '*',
                d < 10 ? '0' + d : d < 36 ? 'a' + d - 10
                               : d < 62   ? 'A' + d - 36
                                          : '*');
        if (l >= DLX.show_levels_max)
        {
            fprintf(stderr, "...");
            break;
        }
    }
    fprintf(stderr, " " O ".5f\n", f + 0.5 / fd);
}

;
void dlx1(int argc, char *argv[])
{
    register int cc, i, j, k, p, pp, q, r, t, cur_node, best_itm;

    p = best_itm = 0;

    dlxState_t DLX;

    DLX.random_seed = 0;
    DLX.vbose = show_basics + show_warnings;
    DLX.show_choices_max = 1000000;
    DLX.show_choices_gap = 1000000;

    DLX.show_levels_max = 1000000;
    DLX.maxl = 0;
    DLX.thresh = 10000000000;
    DLX.delta = 10000000000;
    DLX.maxcount = 0xffffffffffffffff;
    DLX.timeout = 0x1fffffffffffffff;

    for (j = argc - 1, k = 0; j; j--)
        switch (argv[j][0])
        {
        case 'v':
            k |= (sscanf(argv[j] + 1, "" O "d", &DLX.vbose) - 1);
            break;
        case 'm':
            k |= (sscanf(argv[j] + 1, "" O "d", &DLX.spacing) - 1);
            break;
        case 's':
            k |= (sscanf(argv[j] + 1, "" O "d", &DLX.random_seed) - 1), DLX.randomizing = 1;
            break;
        case 'd':
            k |= (sscanf(argv[j] + 1, "" O "lld", &DLX.delta) - 1), DLX.thresh = DLX.delta;
            break;
        case 'c':
            k |= (sscanf(argv[j] + 1, "" O "d", &DLX.show_choices_max) - 1);
            break;
        case 'C':
            k |= (sscanf(argv[j] + 1, "" O "d", &DLX.show_levels_max) - 1);
            break;
        case 'l':
            k |= (sscanf(argv[j] + 1, "" O "d", &DLX.show_choices_gap) - 1);
            break;
        case 't':
            k |= (sscanf(argv[j] + 1, "" O "lld", &DLX.maxcount) - 1);
            break;
        case 'T':
            k |= (sscanf(argv[j] + 1, "" O "lld", &DLX.timeout) - 1);
            break;
        case 'S':
            DLX.shape_name = argv[j] + 1, DLX.shape_file = fopen(DLX.shape_name, "w");
            if (!DLX.shape_file)
                fprintf(stderr, "Sorry, I can't open file `" O "s' for writing!\n",
                        DLX.shape_name);
            break;
        default:
            k = 1;
        }
    if (k)
    {
        fprintf(stderr, "Usage: " O "s [v<n>] [m<n>] [s<n>] [d<n>]"
                        " [c<n>] [C<n>] [l<n>] [t<n>] [T<n>] [S<bar>] < foo.dlx\n",
                argv[0]);
        exit(-1);
    }
    if (DLX.randomizing)
        gb_init_rand(DLX.random_seed);

    ;

    if (max_nodes <= 2 * max_cols)
    {
        fprintf(stderr, "Recompile me: max_nodes must exceed twice max_cols!\n");
        exit(-999);
    }
    while (1)
    {
        if (!fgets(DLX.buf, bufsize, stdin))
            break;
        if (o, DLX.buf[p = strlen(DLX.buf) - 1] != '\n')
            panic(DLX, "Input line way too long", p);
        for (p = 0; o, isspace(DLX.buf[p]); p++)
            ;
        if (DLX.buf[p] == '|' || !DLX.buf[p])
            continue;
        DLX.last_itm = 1;
        break;
    }
    if (!DLX.last_itm)
        panic(DLX, "No items", p);
    for (; o, DLX.buf[p];)
    {
        for (j = 0; j < 8 && (o, !isspace(DLX.buf[p + j])); j++)
        {
            if (DLX.buf[p + j] == ':' || DLX.buf[p + j] == '|')
                panic(DLX, "Illegal character in item name", p);
            o, DLX.cl[DLX.last_itm].name[j] = DLX.buf[p + j];
        }
        if (j == 8 && !isspace(DLX.buf[p + j]))
            panic(DLX, "Item name too long", p);

        for (k = 1; o, strncmp(DLX.cl[k].name, DLX.cl[DLX.last_itm].name, 8); k++)
            ;
        if (k < DLX.last_itm)
            panic(DLX, "Duplicate item name", p);

        ;

        if (DLX.last_itm > max_cols)
            panic(DLX, "Too many items", p);
        oo, DLX.cl[DLX.last_itm - 1].next = DLX.last_itm, DLX.cl[DLX.last_itm].prev = DLX.last_itm - 1;

        o, DLX.nd[DLX.last_itm].up = DLX.nd[DLX.last_itm].down = DLX.last_itm;
        DLX.last_itm++;

        ;
        for (p += j + 1; o, isspace(DLX.buf[p]); p++)
            ;
        if (DLX.buf[p] == '|')
        {
            if (DLX.second != max_cols)
                panic(DLX, "Item name line contains | twice", p);
            DLX.second = DLX.last_itm;
            for (p++; o, isspace(DLX.buf[p]); p++)
                ;
        }
    }
    if (DLX.second == max_cols)
        DLX.second = DLX.last_itm;
    oo, DLX.cl[DLX.last_itm].prev = DLX.last_itm - 1, DLX.cl[DLX.last_itm - 1].next = DLX.last_itm;
    oo, DLX.cl[DLX.second].prev = DLX.last_itm, DLX.cl[DLX.last_itm].next = DLX.second;

    oo, DLX.cl[root].prev = DLX.second - 1, DLX.cl[DLX.second - 1].next = root;
    DLX.last_node = DLX.last_itm;

    ;

    while (1)
    {
        if (!fgets(DLX.buf, bufsize, stdin))
            break;
        if (o, DLX.buf[p = strlen(DLX.buf) - 1] != '\n')
            panic(DLX, "Option line too long", p);
        for (p = 0; o, isspace(DLX.buf[p]); p++)
            ;
        if (DLX.buf[p] == '|' || !DLX.buf[p])
            continue;
        i = DLX.last_node;
        for (pp = 0; DLX.buf[p];)
        {
            for (j = 0; j < 8 && (o, !isspace(DLX.buf[p + j])); j++)
                o, DLX.cl[DLX.last_itm].name[j] = DLX.buf[p + j];
            if (j == 8 && !isspace(DLX.buf[p + j]))
                panic(DLX, "Item name too long", p);
            if (j < 8)
                o, DLX.cl[DLX.last_itm].name[j] = '\0';

            for (k = 0; o, strncmp(DLX.cl[k].name, DLX.cl[DLX.last_itm].name, 8); k++)
                ;
            if (k == DLX.last_itm)
                panic(DLX, "Unknown item name", p);
            if (o, DLX.nd[k].aux >= i)
                panic(DLX, "Duplicate item name in this option", p);
            DLX.last_node++;
            if (DLX.last_node == max_nodes)
                panic(DLX, "Too many nodes", p);
            o, DLX.nd[DLX.last_node].itm = k;
            if (k < DLX.second)
                pp = 1;
            o, t = DLX.nd[k].len + 1;

            o, DLX.nd[k].len = t;
            DLX.nd[k].aux = DLX.last_node;
            if (!DLX.randomizing)
            {
                o, r = DLX.nd[k].up;
                ooo, DLX.nd[r].down = DLX.nd[k].up = DLX.last_node, DLX.nd[DLX.last_node].up = r, DLX.nd[DLX.last_node].down = k;
            }
            else
            {
                DLX.mems += 4, t = gb_unif_rand(t);
                for (o, r = k; t; o, r = DLX.nd[r].down, t--)
                    ;
                ooo, q = DLX.nd[r].up, DLX.nd[q].down = DLX.nd[r].up = DLX.last_node;
                o, DLX.nd[DLX.last_node].up = q, DLX.nd[DLX.last_node].down = r;
            }

            ;

            ;
            for (p += j + 1; o, isspace(DLX.buf[p]); p++)
                ;
        }
        if (!pp)
        {
            if (DLX.vbose & show_warnings)
                fprintf(stderr, "Option ignored (no primary items): " O "s", DLX.buf);
            while (DLX.last_node > i)
            {

                o, k = DLX.nd[DLX.last_node].itm;
                oo, DLX.nd[k].len--, DLX.nd[k].aux = i - 1;
                o, q = DLX.nd[DLX.last_node].up, r = DLX.nd[DLX.last_node].down;
                oo, DLX.nd[q].down = r, DLX.nd[r].up = q;

                ;
                DLX.last_node--;
            }
        }
        else
        {
            o, DLX.nd[i].down = DLX.last_node;
            DLX.last_node++;
            if (DLX.last_node == max_nodes)
                panic(DLX, "Too many nodes", p);
            DLX.options++;
            o, DLX.nd[DLX.last_node].up = i + 1;
            o, DLX.nd[DLX.last_node].itm = -DLX.options;
        }
    }

    ;
    if (DLX.vbose & show_basics)

        fprintf(stderr,
                "(" O "lld options, " O "d+" O "d items, " O "d entries successfully read)\n",
                DLX.options, DLX.second - 1, DLX.last_itm - DLX.second, DLX.last_node - DLX.last_itm);

    ;
    if (DLX.vbose & show_tots)

    {
        fprintf(stderr, "Item totals:");
        for (k = 1; k < DLX.last_itm; k++)
        {
            if (k == DLX.second)
                fprintf(stderr, " |");
            fprintf(stderr, " " O "d", DLX.nd[k].len);
        }
        fprintf(stderr, "\n");
    }

    ;
    DLX.imems = DLX.mems, DLX.mems = 0;

    DLX.level = 0;
forward:
    DLX.nodes++;
    if (DLX.vbose & show_profile)
        DLX.profile[DLX.level]++;
    if (sanity_checking)
        sanity(DLX);

    if (DLX.delta && (DLX.mems >= DLX.thresh))
    {
        DLX.thresh += DLX.delta;
        if (DLX.vbose & show_full_state)
            print_state(DLX);
        else
            print_progress(DLX);
    }
    if (DLX.mems >= DLX.timeout)
    {
        fprintf(stderr, "TIMEOUT!\n");
        goto done;
    }

    ;

    DLX.tmems = DLX.mems, t = max_nodes;
    if ((DLX.vbose & show_details) &&
        DLX.level < DLX.show_choices_max && DLX.level >= DLX.maxl - DLX.show_choices_gap)
        fprintf(stderr, "Level " O "d:", DLX.level);
    for (o, k = DLX.cl[root].next; t && k != root; o, k = DLX.cl[k].next)
    {
        if ((DLX.vbose & show_details) &&
            DLX.level < DLX.show_choices_max && DLX.level >= DLX.maxl - DLX.show_choices_gap)
            fprintf(stderr, " " O ".8s(" O "d)", DLX.cl[k].name, DLX.nd[k].len);
        if (o, DLX.nd[k].len <= t)
        {
            if (DLX.nd[k].len < t)
                best_itm = k, t = DLX.nd[k].len, p = 1;
            else
            {
                p++;
                if (DLX.randomizing && (DLX.mems += 4, !gb_unif_rand(p)))
                    best_itm = k;
            }
        }
    }
    if ((DLX.vbose & show_details) &&
        DLX.level < DLX.show_choices_max && DLX.level >= DLX.maxl - DLX.show_choices_gap)
        fprintf(stderr, " branching on " O ".8s(" O "d)\n", DLX.cl[best_itm].name, t);
    if (t > DLX.maxdeg)
        DLX.maxdeg = t;
    if (DLX.shape_file)
    {
        fprintf(DLX.shape_file, "" O "d " O ".8s\n", t, DLX.cl[best_itm].name);
        fflush(DLX.shape_file);
    }
    DLX.cmems += DLX.mems - DLX.tmems;

    ;
    cover(DLX, best_itm);
    oo, cur_node = DLX.choice[DLX.level] = DLX.nd[best_itm].down;
advance:
    if (cur_node == best_itm)
        goto backup;
    if ((DLX.vbose & show_choices) && DLX.level < DLX.show_choices_max)
    {
        fprintf(stderr, "L" O "d:", DLX.level);
        print_option(DLX, cur_node, stderr);
    }

    for (pp = cur_node + 1; pp != cur_node;)
    {
        o, cc = DLX.nd[pp].itm;
        if (cc <= 0)
            o, pp = DLX.nd[pp].up;
        else
            cover(DLX, cc), pp++;
    }

    ;

    {
        DLX.nodes++;
        if (DLX.level + 1 > DLX.maxl)
        {
            if (DLX.level + 1 >= max_level)
            {
                fprintf(stderr, "Too many levels!\n");
                exit(-5);
            }
            DLX.maxl = DLX.level + 1;
        }
        if (DLX.vbose & show_profile)
            DLX.profile[DLX.level + 1]++;
        if (DLX.shape_file)
        {
            fprintf(DLX.shape_file, "sol\n");
            fflush(DLX.shape_file);
        }

        {
            DLX.count++;
            if (DLX.spacing && (DLX.count mod DLX.spacing == 0))
            {
                printf("" O "lld:\n", DLX.count);
                for (k = 0; k <= DLX.level; k++)
                    print_option(DLX, DLX.choice[k], stdout);
                fflush(stdout);
            }
            if (DLX.count >= DLX.maxcount)
                goto done;
            goto recover;
        }

        ;
    }

    ;
    if (++DLX.level > DLX.maxl)
    {
        if (DLX.level >= max_level)
        {
            fprintf(stderr, "Too many levels!\n");
            exit(-4);
        }
        DLX.maxl = DLX.level;
    }
    goto forward;
backup:
    uncover(DLX, best_itm);
    if (DLX.level == 0)
        goto done;
    DLX.level--;
    oo, cur_node = DLX.choice[DLX.level], best_itm = DLX.nd[cur_node].itm;
recover:

    for (pp = cur_node - 1; pp != cur_node;)
    {
        o, cc = DLX.nd[pp].itm;
        if (cc <= 0)
            o, pp = DLX.nd[pp].down;
        else
            uncover(DLX, cc), pp--;
    }

    ;
    oo, cur_node = DLX.choice[DLX.level] = DLX.nd[cur_node].down;
    goto advance;

    ;
done:
    if (DLX.vbose & show_tots)

    {
        fprintf(stderr, "Item totals:");
        for (k = 1; k < DLX.last_itm; k++)
        {
            if (k == DLX.second)
                fprintf(stderr, " |");
            fprintf(stderr, " " O "d", DLX.nd[k].len);
        }
        fprintf(stderr, "\n");
    }

    ;

    {
        fprintf(stderr, "Profile:\n");
        for (DLX.level = 0; DLX.level <= DLX.maxl; DLX.level++)
            fprintf(stderr, "" O "3d: " O "lld\n",
                    DLX.level, DLX.profile[DLX.level]);
    }

    ;
    if (DLX.vbose & show_max_deg)
        fprintf(stderr, "The maximum branching degree was " O "d.\n", DLX.maxdeg);
    if (DLX.vbose & show_basics)
    {
        fprintf(stderr, "Altogether " O "llu solution" O "s, " O "llu+" O "llu mems,",
                DLX.count, DLX.count == 1 ? "" : "s", DLX.imems, DLX.mems);
        DLX.bytes = DLX.last_itm * sizeof(item) + DLX.last_node * sizeof(node) + DLX.maxl * sizeof(int);
        fprintf(stderr, " " O "llu updates, " O "llu bytes, " O "llu nodes,",
                DLX.updates, DLX.bytes, DLX.nodes);
        fprintf(stderr, " ccost " O "lld%%.\n",
                (200 * DLX.cmems + DLX.mems) / (2 * DLX.mems));
    }

    if (DLX.shape_file)
        fclose(DLX.shape_file);

    ;
}