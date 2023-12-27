

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <gb_flip.h>

#include <lua.h>
#include <lauxlib.h>

char encode(int x)
{
    if (x < 10)
        return '0' + x;
    else if (x < 36)
        return 'a' + x - 10;
    else
        return 'A' + x - 36;
}

int l_queens(lua_State *L)
{
    int pn = lua_tointeger(L, 1);

    register int j, k, n, nn, t;

    n = pn, nn = n + n - 2;
    if (nn > 62)
    {
        luaL_error(L, "Sorry, I can't currently handle n>32!");
    }

    luaL_Buffer b;
    luaL_buffinit(L, &b);

    luaL_addstring(&b, "| This data produced by Lua \n");

    for (j = 0; j < n; j++)
    {
        t = (j & 1 ? n - 1 - j : n + j) >> 1;
        luaL_addchar(&b, 'r');
        luaL_addchar(&b, encode(t));
        luaL_addstring(&b, " c");
        luaL_addchar(&b, encode(t));
        luaL_addchar(&b, ' ');
    }
    luaL_addstring(&b, "|");
    for (j = 1; j < nn; j++)
    {
        luaL_addstring(&b, " a");
        luaL_addchar(&b, encode(j));
        luaL_addstring(&b, " b");
        luaL_addchar(&b, encode(j));
    }

    luaL_addchar(&b, '\n');

    for (j = 0; j < n; j++)
        for (k = 0; k < n; k++)
        {
            luaL_addchar(&b, 'r');
            luaL_addchar(&b, encode(j));
            luaL_addstring(&b, " c");
            luaL_addchar(&b, encode(k));

            t = j + k;
            if (t && (t < nn))
            {
                luaL_addstring(&b, " a");
                luaL_addchar(&b, encode(t));
            }

            t = n - 1 - j + k;
            if (t && (t < nn))
            {
                luaL_addstring(&b, " b");
                luaL_addchar(&b, encode(t));
            }

            luaL_addchar(&b, '\n');
        }

    luaL_pushresult(&b);

    return 1;
}

const struct luaL_Reg libdlxgenerator[] = {
    {"queens", l_queens},
    {NULL, NULL} /* sentinel */
};

int luaopen_libdlxgenerator(lua_State *L) // the initialization function of the module.
{
    luaL_newlib(L, libdlxgenerator);

    return 1;
}