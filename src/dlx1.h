

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

#define root 0

#define max_name_length 64

#include <stdio.h>

typedef unsigned int uint;
typedef unsigned long long ullng;

typedef struct
{
    int up, down;
    int itm;
    int spare;
} dlx1Node_t;

typedef struct
{
    char name[max_name_length];
    int prev, next;
} dlx1Item_t;

typedef struct dlx1State_s
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

    dlx1Node_t nd[max_nodes];
    int last_node;
    dlx1Item_t cl[max_cols + 2];
    int second;
    int last_itm;

    int level;
    int choice[max_level];
    ullng profile[max_level];

    FILE *stream_in;
    FILE *stream_out;
    FILE *stream_err;

    int sanity_checking;

} dlx1State_t;
