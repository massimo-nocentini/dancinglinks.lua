
local lu = require 'luaunit'
local ec = require 'exactcover'

function test_matrix ()

	local v = ec.indexed('v')

	local L = { items = {}, options = {} }

	L.items[ v {1} ] = { isprimary = true }
	L.items[ v {2} ] = { isprimary = true }
	L.items[ v {3} ] = { isprimary = true }
	L.items[ v {4} ] = { isprimary = true }
	L.items[ v {5} ] = { isprimary = true }
	L.items[ v {6} ] = { isprimary = true }
	L.items[ v {7} ] = { isprimary = true }

	L.options = {
		{v {3}, v {5}},
		{v {1}, v {4}, v {7}},
		{v {2}, v {3}, v {6}},
		{v {1}, v {4}, v {6}},
		{v {2}, v {7}},
		{v {4}, v {5}, v {7}},
	}
	
	local solver = ec.solver (L)

	local flag, value = coroutine.resume (solver)

	lu.assertTrue (flag)
	lu.assertItemsEquals (value, {1, 4, 5})

	local flag, value = coroutine.resume (solver)
	lu.assertTrue (flag)
	lu.assertNil (value)	-- just one solution.
end

function test_langfordpairs_3 ()

	local v, s = ec.indexed('v'), ec.indexed('s')

	local L = { items = {}, options = {} }

	L.items[ v {1} ] = { isprimary = true } 
	L.items[ v {2} ] = { isprimary = true } 
	L.items[ v {3} ] = { isprimary = true } 
	L.items[ s {1} ] = { isprimary = true } 
	L.items[ s {2} ] = { isprimary = true } 
	L.items[ s {3} ] = { isprimary = true } 
	L.items[ s {4} ] = { isprimary = true } 
	L.items[ s {5} ] = { isprimary = true } 
	L.items[ s {6} ] = { isprimary = true }

	L.options = {
		{v {1}, s {1}, s {3}},
		{v {1}, s {2}, s {4}},
		{v {1}, s {3}, s {5}},
		{v {1}, s {4}, s {6}},
		{v {2}, s {1}, s {4}},
		{v {2}, s {2}, s {5}},
		{v {2}, s {3}, s {6}},
		{v {3}, s {1}, s {5}},
		{v {3}, s {2}, s {6}},
	}

	local solver = ec.solver (L)

	local sols = {}

	local flag, permutation = coroutine.resume (solver)
	table.insert(sols, permutation)
	
	local flag, permutation = coroutine.resume (solver)
	table.insert(sols, permutation)

	lu.assertItemsEquals (sols, {{2, 7, 8}, {3, 5, 9}})

	local flag, permutation = coroutine.resume (solver)
	lu.assertNil(permutation)
end

function test_langfordpairs_7_count ()

	local n = 7 

	local v, s = ec.indexed('v'), ec.indexed('s')

	local L = { items = {}, options = {} }

	for i = 1, n do for j = 1, 2 * n do

		L.items[ v {i} ] = { isprimary = true }
		L.items[ s {j} ] = { isprimary = true }

		local k = i + j + 1
		if k <= 2 * n then table.insert(L.options, {v {i}, s {j}, s {k}}) end
	end end

	local solver = ec.solver (L)

	local count = -1
	repeat
		local flag, value = coroutine.resume (solver)
		count = count + 1
	until not value

	lu.assertEquals (count, 52)
end

function test_nqueens_slack ()

	local n = 4 
	
	local r, c, a, b = ec.indexed('r'), ec.indexed('c'), ec.indexed('a'), ec.indexed('b')

	local L = { items = {}, options = {} }
	
	for i = 1, n do for j = 1, n do

		local s, d = i + j, i - j

		L.items[ r {i} ] = { isprimary = true }
		L.items[ c {j} ] = { isprimary = true }
		L.items[ a {s} ] = { isprimary = true }
		L.items[ b {d} ] = { isprimary = true }

		table.insert(L.options, { r {i}, c {j}, a {s}, b {d} })
		table.insert(L.options, { a {s} })
		table.insert(L.options, { b {d} })
	end end

	local solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = L.options[iopt]	
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {
		{a {2}},
		{b {0}},
		{r {1}, c {2}, a {3}, b {-1}},
		{a {5}},
		{b {-3}},
		{r {2}, c {4}, a {6}, b {-2}},
		{r {3}, c {1}, a {4}, b {2}},
		{b {3}},
		{r {4}, c {3}, a {7}, b {1}},
		{a {8}}
	})

