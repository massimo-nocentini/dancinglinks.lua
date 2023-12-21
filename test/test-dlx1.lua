
local dlx = require 'dlx'.dlx1
local op = require 'operator'
local unittest = require 'unittest'

local tests = {}

function tests.test_queens_literal ()

    local co = dlx.coroutine {
        stdout = 'sols.txt',
        stderr = 'log.txt',
        -- stdin = '../knuth/test/queens/data_8x8.txt',
        stdin = {
            literal = [[

| This data produced by ./queens-dlx 8
r4 c4 r3 c3 r5 c5 r2 c2 r6 c6 r1 c1 r7 c7 r0 c0 | a1 b1 a2 b2 a3 b3 a4 b4 a5 b5 a6 b6 a7 b7 a8 b8 a9 b9 aa ba ab bb ac bc ad bd
r0 c0 b7
r0 c1 a1 b8
r0 c2 a2 b9
r0 c3 a3 ba
r0 c4 a4 bb
r0 c5 a5 bc
r0 c6 a6 bd
r0 c7 a7
r1 c0 a1 b6
r1 c1 a2 b7
r1 c2 a3 b8
r1 c3 a4 b9
r1 c4 a5 ba
r1 c5 a6 bb
r1 c6 a7 bc
r1 c7 a8 bd
r2 c0 a2 b5
r2 c1 a3 b6
r2 c2 a4 b7
r2 c3 a5 b8
r2 c4 a6 b9
r2 c5 a7 ba
r2 c6 a8 bb
r2 c7 a9 bc
r3 c0 a3 b4
r3 c1 a4 b5
r3 c2 a5 b6
r3 c3 a6 b7
r3 c4 a7 b8
r3 c5 a8 b9
r3 c6 a9 ba
r3 c7 aa bb
r4 c0 a4 b3
r4 c1 a5 b4
r4 c2 a6 b5
r4 c3 a7 b6
r4 c4 a8 b7
r4 c5 a9 b8
r4 c6 aa b9
r4 c7 ab ba
r5 c0 a5 b2
r5 c1 a6 b3
r5 c2 a7 b4
r5 c3 a8 b5
r5 c4 a9 b6
r5 c5 aa b7
r5 c6 ab b8
r5 c7 ac b9
r6 c0 a6 b1
r6 c1 a7 b2
r6 c2 a8 b3
r6 c3 a9 b4
r6 c4 aa b5
r6 c5 ab b6
r6 c6 ac b7
r6 c7 ad b8
r7 c0 a7
r7 c1 a8 b1
r7 c2 a9 b2
r7 c3 aa b3
r7 c4 ab b4
r7 c5 ac b5
r7 c6 ad b6
r7 c7 b7

]]
        },
        arguments = {
            --S = 'tree.txt',
            m = 1,
            s = 541,
            v = dlx.show.basics | dlx.show.choices | dlx.show.details | dlx.show.profile,
        },
        sanity_checking = false,
    }
    
    unittest.assert.equals (type (co), 'thread')

    local sols, error_happened = {}, false

    for i = 1, 95 do
        local flag, sol = coroutine.resume (co)
        
        if not flag then error_happened = true; break end -- print an error message.
        
        if not sol then break end   -- all ok, just finished to enumerate the solution space.
        
        -- print('Solution ' .. i .. ':')
        -- op.print_table (sol)

        table.insert (sols, sol)
    end

    unittest.assert.isfalse (error_happened)
    unittest.assert.equals (#sols, 92)

    unittest.assert.equals (sols[1], {
        ' r3 c1 a4 b5',
        ' c0 a1 b6 r1',
        ' c4 a6 b9 r2',
        ' c6 ac b7 r6',
        ' r5 c2 a7 b4',
        ' r7 c3 aa b3',
        ' r0 c5 a5 bc',
        ' r4 c7 ab ba',
    })

end


function tests.test_queens_stdin ()

    local co = dlx.coroutine {
        stdout = 'sols.txt',
        stderr = 'log.txt',
        stdin = '../knuth/test/queens/data_8x8.txt',
        arguments = {
            --S = 'tree.txt',
            m = 1,
            s = 541,
            v = dlx.show.basics | dlx.show.choices | dlx.show.details | dlx.show.profile,
        },
        sanity_checking = false,
    }
    
    unittest.assert.equals (type (co), 'thread')

    local sols, error_happened = {}, false

    for i = 1, 95 do
        local flag, sol = coroutine.resume (co)
        
        if not flag then error_happened = true; break end -- print an error message.
        
        if not sol then break end   -- all ok, just finished to enumerate the solution space.
        
        -- print('Solution ' .. i .. ':')
        -- op.print_table (sol)

        table.insert (sols, sol)
    end

    unittest.assert.isfalse (error_happened)
    unittest.assert.equals (#sols, 92)

    unittest.assert.equals (sols[1], {
        ' r3 c1 a4 b5',
        ' c0 a1 b6 r1',
        ' c4 a6 b9 r2',
        ' c6 ac b7 r6',
        ' r5 c2 a7 b4',
        ' r7 c3 aa b3',
        ' r0 c5 a5 bc',
        ' r4 c7 ab ba',
    })

end

-----------------------------------------------------------------------------------------------

print (unittest.run (tests):summary ())