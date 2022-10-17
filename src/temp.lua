
	[-[
	::x1::
	local l, item, ref = 1, nil, {}	-- to silent the interpreter on goto checks.

	::x2::	
	if iscovered () then coroutine.yield(options); goto x8 end

	::x3::
	item = rlink[primary_header]		-- just pick the next item to be covered.
	assert(primary_header ~= item)		-- which hasn't to be the `primary_header`.

	::x4::
	cover (item)
	ref[l] = dlink[item]
	options[l] = option (ref[l])
	print (options[l])

	::x5::
	if ref[l] == item then goto x7
	else
		looparound_do(ref[l], rlink, function (p) cover(top[p]) end)
		l = l + 1; goto x2
	end

	::x6::
	looparound_do(ref[l], llink, function (p) uncover(top[p]) end)
	item = top[ref[l]]
	ref[l] = dlink[ref[l]]
	goto x5

	::x7::
	uncover (item)
	
	::x8::
	options[l] = nil	-- cleaning it up for the next solution.
	item, ref[l] = nil, nil
	if l > 1 then l = l - 1; goto x6 end
	]-]
