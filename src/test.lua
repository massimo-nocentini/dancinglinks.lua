
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
	local solver = coroutine.create(dl.solver)
	local P = table.pack(dl.problem (langfordpair (3)))
	--lu.assertEquals (P, {})
	local flag, value = coroutine.resume (solver, table.unpack(P))
	print (table.concat (value, ', '))
end





os.exit( lu.LuaUnit.run() )
