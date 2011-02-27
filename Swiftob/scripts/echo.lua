function echo_message(command, params, message)
   swiftob_reply_to(message, params)
end

swiftob_register_command("echo", "Anyone", "What did you say?", echo_message)