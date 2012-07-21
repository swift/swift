function log_a_message(body, muc, nick, message)
   print("Received line from '" .. nick .. "' in '" .. muc .. "':")
   print(body)
end

swiftob_register_listener(log_a_message)
