
local heapq = require 'heapq'

local dl = {}

function dl.solver (llink, rlink, ulink, dlink, len, items, items_header)

	local options, nsols = {}, 0

	local function all_items_covered ()
		return rlink[items_header] == items_header end
		
	local function looparound_do (start, toward, f)
		local each = start
		repeat
			f (each)
			each = toward[each]
		until each == start
	end

	local function option (ref)

		local tbl = {}

		local function T (each) table.insert (tbl, each.top) end
			
		looparound_do (ref, rlink, T)

		return tbl
	end

	-- COVERING ------------------------------------------------------------------

	local function H (q) 
		local x, u, d = q.top, ulink[q], dlink[q]
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
		local x, u, d = q.top, ulink[q], dlink[q]
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
	if all_items_covered () then 
		nsols = nsols + 1
		local reward = coroutine.yield(options)
		goto x8
	end

	::x3::
	item = rlink[items_header]	-- just pick the next item to be covered.

	::x4::
	cover (item)
	ref = dlink[item]
	options[l] = option (ref)

	::x5::
	if ref == item then goto x7
	else
		looparound_do(ref, rlink, function (p) cover(p.top) end)
		l = l + 1; goto x2
	end

	::x6::
	looparound_do(ref, llink, function (p) uncover(p.top) end)
	item = ref.top
	ref = dlink[ref]
	goto x5

	::x7::
	uncover (item)
	
	::x8::
	options[l] = nil	-- cleaning it up for the next solution.
	if l > 1 then l = l - 1; goto x6 
	else return nsols end

end


return dl
