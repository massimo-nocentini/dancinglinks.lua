
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
	lu.assertEquals (sol, 
	{
	    {
		"l_utah",
		["p_1,4"]={color=117},
		["p_2,4"]={color=116},
		["p_3,4"]={color=97},
		["p_4,4"]={color=104}
	    },
	    {
		"l_knuth",
		["p_2,1"]={color=107},
		["p_2,2"]={color=110},
		["p_2,3"]={color=117},
		["p_2,4"]={color=116},
		["p_2,5"]={color=104}
	    }
	})

end

function test_partridge_options_n_equals_2 ()

	local n = 2
	local N = n * (n + 1) / 2

	local p, v = ec.indexed('p'), ec.indexed('v')

	local L = { items = {}, options = {} }	-- our problem

	for k = 1, n do 

		L.items[ v {k} ] = { isprimary = true, atleast = k, atmost = k }

		for i = 0, N - k do for j = 0, N - k do

			local option = { v {k} }

			for u = 0, k - 1 do for r = 0, k - 1 do
				L.items[ p {i + u, j + r} ] = { isprimary = true }

				table.insert(option, p {i + u, j + r})
			end end

			table.insert(L.options, option) 
		end end 
	end

	lu.assertEquals (L.options, {
		    {"v_1", "p_0,0"},
		    {"v_1", "p_0,1"},
		    {"v_1", "p_0,2"},
		    {"v_1", "p_1,0"},
		    {"v_1", "p_1,1"},
		    {"v_1", "p_1,2"},
		    {"v_1", "p_2,0"},
		    {"v_1", "p_2,1"},
		    {"v_1", "p_2,2"},
		    {"v_2", "p_0,0", "p_0,1", "p_1,0", "p_1,1"},
		    {"v_2", "p_0,1", "p_0,2", "p_1,1", "p_1,2"},
		    {"v_2", "p_1,0", "p_1,1", "p_2,0", "p_2,1"},
		    {"v_2", "p_1,1", "p_1,2", "p_2,1", "p_2,2"} })

	local _, solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)

	lu.assertTrue (flag)
	lu.assertEquals (sol, nil)

end

function est_partridge ()

	local n = 8
	local N = math.tointeger(n * (n + 1) / 2)

	local p, v = ec.indexed('p'), ec.indexed('v')

	local L = { items = {}, options = {} }	-- our problem

	for k = 1, n do L.items[ v {k} ] = { isprimary = true, atleast = k, atmost = k } end

	for i = 0, N - 1 do for j = 0, N - 1 do L.items[ p {i, j} ] = { isprimary = true } end end

	for k = 1, n do for i = 0, N - k do for j = 0, N - k do

		local option = { v {k} }

		for u = 0, k - 1 do for r = 0, k - 1 do table.insert(option, p {i + u, j + r}) end end

		table.insert(L.options, option) 

	end end end

	local _, solver = ec.solver (L, false)

	lu.assertEquals (N, 36)
	lu.assertEquals (L.primarysize, N*N + n)
	lu.assertEquals (L.secondarysize, 0)
	lu.assertEquals (#L.options, L.optionssize)

	print 'first resume'
	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		sol[i] = L.options[iopt]	
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {})

end

function est_partridge_xcc ()

	local n = 8
	local N = math.tointeger(n * (n + 1) / 2)

	local p, v = ec.indexed('p'), ec.indexed('v')

	local L = { items = {}, options = {} }	-- our problem

	for k = 1, n do L.items[ v {k} ] = { isprimary = true, atleast = k, atmost = k } end

	for i = 0, N - 1 do for j = 0, N - 1 do L.items[ p {i, j} ] = { isprimary = true } end end

	for k = 1, n do for i = 0, N - k do for j = 0, N - k do

		local option = { v {k} }

		for u = 0, k - 1 do for r = 0, k - 1 do table.insert(option, p {i + u, j + r}) end end

		table.insert(L.options, option) 

	end end end

	local solver = ec.solver (L, true)

	lu.assertEquals (N, 36)
	lu.assertEquals (L.primarysize, N*N + N)
	lu.assertEquals (L.secondarysize, 7196)
	lu.assertEquals (#L.options, 8492)
	lu.assertEquals (L.optionssize, 35484)

	print 'first resume'
	local flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		sol[i] = L.options[iopt]	
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {})

end

function test_mcc_simple ()

	local v = ec.indexed('v')

	local L = { items = {}, options = {} }

	L.items[ v {'a'} ] = { isprimary = true } 
	L.items[ v {'b'} ] = { isprimary = true } 
	L.items[ v {'c'} ] = { isprimary = true, atleast = 2, atmost = 3 } 
	L.items[ v {'x'} ] = { isprimary = false } 
	L.items[ v {'y'} ] = { isprimary = false } 

	L.options = {
		{v {'c'},          [v {'y'}] = {color = 1}},
		{v {'a'}, v {'b'}, [v {'x'}] = {color = 0}, [v {'y'}] = {color = 0}},
		{v {'a'}, v {'c'}, [v {'x'}] = {color = 1}, [v {'y'}] = {color = 1}},
		{v {'c'}, 	   [v {'x'}] = {color = 0}},
		{v {'b'},          [v {'x'}] = {color = 1}},
	}

	local _, solver = ec.solver (L)

	local flag, selection = coroutine.resume (solver)
	
	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		sol[i] = L.options[iopt]	
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {{"v_a", "v_c", v_x={color=1}, v_y={color=1}}, {"v_b", v_x={color=1}}, {"v_c", v_y={color=1}}})

	-- no more solutions.
	flag, selection = coroutine.resume (solver)
	lu.assertTrue (flag)
	lu.assertNil (selection)
