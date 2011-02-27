
function eval_command(command, params, message)
   assert(loadstring(params))()
   swiftob_reply_to(message, "done")
end

function evalr_command(command, params, message)
   result = assert(loadstring(params))()
   swiftob_reply_to(message, "" .. result)
end

swiftob_register_command("eval", "Owner", "Evaluate an expression", eval_command)
swiftob_register_command("evalr", "Owner", "Evaluate an expression and return the result", evalr_command)

