
#define o mems++
#define oo mems += 2
#define ooo mems += 3
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

#define max_name_length 64

typedef unsigned int uint;
typedef unsigned long long ullng;
/*6:*/
#line 318 "dlx1.w"

typedef struct node_struct
{
    int up, down;
    int itm;
    int spare;
} node;

/*:6*/ /*7:*/
#line 338 "dlx1.w"

typedef struct itm_struct
{
    char name[max_name_length];
    int prev, next;
} item;

/*:7*/
#line 116 "dlx1.w"
;
/*3:*/
#line 193 "dlx1.w"

void dlx1(int argc, char *argv[]);