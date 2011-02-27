function friendly_version(version) 
   result = version['name']
   if version['version'] ~= nil and version['version'] ~= "" then
      result = result.." version "..version['version']
   end
   if version['os'] ~= nil and version['os'] ~= "" then
      result = result .." on "..version['os']
   end
   return result
end

function version_command(command, params, message)
   jid = swiftob_muc_input_to_jid(params, message['from'])
   if jid == nil then

   else 
      swiftob_get_software_version({
	to=jid, 
	timeout=10, 
	success_callback=function(result)
			    swiftob_reply_to(message, params.." is running "..friendly_version(result))
			 end,
	failure_callback=function(error)
			    swiftob_reply_to(message, "Error getting version from "..params..": "..error)
			end,
	timeout_callback=function()
			    swiftob_reply_to(message, "Timeout waiting for version from "..params)
			 end})
   end
end

swiftob_register_command("version", "Anyone", "Ask for someone's version", version_command)