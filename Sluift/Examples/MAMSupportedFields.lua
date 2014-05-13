-- Retrieving form fields
-- Usage: ./sluift MAMSupportedFields.lua <jid> <password>

sluift.debug = true

c = sluift.new_client(arg[1], arg[2])

c:connect();

mam_result = c:get_mam{}

for i=1,#mam_result.form.fields do
	if mam_result.form.fields[i].type ~= "hidden" then
		print("Server supports: " .. mam_result.form.fields[i].name)
	end
end

c:disconnect()
