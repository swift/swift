-- Test connect with function parameter
client = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
x, y, z = client:connect(function (c) return 1, '2', true end)
assert(x == 1)
assert(y == '2')
assert(z == true)
