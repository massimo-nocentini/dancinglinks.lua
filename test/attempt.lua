
local dlx = require 'dlx'.dlx1
local op = require 'operator'

local stdin = {
    literal = [[

    ]], -- or,
    items = {
        primary = {},
        secondary = {},
    },
    options = {
        [{'r4', 'c5', 'a9', 'b8'}] = a_value
    }
}  --. Finally,

local co = dlx.create {
    stdout = 'sols.txt', -- default to actual stdout.
    stderr = 'log.txt',   -- default to actual stderr.
    stdin = '../knuth/test/queens/data_8x8.txt',
    arguments = {
        --S = 'tree.txt',
        m = 1,
        s = 541,
        v = dlx.show.basics | dlx.show.choices | dlx.show.details | dlx.show.profile,
    }
}

for i = 1, 95 do
    local flag, sol = coroutine.resume (co)
    if not sol then break end
    print('Solution ' .. i .. ':')
    op.print_table (sol)
end
-- use coroutine.resume as usual.
-- local yielded = dlx.resume (co, function (sol) end)