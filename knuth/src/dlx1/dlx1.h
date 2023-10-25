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

typedef void (*panic_t)(dlxState_t, char *, int);

void default_panic(dlxState_t, char *, int);

void dlx1(int argc, char *argv[], panic_t panic);