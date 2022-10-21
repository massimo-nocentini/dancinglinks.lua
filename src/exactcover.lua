
local dl = {}

function dl.solver (P)

	local function monus (x, y) return math.max (x - y, 0) end

	local nocolor, handledcolor, noop = {}, {}, function () end	-- just witnesses.

	local llink, rlink, ulink, dlink, len, top, option, color, slack, bound, firsttweaks = {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}

	local primary_header = {}
	local last_primary_item = primary_header	-- cursor variable for primary items.

	local primarysize, secondarysize = 0, 0

	for item, descriptor in pairs(P.items) do	-- link primary items

		len[item] = 0

		if descriptor.isprimary then

			ulink[item], dlink[item] = item, item	-- self loops on the vertical dimension.
			llink[item], rlink[last_primary_item] = last_primary_item, item	-- link among the horizontal dimension.

			local u, v = descriptor.atleast or 1, descriptor.atmost or 1
			assert (u >= 0 and v > 0 and v >= u)
			slack[item], bound[item] = v - u, v

			last_primary_item = item
			primarysize = primarysize + 1
		else 
			ulink[item], dlink[item] = item, item
			llink[item], rlink[item] = item, item

			secondarysize = secondarysize + 1
		end
	end

	P.primarysize, P.secondarysize = primarysize, secondarysize	-- update the given problem, in place.

	rlink[last_primary_item], llink[primary_header] = primary_header, last_primary_item	-- closing the doubly circular list.

	for iopt, opt in ipairs(P.options) do

		local header = {}
		local last = header

		for id, decoration in pairs(opt) do

			if type (id) == 'number' then id, decoration = decoration, {} end
			
			local o = id

			len[o] = len[o] + 1

			local point = {}	-- every single 1 in the model matrix.

			top[point], option[point], color[point] = o, iopt, decoration.color or nocolor

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

	-- HELPERS  ------------------------------------------------------------------

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

	local function nextitem_naive () 
		return rlink[primary_header] end

	local function nextitem_minlen () 

		local function branch (each) 
			return monus (len[each] + 1, monus (bound[each], slack[each])) end

		local max, item = math.huge, nil

		loop (primary_header, rlink, function (each) 

			local lambda = branch (each)

			local es = slack[each]
			if (lambda < max)
				or (lambda == max and ec < slack[item])
				or (lambda == max and ec == slack[item] and len[each] > len[item])
				then max, item = lambda, each end
		end)

		return item, max
	end

	-- COVERING ------------------------------------------------------------------

	local function H (q)

		if q == handledcolor then return end

		local x, u, d = top[q], ulink[q], dlink[q]
		dlink[u], ulink[d] = d, u
		len[x] = len[x] - 1
	end

	local function hide (p) loop(p, rlink, H) end

	local function purify (p)

		local c = color[p]
		
		loop (top[p], dlink, function (q)
			if color[q] == c then color[q] = handledcolor else hide (q) end
		end)
	end

	local function cover (i)
		loop(i, dlink, hide)
		local l, r = llink[i], rlink[i]
		rlink[l], llink[r] = r, l
	end

	local function commit (i, p)
		if color[p] == nocolor then cover (i) else purify (p) end
	end

	local function covertop (p)
		local item = top[p]
		local b = bound[item] 
		if b then 
			b = b - 1
			bound[item] = b
			if b == 0 then cover (item) else commit (item, p) end
		end
	end

	-- UNCOVERING ----------------------------------------------------------------

	local function U (q)

		if q == handledcolor then return end

		local x, u, d = top[q], ulink[q], dlink[q]
		dlink[u], ulink[d] = q, q
		len[x] = len[x] + 1
	end

	local function unhide (p) loop(p, llink, U) end

	local function unpurify (p)

		local c = color[p]
		
		loop (top[p], ulink, function (q)
			if color[q] == handledcolor then color[q] = c else unhide (q) end
		end)
	end

	local function uncover (i)
		local l, r = llink[i], rlink[i]
		rlink[l], llink[r] = i, i
		loop(i, ulink, unhide)	
	end

	local function uncommit (i, p)
		if color[p] == nocolor then uncover (i) else unpurify (p) end
	end

	local function uncovertop (p) 
		local item = top[p]
		local b = bound[item] 
		if b then 
			b = b + 1
			bound[item] = b
			if b == 1 then uncover (item) else uncommit (item, p) end
		end
	end

	-- TWEAKING ------------------------------------------------------------------
	
	local function tweakw (p, x)
		local d = dlink[x]
		dlink[p], ulink[d] = d, p
		len[p] = len[p] - 1
	end

	local function tweak (p, x) hide (x); wtweak (p, x) end

	-- UNTWEAKING ------------------------------------------------------------------
	
	local function untweakf (l, f)

		local k = 0
		local a = firsttweaks[l]
		local p = top[a]
		local x, y = a, p
		local z = dlink[p]

		dlink[p] = x
		while x ~= z do
			ulink[x] = y
			k = k + 1
			f (x)
			y = x
			x = dlink[x]
		end
		ulink[z] = y
		len[p] = len[p] + k
		return p
	end

	local function untweak (l) untweakf (l, unhide) end

	local function untweakw (l) uncover (untweakf (l, noop)) end	-- this is the weak version.

	------------------------------------------------------------------------------

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
			local item, branch = nextitem_minlen ()

			if branch == 0 then goto M9 end

			local b, s, xl = bound[item] - 1, slack[item], dlink[item]
			bound[item] = b

			if b == 0 then cover (item) end
			if b > 0 or s > 0 then firsttweaks[l] = xl end

			loop (item, dlink, function (ref) 

				local b, s = bound[item], slack[item]

				if b == 0 and s == 0 then

					loop (ref, rlink, covertop)

					R (l + 1, { 
						level = l, 
						index = option[ref], 
						nextoption = opt, 
					})

					loop (ref, llink, uncovertop)
				
				elseif len[item] > b - s then 
					if b == 0 then tweakw (item, ref) 
					else 
						tweak (item, ref) 

						--local p, q = llink[item], rlink[item]
						--rlink[p], llink[q] = q, p
					end
				end
			end)

			if bound[item] == 0 then
				if slack[item] == 0 then uncover (item) 
				else untweakw (l) end
			else untweak (l) end

			bound[item] = bound[item] + 1

			firsttweaks[l] = nil

			::M9::
		end
	end

	return coroutine.create (function () R (1, nil) end)
end

function dl.indexed(name)

	return function (tbl)
		local seq = {}
		for i, item in ipairs(tbl) do seq[i] = item end
		return name .. '_' .. table.concat (seq, ',')
	end	
end

return dl
