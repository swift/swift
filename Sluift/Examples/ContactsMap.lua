--[[
	Copyright (c) 2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[

	Contacts map

	Creates an HTML file of a map with all your contacts on it.

	The following environment variables are used:
	- SLUIFT_JID, SWIFT_PASS: JID and password to log in with

--]]

require "sluift"

output_dir = arg[1]
if not output_dir then
	error("Please specify the directory to write the map to")
end

-- Collect all data
geolocs = {}
avatars = {}
c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:connect(function () 
	-- Indicate we're interested in getting user location information, and send initial presence
	c:set_caps_node("http://swift.im/ContactsMap")
	c:set_disco_info({identities = {{name = 'ContactsMap'}}, features = {
		sluift.disco.features.DISCO_INFO,
		sluift.disco.features.USER_LOCATION .. '+notify',
	}})
	c:send_presence()

	-- Collect geoloc info
	for event in c:pubsub_events {timeout = 10000} do
		local from = sluift.jid.to_bare(event.from)
		if event._type == 'pubsub_event_items' and event.item then
			if event.node == sluift.disco.features.USER_LOCATION then
				local lat, lon = event.item.latitude, event.item.longitude
				if lat and lon then geolocs[from] = {lat = lat, lon = lon} end
			end
		end
	end

	-- Download the necessary avatars
	for contact in pairs(geolocs) do
		local vcard = c:get_vcard {to = contact}
		if vcard and vcard.photo then
			local avatar_hash = sluift.hexify(sluift.sha1(vcard.photo))
			local file = io.open(output_dir.."/" .. avatar_hash .. ".png", "wb")
			file:write(vcard.photo)
			file:close()
			avatars[contact] = avatar_hash
		end
	end
end)

-- Generate html
min_lat, max_lat = 90, -90
min_lon, max_lon = 180, -180
contacts_html = {}
for contact, geoloc in pairs(geolocs) do
	if geoloc.lat < min_lat then min_lat = geoloc.lat end
	if geoloc.lon < min_lon then min_lon = geoloc.lon end
	if geoloc.lat > max_lat then max_lat = geoloc.lat end
	if geoloc.lon > max_lon then max_lon = geoloc.lon end
	local image = 'null'
	if avatars[contact] then
		image = "'" .. avatars[contact] .. ".png'"
	end
	contacts_html[#contacts_html+1] = "['" .. contact .. "'," .. geoloc.lat .. "," .. geoloc.lon .. "," .. image .. "]"
end
center_html = ((min_lat + max_lat) / 2) .. ',' .. ((min_lon + max_lon) / 2)

map_html = [[
<html>
	<head>
		<title>Contacts Map</title>
		<script src="http://maps.google.com/maps/api/js?sensor=false" type="text/javascript"></script>
	</head>
	<body>
		<div id="map" style="height: 100%; width: 100%;"/>
		<script>
			var map = new google.maps.Map(document.getElementById('map'), {
				zoom: 2,
				center: new google.maps.LatLng(%(CENTER)),
				mapTypeId: google.maps.MapTypeId.ROADMAP
			});
			var infowindow = new google.maps.InfoWindow();
			var contacts = [%(CONTACTS)];
			for (var i = 0; i < contacts.length; i++) { 
				var icon = null;
				if (contacts[i][3]) {
					icon = { url: contacts[i][3], scaledSize: { width: 30, height: 30} };
				}
				var marker = new google.maps.Marker({
					position: new google.maps.LatLng(contacts[i][1], contacts[i][2]),
					map: map,
					icon: icon,
				});
				google.maps.event.addListener(marker, 'click', (function(marker, i) {
					return function() {
						infowindow.setContent(contacts[i][0]);
						infowindow.open(map, marker);
					}
				})(marker, i));
			}
		</script>
	</body>
</html>
]]
local file = io.open(output_dir .. "/index.html", "w")
file:write(map_html:
	gsub('%%%(CONTACTS%)', table.concat(contacts_html, ",")):
	gsub('%%%(CENTER%)', center_html))
file:close()
