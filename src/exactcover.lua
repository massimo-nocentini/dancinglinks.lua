
local dl = {}

local function disconnect (q, rel, irel) 
	local u, d = rel[q], irel[q]
	rel[d], irel[u] = u, d
end

local function connect (q, rel, irel) 
	local u, d = rel[q], irel[q]
	irel[u], rel[d] = q, q
end

local function monus (x, y) return math.max (x - y, 0) end

local nocolor, handledcolor, noop = {}, {}, function () end	-- just witnesses.

function dl.solver (P)

	local llink, rlink, ulink, dlink, len, top, option, color, slack, bound  = {}, {}, {}, {}, {}, {}, {}, {}, {}, {}

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

	local function iscovered () return rlink[primary_header] == primary_header end
		
	local function loop (start, toward, f, inclusive)

		if inclusive then
			local each = start
			repeat f (each); each = toward[each] until each == start
		else
			local each = toward[start]
			while each ~= start do 
				local v = f (each)
				if v then return v else each = toward[each] end
			end
		end
	end

	local function nextitem_naive () 
		local item = rlink[primary_header]
		return item, len[item] 
	end

	local function nextitem_randomized () 

		local min, pool = math.huge, nil

		loop (primary_header, rlink, function (each) 

			local l = len[each]
			if l < min then min, pool = l, {each} 
			elseif l == min then table.insert (pool, each) end
		end)

		assert (#pool > 0)	-- ensure we've found something.

		return pool[math.random (#pool)], min
	end

	local function nextitem_minlen () 

		local function branch (each) return monus (len[each] + 1, monus (bound[each], slack[each])) end

		local min, item = math.huge, nil

		loop (primary_header, rlink, function (each) 

			local lambda = branch (each)

			local es = slack[each]

			if (lambda < min) 
			   or (lambda == min and es < slack[item]) 
			   or (lambda == min and es == slack[item] and len[each] > len[item])
				then min, item = lambda, each end
		end)

		assert (item)	-- ensure we've found something.

		return item, min
	end

	local function connectv (q) return connect (q, ulink, dlink) end
	local function disconnectv (q) return disconnect (q, ulink, dlink) end

	local function connecth (q) return connect (q, llink, rlink) end
	local function disconnecth (q) return disconnect (q, llink, rlink) end

	-- COVERING ------------------------------------------------------------------

	local function H (q)

		if color[q] == handledcolor then return end

		disconnectv (q)

		local x = top[q]
		len[x] = len[x] - 1 
	end

	local function hide (p) loop(p, rlink, H) end

	local function purify (p, c)
		loop (top[p], dlink, function (q)
			if color[q] == c then color[q] = handledcolor else hide (q) end
		end)
	end

	local function cover (i)
		loop(i, dlink, hide)
		disconnecth (i)
	end

	local function commit (i, p)
		local c = color[p]
		if c == nocolor then cover (i) elseif c ~= handledcolor then purify (p, c) end
	end

	--local function covertop (p) cover (top[p]) end

	local function covertop (p) commit (top[p], p) end

	local function covertopm (p)
		local item = top[p]
		local b = bound[item] 
		if b then 
			b = b - 1
			bound[item] = b
			if b == 0 then cover (item) end 
		else covertop (p) end
	end

	-- UNCOVERING ----------------------------------------------------------------

	local function U (q)

		if color[q] == handledcolor then return end

		connectv (q)

		local x = top[q]
		len[x] = len[x] + 1
	end

	local function unhide (p) loop(p, llink, U) end

	local function unpurify (p, c)

		loop (top[p], ulink, function (q)
			if color[q] == handledcolor then color[q] = c else unhide (q) end end)
	end

	local function uncover (i)

		connecth (i)
		loop(i, ulink, unhide)	
	end

	local function uncommit (i, p)
		local c = color[p]
		if c == nocolor then uncover (i) elseif c ~= handledcolor then unpurify (p, c) end
	end

	--local function uncovertop (p) uncover (top[p]) end

	local function uncovertop (p) uncommit (top[p], p) end

	local function uncovertopm (p) 
		local item = top[p]
		local b = bound[item] 
		if b then 
			b = b + 1
			bound[item] = b
			if b == 1 then uncover (item) end
		else uncovertop (p) end
	end

	-- TWEAKING ------------------------------------------------------------------
	
	local function tweakw (p, x)
		assert (x == dlink[p] and p == ulink[x])
		local d = dlink[x]
		dlink[p], ulink[d] = d, p
		len[p] = len[p] - 1
	end

	local function tweakh (p, x)
		hide (x)
		tweakw (p, x) 
	end

	local function tweak (p, x)
		if bound[p] == 0 then tweakw (p, x) else tweakh (p, x) end
	end

	-- UNTWEAKING ------------------------------------------------------------------
	
	local function untweakf (a, f)

		local k = 0

		local p
		if bound[a] then p = a else p = top[a] end
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

	local function untweakh (a) untweakf (a, unhide) end
	local function untweakw (a) uncover (untweakf (a, noop)) end
	local function untweak (item, a) if bound[item] == 0 then untweakw (a) else untweakh (a) end end

	------------------------------------------------------------------------------

	local function XCC (l, opt)	-- eXact Cover with Colors.

		if iscovered () then 

			local perm = {}
			while opt do
				perm[opt.level] = opt.index
				opt = opt.nextoption
			end

			table.sort(perm)
			coroutine.yield (perm)
		else
			local item = nextitem_randomized ()

			cover (item)

			loop (item, dlink, function (ref)

				loop (ref, rlink, covertop) 

				XCC (l + 1, { 
					level = l,
					point = ref,
					index = option[ref],
					nextoption = opt,
				})

				loop (ref, llink, uncovertop)
			
			end)
			
			uncover (item)
		end
	end

	local function MCC (l, opt)	-- Multiplicities Cover with Colors.

		if iscovered () then 
			local cpy = {} 
			while opt do 
				local i = opt.index
				if i then table.insert (cpy, i) end
				opt = opt.nextoption 
			end

			table.sort(cpy)
			coroutine.yield (cpy)
		else
			local item, branch = nextitem_minlen ()

			if branch > 0 then
				
				local s = slack[item]	-- the slack `s` doesn't change during the actual recursion step.
				local ft = dlink[item]	-- which stands for `First Tweaks`.

				bound[item] = bound[item] - 1

				if bound[item] == 0 then cover (item) end

				local ref = dlink[item]
				
				::M5::
					if bound[item] == 0 and s == 0 then if ref ~= item then goto M6 else goto M8 end
					elseif (len[item] + s) <= bound[item] then goto M8
					elseif ref ~= item then tweak (item, ref); goto M6
					elseif bound[item] > 0 or s > 0 then goto M7
					else error 'not expected' end

				::M6::
					loop (ref, rlink, covertopm) 

					MCC (l + 1, { 
						level = l,
						point = ref,
						index = option[ref],
						nextoption = opt,
					})

					loop (ref, llink, uncovertopm)

					ref = dlink[ref]

					goto M5

				::M7::
					disconnecth (item)
					--R (l, opt)
					MCC (l + 1, { 
						level = l,
						point = item,
						index = nil,
						nextoption = opt,
					})
					connecth (item)
				
				::M8::
					if bound[item] == 0 and s == 0 then uncover (item) else untweak (item, ft) end
					bound[item] = bound[item] + 1
			end
		end
	end

	local xcc = coroutine.create (function () XCC (1, nil) end)
	local mcc = coroutine.create (function () MCC (1, nil) end)

	return xcc, mcc
end

function dl.indexed(name)

	return function (tbl)
		local seq = {}
		for i, item in ipairs(tbl) do seq[i] = item end
		return name .. '_' .. table.concat (seq, ',')
	end	
end

return dl
