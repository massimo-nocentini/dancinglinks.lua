

local libdlx1 = require 'libdlx1'

local dlx = {
    dlx1 = {
        show = libdlx1.show
    }
}

local function parse_argv (tbl)
    local argc, argv = 1, {'lua'}

    for k, v in pairs (tbl.arguments) do
        argc = argc + 1
        table.insert(argv, k .. v)
    end

    return argc, argv
end

function dlx.dlx1.coroutine (tbl)

    local argc, argv = parse_argv (tbl)
    local sanity_checking = false

    -- handle here the streams for input, output and error.

    return libdlx1.coroutine (argc, argv, tbl.stdin, tbl.stdout, tbl.stderr, sanity_checking)    -- for now discard the std streams.

end

return dlx