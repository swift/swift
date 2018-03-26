#!/usr/bin/env sluift --

-- This is an example of sending a custom payload, a XEP-0372 reference in this
-- case.
--
-- Running this requires a sluift binary somewhere in the PATH. Alternatively,
-- you can feed this to sluift, or to a Lua that has sluift.so in its library
-- path.

require("sluift")

DEFAULT_URI = "http://example.com/kitten.jpg"

function usage(err)
    print("Usage: SendReference.lua jid [tojid] [datauri]")
    print()
    print("This tool will log in as jid, and send a reference to tojid. If no tojid")
    print("is provided, a message to itself is sent. The datauri defaults to")
    print(DEFAULT_URI .. ", but can be overridden.")

    if err then
        print("")
        print("Error: " .. err)
    end
    os.exit(1)
end

function get_password(prompt)
    if prompt then
        io.write(prompt)
    end
    local stty_ret = os.execute("stty -echo 2>/dev/null")
    if stty_ret then
        io.write("\027[8m")
    end
    local ok, pass = pcall(io.read, "*l")
    if stty_ret then
        io.write("\027[m")
    else
        os.execute("stty echo")
    end
    io.write("\n");
    if ok then
        return pass
    end
end

if #arg < 1 then
    usage("no jid specified")
end
jid = arg[1]
password = os.getenv("XMPP_PASSWORD") or get_password("Password: ")
tojid = #arg > 1 and arg[2] or jid
datauri = #arg > 2 and arg[3] or DEFAULT_URI

sluift.debug = 1
sluift.with(sluift.new_client(jid, password), function()
    options = { host=os.getenv("XMPP_HOST"), port=os.getenv("XMPP_PORT") }
    connect(options)
    reference = {
        ['_type'] = 'dom',
        ['tag'] = 'reference',
        ['ns'] = 'urn:xmpp:reference:0',
        ['attributes'] = {
            { ['name'] = 'type', ['value'] = 'data' },
            { ['name'] = 'uri', ['value'] = datauri },
            { ['name'] = 'mimeType', ['value'] = arg[4] },
            { ['name'] = 'begin', ['value'] = arg[5] },
            { ['name'] = 'end', ['value'] = arg[6] }
        },
    }
    send_message{to=tojid, body="Check out this data!", payloads={reference}}
    disconnect()
end)