end

function test_mcc_simple_xcc_manual ()

	local v = ec.indexed('v')
	local o = ec.indexed('o')

	local L = { items = {}, options = {} }

	local t, w, s = os.tmpname (), os.tmpname (), os.tmpname ()

	L.items[ v {'a'} ] = { isprimary = true } 
	L.items[ v {'b'} ] = { isprimary = true } 
	L.items[ v {'c', 1} ] = { isprimary = true } 
	L.items[ v {'c', 2} ] = { isprimary = true } 
	L.items[ v {'c', 3} ] = { isprimary = false } 
	L.items[ v {'x'} ] = { isprimary = false } 
	L.items[ v {'y'} ] = { isprimary = false } 

	L.items[ o { t } ] = { isprimary = false } 
	L.items[ o { w } ] = { isprimary = false } 
	L.items[ o { s } ] = { isprimary = false } 

	local e1, e2, e3 =
		{"v_c", v_y={color=1}},
		{"v_a", "v_c", v_x={color=1}, v_y={color=1}},
		{v {'c'}, [v {'x'}] = {color = 0}}

	local o1, o2, o3 = 
		{ o { t }, v {'c', 1}, [v {'y'}] = {color = 1}},
		{ o { t }, v {'c', 2}, [v {'y'}] = {color = 1}},
		{ o { t }, v {'c', 3}, [v {'y'}] = {color = 1}} 

	local o4, o5, o6 =
		{ o { w }, v {'a'}, v {'c', 1}, 	[v {'x'}] = {color = 1}, [v {'y'}] = {color = 1}},
		{ o { w }, v {'a'}, v {'c', 2}, 	[v {'x'}] = {color = 1}, [v {'y'}] = {color = 1}},
		{ o { w }, v {'a'}, v {'c', 3}, 	[v {'x'}] = {color = 1}, [v {'y'}] = {color = 1}} 

	local o7, o8, o9 =
		{ o { s }, v {'c', 1}, 	   	[v {'x'}] = {color = 0}},
		{ o { s }, v {'c', 2}, 	   	[v {'x'}] = {color = 0}},
		{ o { s }, v {'c', 3}, 	   	[v {'x'}] = {color = 0}}

	L.options = {
		o1, o2, o3,
		{ v {'a'}, v {'b'}, [v {'x'}] = {color = 0}, [v {'y'}] = {color = 0}},
		o4, o5, o6,
		o7, o8, o9,
		{ v {'b'}, [v {'x'}] = {color = 1}},
	}

	local map = {}

	map[o1] = e1
	map[o2] = e1
	map[o3] = e1

	map[o4] = e2
	map[o5] = e2
	map[o6] = e2

	map[o7] = e3
	map[o8] = e3
	map[o9] = e3

	local solver, _ = ec.solver (L)

	local flag, selection = coroutine.resume (solver)
	
	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		local o = L.options[iopt]	
		local m = map[o]
		if m then sol[i] = m else sol[i] = o end
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {e2, {"v_b", v_x={color=1}}, e1})

	-- one more solution, namely the same as before because of combinations.
	flag, selection = coroutine.resume (solver)

	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		local o = L.options[iopt]	
		local m = map[o]
		if m then sol[i] = m else sol[i] = o end
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {e2, {"v_b", v_x={color=1}}, e1})

	-- no more solutions.
	flag, selection = coroutine.resume (solver)
	lu.assertNil (selection)

end

function test_mcc_simple_xcc_automatic ()

	local v = ec.indexed('v')
	local o = ec.indexed('o')

	local L = { items = {}, options = {} }

	L.items[ v {'a'} ] = { isprimary = true } 
	L.items[ v {'b'} ] = { isprimary = true } 
	L.items[ v {'c'} ] = { isprimary = true, atleast = 2, atmost = 3 } 
	L.items[ v {'x'} ] = { isprimary = false } 
	L.items[ v {'y'} ] = { isprimary = false } 

	L.options = {
		{v {'c'},          [v {'y'}] = {color = 1}},
		{v {'a'}, v {'b'}, [v {'x'}] = {color = 0}, [v {'y'}] = {color = 0}},
		{v {'a'}, v {'c'}, [v {'x'}] = {color = 1}, [v {'y'}] = {color = 1}},
		{v {'c'}, 	   [v {'x'}] = {color = 0}},
		{v {'b'},          [v {'x'}] = {color = 1}},
	}

	local solver, _ = ec.solver (L, true)

	local flag, selection = coroutine.resume (solver)
	
	local sol = {}
	for i, iopt in ipairs(selection or {}) do
		sol[i] = L.options[iopt]	
		--print (table.concat (sol[i], ' '))
	end

	lu.assertTrue (flag)
	lu.assertItemsEquals (sol, {{"v_a", "v_c", v_x={color=1}, v_y={color=1}}, {"v_b", v_x={color=1}}, {"v_c", v_y={color=1}}})

	-- no more solutions.
	flag, selection = coroutine.resume (solver)
	lu.assertTrue (flag)
	lu.assertNil (selection)
end


os.exit( lu.LuaUnit.run() )
