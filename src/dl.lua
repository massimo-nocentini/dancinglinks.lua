
local dl = {}

function dl.solver (llink, rlink, ulink, dlink, len, top, option, primary_header)

	local function iscovered ()
		return rlink[primary_header] == primary_header end
		
	local function loop (start, toward, f, inclusive)

		if inclusive then
			local each = start
			repeat f (each); each = toward[each] until each == start
		else
			local each = toward[start]
			while each ~= start do f (each); each = toward[each] end
		end
	end

	-- COVERING ------------------------------------------------------------------

	local function H (q)
		local x, u, d = top[q], ulink[q], dlink[q]
		dlink[u], ulink[d] = d, u
		len[x] = len[x] - 1
	end

	local function hide (p) loop(p, rlink, H) end

	local function cover (i)
		loop(i, dlink, hide)
		local l, r = llink[i], rlink[i]
		rlink[l], llink[r] = r, l
	end

	local function covertop (p) 	cover (top[p]) end

	-- UNCOVERING ----------------------------------------------------------------

	local function U (q)
		local x, u, d = top[q], ulink[q], dlink[q]
		dlink[u], ulink[d] = q, q
		len[x] = len[x] + 1
	end

	local function unhide (p) loop(p, llink, U) end

	local function uncover (i)
		local l, r = llink[i], rlink[i]
		rlink[l], llink[r] = i, i
		loop(i, ulink, unhide)	
	end

	local function uncovertop (p) 	uncover (top[p]) end

	------------------------------------------------------------------------------

	local function nextitem_naive () 
		return rlink[primary_header] end

	local function nextitem_minlen () 

		local max, item = math.huge, nil
		loop (primary_header, rlink, function (each) 
			local m = len[each] 
			if m < max then max, item = m, each end
		end)

		return item
	end

	local function R (l, opt)

		if iscovered () then 
			local cpy = {} 
			while opt do 
				cpy[opt.level] = opt.index 
				opt = opt.nextoption 
			end

			table.sort(cpy)
			coroutine.yield (cpy)
		else
			local item = nextitem_minlen ()

			cover (item)

			loop (item, dlink, function (ref) 

				loop (ref, rlink, covertop)

				R (l + 1, { 
					level = l, 
					index = option[ref], 
					nextoption = opt, 
				})

				loop (ref, llink, uncovertop)

			end)

			uncover (item)
		end
	end

	return coroutine.create (function () R (1, nil) end)
end

function dl.problem (P)

	local llink, rlink, ulink, dlink, len, top, option = {}, {}, {}, {}, {}, {}, {}

	local primary_header = {}
	local last_primary_item = primary_header	-- cursor variable for primary items.

	for _, item in ipairs(P.primary) do	-- link primary items
		ulink[item], dlink[item] = item, item	-- self loops on the vertical dimension.
		len[item] = 0

		llink[item], rlink[last_primary_item] = last_primary_item, item	-- link among the horizontal dimension.
		last_primary_item = item
	end

	rlink[last_primary_item], llink[primary_header] = primary_header, last_primary_item	-- closing the doubly circular list.

	for _, item in pairs(P.secondary or {}) do	-- link secondary items
		len[item] = 0
		ulink[item], dlink[item] = item, item
		llink[item], rlink[item] = item, item
	end

	for iopt, opt in ipairs(P.options) do

		local header = {}
		local last = header

		for _, id in ipairs(opt) do

			local o = id

			len[o] = len[o] + 1

			local point = {}	-- every single 1 in the model matrix.

			top[point], option[point] = o, iopt

			local q = ulink[o]
			ulink[point], ulink[o] = q, point
			dlink[point], dlink[q] = o, point

			llink[point], rlink[last]  = last, point
			last = point
		end

		assert (not llink[header])
		local first = rlink[header]
		rlink[header] = nil
		rlink[last], llink[first]  = first, last
	end

	return llink, rlink, ulink, dlink, len, top, option, primary_header
end

function dl.indexed(name)

	local r = {base = name}
	setmetatable(r, {__index = function (t, i) return t.base..'_'..tostring(i) end})
	return r
end

return dl
