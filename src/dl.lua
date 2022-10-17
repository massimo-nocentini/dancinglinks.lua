
local heapq = require 'heapq'

local dl = {}

function dl.solver (llink, rlink, ulink, dlink, len, top, primary_header, first_secondary_item)

	local function iscovered ()
		return rlink[primary_header] == primary_header end
		
	local function looparound_do (start, toward, f, inclusive)
		if inclusive then
			local each = start
			repeat f (each); each = toward[each] until each == start
		else
			local each = toward[start]
			while each ~= start do f (each); each = toward[each] end
		end
	end

	local options = {}

	local function option (ref)

		local tbl = {}
		setmetatable(tbl, {
			__tostring = function (t) 
				local m = {}
				for i, item in ipairs (t) do m[i] = tostring(item) end	-- necessary for the next `concat`.
				return table.concat(m, ', ') 
			end})

		local function T (each) table.insert (tbl, top[each]) end
		looparound_do (ref, rlink, T, true)

		return tbl
	end

	-- COVERING ------------------------------------------------------------------

	local function H (q)
		local x, u, d = top[q], ulink[q], dlink[q]
		dlink[u], ulink[d] = d, u
		len[x] = len[x] - 1
	end

	local function hide (p) looparound_do(p, rlink, H) end

	local function cover (i)
		looparound_do(i, dlink, hide)
		local l, r = llink[i], rlink[i]
		rlink[l], llink[r] = r, l
	end

	-- UNCOVERING ----------------------------------------------------------------

	local function U (q)
		local x, u, d = top[q], ulink[q], dlink[q]
		dlink[u], ulink[d] = q, q
		len[x] = len[x] + 1
	end

	local function unhide (p) looparound_do(p, llink, U) end

	local function uncover (i)
		local l, r = llink[i], rlink[i]
		rlink[l], llink[r] = i, i
		looparound_do(i, ulink, unhide)	
	end

	------------------------------------------------------------------------------
	
	::x1::
	local l, item, ref = 1, nil, nil	-- to silent the interpreter on goto checks.

	::x2::	
	if iscovered () then coroutine.yield(options); goto x8 end

	::x3::
	item = rlink[primary_header]	-- just pick the next item to be covered.
	assert(primary_header ~= item)	-- which hasn't to be the `primary_header`.

	::x4::
	cover (item)
	ref = dlink[item]
	options[l] = option (ref)
	print (options[l])

	::x5::
	if ref == item then print (item) goto x7
	else
		looparound_do(ref, rlink, function (p) cover(top[p]) end)
		print 'incrementing l'
		l = l + 1; goto x2
	end

	::x6::
	looparound_do(ref, llink, function (p) uncover(top[p]) end)
	item = top[ref]
	ref = dlink[ref]
	goto x5

	::x7::
	uncover (item)
	
	::x8::
	options[l] = nil	-- cleaning it up for the next solution.
	if l > 1 then l = l - 1; goto x6 end

end

function dl.problem (P)

	local llink, rlink, ulink, dlink, len, top = {}, {}, {}, {}, {}, {}

	local primary_header = {}
	local last_primary_item = primary_header	-- cursor variable for primary items.

	for id, item in pairs(P.primary) do	-- link primary items
		ulink[item], dlink[item] = item, item	-- self loops on the vertical dimension.
		len[item] = 0

		llink[item], rlink[last_primary_item] = last_primary_item, item	-- link among the horizontal dimension.
		last_primary_item = item
	end

	rlink[last_primary_item], llink[primary_header] = primary_header, last_primary_item	-- closing the doubly circular list.

	local first_secondary_item = nil
	if P.secondary then

		local secondary_header = {}
		local last_secondary_item = secondary_header	

		for id, item in pairs(P.secondary) do	-- link secondary items
			ulink[item], dlink[item] = item, item
			len[item] = 0

			llink[item], rlink[last_secondary_item] = last_secondary_item, item
			last_secondary_item = item
		end

		assert (not llink[secondary_header])
		first_secondary_item = rlink[secondary_header]
		rlink[secondary_header] = nil
		rlink[last_primary_item], llink[first_secondary_item] = first_secondary_item, last_secondary_item
	end

	for _, opt in ipairs(P.options) do

		local header = {}
		local last = header

		for _, o in ipairs(opt) do
			len[o] = len[o] + 1

			local point = {}	-- every single 1 in the model matrix.
			top[point] = o
			local q = ulink[o]
			ulink[point] = q
			ulink[o] = point
			dlink[point] = dlink[q]
			dlink[q] = point

			llink[point] = last
			rlink[last] = point
			last = point
		end

		assert (not llink[header])
		local first = rlink[header]
		rlink[header] = nil
		rlink[last] = first
		llink[first] = last
	end

	return llink, rlink, ulink, dlink, len, top, primary_header, first_secondary_item
end

function dl.item(id, value)
	local t = {id = id, value = value}
	setmetatable(t, {__tostring = function (v) return id end})
	return t
end

return dl
