
local lu = require 'luaunit'
local dl = require 'dl'

function test_matrix ()

	local v = dl.indexed('v')

	local primary = { v[1], v[2], v[3], v[4], v[5], v[6], v[7], }

	local options = {
		{v[3], v[5]},
		{v[1], v[4], v[7]},
		{v[2], v[3], v[6]},
		{v[1], v[4], v[6]},
		{v[2], v[7]},
		{v[4], v[5], v[7]},
	}
	
	local L = {
		primary = primary,
		options = options,
	}

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

	local flag, value = coroutine.resume (solver)

	lu.assertTrue (flag)
	lu.assertItemsEquals (value, {1, 4, 5})

	local flag, value = coroutine.resume (solver)
	lu.assertTrue (flag)
	lu.assertNil (value)	-- just one solution.
end

function test_langfordpairs_3 ()

	local v, s = dl.indexed('v'), dl.indexed('s')

	local primary = { v[1], v[2], v[3], s[1], s[2], s[3], s[4], s[5], s[6], }

	local options = {
		{v[1], s[1], s[3]},
		{v[1], s[2], s[4]},
		{v[1], s[3], s[5]},
		{v[1], s[4], s[6]},
		{v[2], s[1], s[4]},
		{v[2], s[2], s[5]},
		{v[2], s[3], s[6]},
		{v[3], s[1], s[5]},
		{v[3], s[2], s[6]},
	}
	
	local L = {
		primary = primary,
		options = options,
	}

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

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

	local v, s = dl.indexed('v'), dl.indexed('s')

	local primary = {}
	local options = {}

	for i = 1, 2 * n do table.insert(primary, s[i]) end

	for i = 1, n do

		table.insert(primary, v[i])

		for j = 1, 2 * n do
			local k = i + j + 1
			if k <= 2 * n then table.insert(options, {v[i], s[j], s[k]}) end
		end
	end
	
	local L = {
		primary = primary,
		options = options,
	}

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

	local count = -1
	repeat
		local flag, value = coroutine.resume (solver)
		count = count + 1
	until not value

	lu.assertEquals (count, 52)
end

function test_nqueens_slack ()

	local n = 4 
	
	local r, c, a, b = dl.indexed('r'), dl.indexed('c'), dl.indexed('a'), dl.indexed('b')

	local primary = {}	-- items.
	local options = {}

	for i = 1, n do
		table.insert(primary, r[i])
		table.insert(primary, c[i])
	end

	for s = 2, 2 * n do table.insert(primary, a[s]) end

	for d = 1-n, n-1 do table.insert(primary, b[d]) end
	
	for i = 1, n do for j = 1, n do
		local s, d = i + j, i - j
		table.insert(options, { r[i], c[j], a[s], b[d] })
		table.insert(options, { a[s] })
		table.insert(options, { b[d] })
	end end
	
	local L = {
		primary = primary,
		options = options,
	}

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = options[iopt]	
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {
		{a[2]},
		{b[0]},
		{r[1], c[2], a[3], b[-1]},
		{a[5]},
		{b[-3]},
		{r[2], c[4], a[6], b[-2]},
		{r[3], c[1], a[4], b[2]},
		{b[3]},
		{r[4], c[3], a[7], b[1]},
		{a[8]}
	})

end

function test_nqueens_secondary ()

	local n = 4 

	local r, c, a, b = dl.indexed('r'), dl.indexed('c'), dl.indexed('a'), dl.indexed('b')

	local primary = {}
	local secondary = {}
	local options = {}

	for i = 1, n do
		table.insert(primary, r[i])
		table.insert(primary, c[i])
	end

	for s = 2, 2 * n do table.insert(secondary, a[s]) end

	for d = 1-n, n-1 do table.insert(secondary, b[d]) end
	
	for i = 1, n do for j = 1, n do
		local s, d = i + j, i - j
		table.insert(options, { r[i], c[j], a[s], b[d] }) end end
	
	local L = {
		primary = primary,
		secondary = secondary,
		options = options,
	}

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = options[iopt]	
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {
		{r[1], c[2], a[3], b[-1]},
		{r[2], c[4], a[6], b[-2]},
		{r[3], c[1], a[4], b[2]},
		{r[4], c[3], a[7], b[1]}
	})

end

function test_sudoku ()

	local n = 9
	local nsqrt = math.tointeger (math.sqrt (n))

	local p, r, c, b = dl.indexed('p'), dl.indexed('r'), dl.indexed('c'), dl.indexed('b')

	local primary = {}
	local options = {}

	local count = 0
	for i = 0, n - 1 do for j = 0, n - 1 do for k = 1, n do
		local x = nsqrt * math.floor (i/nsqrt) + math.floor (j/nsqrt)

		primary[p[{i, j}]] = true
		primary[r[{i, k}]] = true
		primary[c[{j, k}]] = true
		primary[b[{x, k}]] = true
		
		table.insert(options, { p[{i, j}], r[{i, k}], c[{j, k}], b[{x, k}] }) 
		count = count + 1

	end end end
	
	local L = {
		primary = {},
		options = options,
	}

	for k, _ in pairs (primary) do table.insert (L.primary, k) end

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection) do
		sol[i] = options[iopt]	
	end

	lu.assertEquals (#L.primary, 4*9*9)
	lu.assertEquals (count, 9*9*9)
	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {
	})

end

os.exit( lu.LuaUnit.run() )
