
-- script that print the table used in md5

local t = {}

for i = 0, 63 do
    -- Let T[i] denote the i-th element of the table, which
    -- is equal to the integer part of 4294967296 times abs(sin(i)), where i
    -- is in radians.
    t[i] = math.floor(4294967296 * math.abs(math.sin(i + 1)))
    print(string.format('%x', t[i]))
end