end

function test_nqueens_secondary ()

	local n = 4 

	local r, c, a, b = ec.indexed('r'), ec.indexed('c'), ec.indexed('a'), ec.indexed('b')

	local L = { items = {}, options = {} }

	for i = 1, n do for j = 1, n do
		local s, d = i + j, i - j

		L.items[ r {i} ] = { isprimary = true }
		L.items[ c {j} ] = { isprimary = true }
		L.items[ a {s} ] = { isprimary = false }
		L.items[ b {d} ] = { isprimary = false }

		table.insert(L.options, { r {i}, c {j}, a {s}, b {d} })
	end end

	local solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = L.options[iopt]	
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {
		{r {1}, c {2}, a {3}, b {-1}},
		{r {2}, c {4}, a {6}, b {-2}},
		{r {3}, c {1}, a {4}, b {2}},
		{r {4}, c {3}, a {7}, b {1}}
	})

end

function test_sudoku ()

	local n = 9
	local nsqrt = math.tointeger (math.sqrt (n))

	local p, r, c, b = ec.indexed('p'), ec.indexed('r'), ec.indexed('c'), ec.indexed('b')

	local L = { items = {}, options = {} }	-- our problem

	for i = 0, n - 1 do for j = 0, n - 1 do for k = 1, n do

		local x = nsqrt * math.floor (i/nsqrt) + math.floor (j/nsqrt)

		L.items[ p {i, j} ] = { isprimary = true }
		L.items[ r {i, k} ] = { isprimary = true }
		L.items[ c {j, k} ] = { isprimary = true }
		L.items[ b {x, k} ] = { isprimary = true }

		table.insert(L.options, { [p {i, j}] = {}, r {i, k}, c {j, k}, b {x, k} }) 

	end end end

	local solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = L.options[iopt]	
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertEquals (L.primarysize, 4*9*9)
	lu.assertEquals (L.secondarysize, 0)
	lu.assertEquals (#L.options, 9*9*9)
	lu.assertEquals (#selection, 9*9)

end

function test_two_crosswords ()

	local l, p = ec.indexed('l'), ec.indexed('p')

	local L = { items = {}, options = {} }	-- our problem

	local name = 'utah'
	L.items[ l {name} ] = { isprimary = true } 
	for j = 1, 5 do
		local option = { l {name} }
		for i, code in ipairs(table.pack(string.byte (name, 1, #name))) do
			option[p {i, j}] = {color = code}
			L.items[ p {i, j} ] = { isprimary = false } 
		end
		table.insert(L.options, option)
	end

	name = 'knuth'
	L.items[ l {name} ] = { isprimary = true } 
	for i = 1, 4 do
		local option = { l {name} }
		for j, code in ipairs(table.pack(string.byte (name, 1, #name))) do
			option[ p {i, j} ] = {color = code}
			L.items[ p {i, j} ] = { isprimary = false } 
		end
		table.insert(L.options, option)
	end

	local solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)	-- skip the first one.
	flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = L.options[iopt]
		--print (table.concat (sol[i], ' '))
	end
	lu.assertTrue (flag)
	lu.assertEquals (sol, {})

end

function test_partridge ()

	local n = 8

	local p, v = ec.indexed('p'), ec.indexed('v')

	local L = { items = {}, options = {} }	-- our problem

	for k = 1, n do for i = 0, n - k do for j = 0, n - k do

		L.items[ v {k} ] = { isprimary = true, atleast = k, atmost = k }

		local option = { v {k} }

		for u = 0, k - 1 do for r = 0, k - 1 do
			L.items[ p {i + u, j + r} ] = { isprimary = true }

			table.insert(option, p {i + u, j + r})
		end end

		table.insert(L.options, option) 

	end end end

	--lu.assertEquals (L.options, {})

	local solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		sol[i] = L.options[iopt]	
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertEquals (sol, {})

end

test_partridge()

os.exit( lu.LuaUnit.run() )
