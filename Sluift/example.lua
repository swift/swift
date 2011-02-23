require "sluift"

c = sluift.connect(os.getenv("SWIFT_CLIENTTEST_JID"), os.getenv("SWIFT_CLIENTTEST_PASS"))
v = c:get_version("swift.im")
print(v["name"], v["version"], v["os"])
c:disconnect()
