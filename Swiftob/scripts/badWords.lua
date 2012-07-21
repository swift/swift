function bad_words(body, muc, nick, message)
  words = {"sbwriel"}
  print("Received line from '" .. nick .. "' in '" .. muc .. "':")
  print(body)

  for _, word in pairs(words) do
    if string.len(string.match(body, word)) > 0 then
      --swiftob_reply_to(message, "Kicking "..nick.." for bad word "..word)
      swiftob_muc_kick(muc, nick)
    end
  end
end

swiftob_register_listener(bad_words)

