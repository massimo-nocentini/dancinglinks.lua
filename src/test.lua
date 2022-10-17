
local lu = require 'luaunit'
local dl = require 'dl'

local function langfordpair (n)
	
	local primary, secondary = {}, nil	-- items.
	local options = {}

	for i = 1, n do

		local id_i = 'v_'..tostring(i)

		for j = 1, 2 * n do
			local id_j = 'f_'..tostring(j)

			local k = i + j + 1

			if k <= 2 * n then
				if not primary[id_i] then primary[id_i] = dl.item(id_i, i) end
				local item_i = primary[id_i]

				if not primary[id_j] then primary[id_j] = dl.item(id_j, j) end
				local item_j = primary[id_j]

				local id_k = 's_'..tostring(k)
				if not primary[id_k] then primary[id_k] = dl.item(id_k, k) end
				local item_k = primary[id_k]

				table.insert(options, {item_i, item_j, item_k})
			end
		end
	end
	
	local problem = {
		primary = primary,
		secondary = secondary,
		options = options,
	}

	return problem
end

function test_langfordpairs ()

	--lu.assertEquals ({}, langfordpair (3))
	local L = langfordpair (3)

	local P = table.pack(dl.problem (L))
	local llink, rlink, ulink, dlink, len, top, primary_header, first_secondary_item = table.unpack(P)

	local v_1 = L.primary['v_1']

	lu.assertNil(first_secondary_item)

	lu.assertNil (L.primary['f_5'])
	lu.assertNil (L.primary['f_6'])
	lu.assertNil (L.primary['s_1'])
	lu.assertNil (L.primary['s_2'])

	lu.assertEquals (4, len[L.primary['v_1']])
	lu.assertEquals (3, len[L.primary['v_2']])
	lu.assertEquals (2, len[L.primary['v_3']])
	lu.assertEquals (3, len[L.primary['f_1']])
	lu.assertEquals (3, len[L.primary['f_2']])
	lu.assertEquals (2, len[L.primary['f_3']])
	lu.assertEquals (1, len[L.primary['f_4']])
	lu.assertEquals (1, len[L.primary['s_3']])
	lu.assertEquals (2, len[L.primary['s_4']])
	lu.assertEquals (3, len[L.primary['s_5']])
	lu.assertEquals (3, len[L.primary['s_6']])

	lu.assertEquals (L.primary['v_1'], ulink[ulink[ulink[ulink[ulink[L.primary['v_1']]]]]])
	lu.assertEquals (L.primary['v_1'], dlink[dlink[dlink[dlink[dlink[L.primary['v_1']]]]]])

	lu.assertEquals (L.primary['v_2'], ulink[ulink[ulink[ulink[L.primary['v_2']]]]])
	lu.assertEquals (L.primary['v_2'], dlink[dlink[dlink[dlink[L.primary['v_2']]]]])

	lu.assertEquals (L.primary['v_3'], ulink[ulink[ulink[L.primary['v_3']]]])
	lu.assertEquals (L.primary['v_3'], dlink[dlink[dlink[L.primary['v_3']]]])

	local solver = coroutine.create(dl.solver)
	local flag, value = coroutine.resume (solver, table.unpack(P))
	print(value)
	print (table.concat (value, ', '))
end





os.exit( lu.LuaUnit.run() )
