
local dlx = require 'dlx'.dlx1
local op = require 'operator'

local co = dlx.create {
    stdout = nil, -- default to actual stdout,
    stderr = nil,   -- default to actual stderr.
    -- either
    stdin = 'queens_8x8.txt',    -- or,
    stdin = {
        literal = [[

        ]], -- or,
        items = {
            primary = {},
            secondary = {},
        },
        options = {
            [{'r4', 'c5', 'a9', 'b8'}] = a_value
        }
    },  --. Finally,
    arguments = {
        S = 'tree.txt',
        m = 1,
        s = 541,
        v = dlx.show.basics | dlx.show.choices | dlx.show.details | dlx.show.profile,
    }
}

for i = 1, 2 do
    local flag, sol = coroutine.resume (co)
    
    op.print_table (sol)
end
-- use coroutine.resume as usual.
-- local yielded = dlx.resume (co, function (sol) end)