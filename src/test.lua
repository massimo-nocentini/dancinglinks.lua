
local lu = require 'luaunit'
local dl = require 'dl'

function test_matrix ()

	local primary = {
		v_1 = dl.item('v_1', 1),
		v_2 = dl.item('v_2', 2),
		v_3 = dl.item('v_3', 3),
		v_4 = dl.item('v_4', 4),
		v_5 = dl.item('v_5', 5),
		v_6 = dl.item('v_6', 6),
		v_7 = dl.item('v_7', 7),
	}

	local options = {
		{primary.v_3, primary.v_5},
		{primary.v_1, primary.v_4, primary.v_7},
		{primary.v_2, primary.v_3, primary.v_6},
		{primary.v_1, primary.v_4, primary.v_6},
		{primary.v_2, primary.v_7},
		{primary.v_4, primary.v_5, primary.v_7},
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

	local primary = {
		v_1 = dl.item('v_1', 1),
		v_2 = dl.item('v_2', 2),
		v_3 = dl.item('v_3', 3),
		s_1 = dl.item('s_1', 1),
		s_2 = dl.item('s_2', 2),
		s_3 = dl.item('s_3', 3),
		s_4 = dl.item('s_4', 4),
		s_5 = dl.item('s_5', 5),
		s_6 = dl.item('s_6', 6),
	}

	local options = {
		{primary.v_1, primary.s_1, primary.s_3},
		{primary.v_1, primary.s_2, primary.s_4},
		{primary.v_1, primary.s_3, primary.s_5},
		{primary.v_1, primary.s_4, primary.s_6},
		{primary.v_2, primary.s_1, primary.s_4},
		{primary.v_2, primary.s_2, primary.s_5},
		{primary.v_2, primary.s_3, primary.s_6},
		{primary.v_3, primary.s_1, primary.s_5},
		{primary.v_3, primary.s_2, primary.s_6},
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

	local primary = {}	-- items.
	local options = {}

	for i = 1, 2 * n do
		local id_i = 's_'..tostring(i)
		primary[id_i] = dl.item (id_i, i)
	end

	for i = 1, n do

		local id_i = 'v_'..tostring(i)
		primary[id_i] = dl.item (id_i, i)

		for j = 1, 2 * n do
			local id_j = 's_'..tostring(j)

			local k = i + j + 1

			if k <= 2 * n then
				local id_k = 's_'..tostring(k)
				local item_k = primary[id_k]
				local item_i = primary[id_i]
				local item_j = primary[id_j]
				table.insert(options, {item_i, item_j, item_k})
			end
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

os.exit( lu.LuaUnit.run() )
