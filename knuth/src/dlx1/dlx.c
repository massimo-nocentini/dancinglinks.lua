

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dlx1.h>

int main(int argc, char *argv[])
{
    dlxInput_t *input = (dlxInput_t *)malloc(sizeof(dlxInput_t));
    input->argc = argc;
    input->argv = argv;

    input->panic = NULL; // to use the default panic function.
    input->panic_ud = NULL;

    input->device_in = stdin;

    dlx1(input);

    free(input);

    return 0;
}