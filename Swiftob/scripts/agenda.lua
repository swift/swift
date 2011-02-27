agendas = {}
currents = {}

function full_agenda(from)
   fullagenda = {}
   fullagenda[1] = "Roll call"
   fullagenda[2] = "Agenda bashing"
   for i, v in ipairs(agendas[from]) do
      table.insert(fullagenda, v)
   end
   table.insert(fullagenda, "Date of next meeting")
   table.insert(fullagenda, "Any other business")
   return fullagenda
end

function agenda_full_command(command, params, message)
   from = message['frombare']
   ensure_loaded(from)
   agenda = agendas[from]
   fullagenda = full_agenda(from)
   reply = ""
   for i, v in ipairs(fullagenda) do
      reply = reply..i..") "..v.."\n"
   end
   reply = reply.."Fini"
   swiftob_reply_to(message, reply)
end

function agenda_append_command(command, params, message)
   from = message['frombare']
   agenda_append(from, params)
   agenda_save(from)
   swiftob_reply_to(message, "Done.")
end

function agenda_up_command(command, params, message)
   from = message['frombare']
   ensure_loaded(from)
   up = tonumber(params)
   if up == nil then up = 1 end
   currents[from] = currents[from] + up
   if currents[from] <= 0 then currents[from] = 1 end
   item = full_agenda(from)[currents[from]]
   if item == nil then item = "Fini." end
   reply = currents[from]..") "..item
   swiftob_reply_to(message, reply)
end


function agenda_clear_command(command, params, message)
   from = message['frombare']
   agendas[from] = {}
   agenda_save(from)
   swiftob_reply_to(message, "Done.")
end

function agenda_save(from)
   agenda = agendas[from]
   swiftob_store_setting("count@@@"..from, #agenda)
   for i, v in ipairs(agenda) do
      swiftob_store_setting(i.."@@@"..from, v)
   end
end

function ensure_loaded(from)
   if agendas[from] == nil then
      agenda_load(from)
   end
end

function agenda_load(from)
   agendas[from] = {}
   currents[from] = 0
   num_items = tonumber(swiftob_get_setting("count@@@"..from))
   if num_items == nil then num_items = 0 end
   for i = 1, num_items do
      agenda_append(from, swiftob_get_setting(i.."@@@"..from))
   end
end

function agenda_append(from, item)
   ensure_loaded(from)
   agenda = agendas[from]
   table.insert(agenda, item)
   agendas[from] = agenda
end

swiftob_register_command("agenda", "Anyone", "print the full agenda", agenda_full_command)
swiftob_register_command("agendaappend", "Owner", "append an item to the agenda", agenda_append_command)
swiftob_register_command("agendaclear", "Owner", "clear the agenda", agenda_clear_command)
swiftob_register_command("agendaup", "Owner", "Moves the current counter by n, and returns the current agenda item", agenda_up_command)



