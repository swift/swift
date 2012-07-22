function irssi_log_to_file(muc, line)
	filename = muc:gsub("@", "_at_")
	filename = filename:gsub("%.%.", "_")
	filename = filename:gsub("/", "_")
	filename = filename:gsub("\\", "_")
	file = io.open(filename, "a+")
	if file == nil then
		io.close()
	else
		file:write(line)
		file:write("\n")
		file:close()
	end
	
end

function irssi_log_message(body, muc, nick, message)
	time = os.date("%H:%M")
	irssi_log_to_file(muc, time.." <"..nick.."> "..body)
end

swiftob_register_listener(irssi_log_message)

