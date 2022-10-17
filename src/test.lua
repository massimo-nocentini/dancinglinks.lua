
local lu = require 'luaunit'
local dl = require 'dl'

local function matrix ()
	
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
	
	local problem = {
		primary = primary,
		options = options,
	}

	return problem
end

local function langfordpair ()
	
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
	
	local problem = {
		primary = primary,
		options = options,
	}

	return problem
end

local function langfordpair_1 (n)
	
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
				local item_i = primary[id_i]

				local item_j = primary[id_j]

				local id_k = 's_'..tostring(k)
				local item_k = primary[id_k]

				table.insert(options, {item_i, item_j, item_k})
			end
		end
	end
	
	local problem = {
		primary = primary,
		options = options,
	}

	return problem
end


function test_matrix ()

	local L = matrix ()

	local P = table.pack(dl.problem (L))
	local llink, rlink, ulink, dlink, len, top, primary_header, first_secondary_item = table.unpack(P)

	--local solver = coroutine.create(dl.solver)
	--local flag, value = coroutine.resume (solver, table.unpack(P))
	--
	local solver = dl.solver (table.unpack(P))
	local flag, value = coroutine.resume (solver)
	lu.assertTrue (flag)
	lu.assertItemsEquals (value, {
		{L.primary.v_4, L.primary.v_6, L.primary.v_1},
		{L.primary.v_3, L.primary.v_5},
		{L.primary.v_2, L.primary.v_7},
	})

	local flag, value = coroutine.resume (solver)
	lu.assertTrue (flag)
	lu.assertNil (value)	-- just one solution.
end

function test_langfordpairs ()

	local L = langfordpair ()

	local P = table.pack(dl.problem (L))
	local llink, rlink, ulink, dlink, len, top, primary_header, first_secondary_item = table.unpack(P)

	local solver = dl.solver (table.unpack(P))
	local flag, value = coroutine.resume (solver)
	local flag, value = coroutine.resume (solver)
	local flag, value = coroutine.resume (solver)
	lu.assertNil(value)
end

function test_langfordpairs_1 ()

	local L = langfordpair_1 (11)

	local P = table.pack(dl.problem (L))
	local solver = dl.solver (table.unpack(P))

	local count = -1
	repeat
		local flag, value = coroutine.resume (solver)
		count = count + 1
	until not value

	print(count)
end

os.exit( lu.LuaUnit.run() )
