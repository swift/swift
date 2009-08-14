INCLUDEPATH += $$PWD/../../$(PWD)
INCLUDEPATH += $$PWD/../../$(PWD)
DUMMY {
DUMMY {
!DUMMY {
} else {
}
}
}
## Begin File: 3rdParty/Boost/Makefile.inc
DEFINES += BOOST_ALL_NO_LIB
INCLUDEPATH += $$PWD/../../3rdParty/Boost
DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals
SOURCES += $$PWD/../../3rdParty/Boost/libs/date_time/src/gregorian/date_generators.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/date_time/src/gregorian/greg_month.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/date_time/src/gregorian/greg_weekday.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/date_time/src/gregorian/gregorian_types.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/date_time/src/posix_time/posix_time_types.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/system/src/error_code.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/tss_null.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/signals/src/connection.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/signals/src/named_slot_map.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/signals/src/signal_base.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/signals/src/slot.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/signals/src/trackable.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/filesystem/src/operations.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/filesystem/src/path.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/filesystem/src/portability.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/filesystem/src/utf8_codecvt_facet.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/c_regex_traits.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/cpp_regex_traits.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/cregex.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/fileiter.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/icu.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/instances.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/posix_api.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/regex.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/regex_debug.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/regex_raw_buffer.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/regex_traits_defaults.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/static_mutex.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/w32_regex_traits.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/wc_regex_traits.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/wide_posix_api.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/winstances.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/regex/src/usinstances.cpp
win32 {
DEFINES += _WIN32_WINNT=0x0501
DEFINES += __USE_W32_SOCKETS
SOURCES += $$PWD/../../3rdParty/Boost/win32_stubs.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/win32/exceptions.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/win32/thread.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/win32/tss_dll.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/win32/tss_pe.cpp
} else {
LIBS += -lpthread
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/pthread/exceptions.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/pthread/once.cpp
SOURCES += $$PWD/../../3rdParty/Boost/libs/thread/src/pthread/thread.cpp
}
## End file
## Begin File: 3rdParty/CppUnit/Makefile.inc
INCLUDEPATH += $$PWD/../../3rdParty/CppUnit
## End file
## Begin File: 3rdParty/LibIDN/Makefile.inc
INCLUDEPATH += $$PWD/../../3rdParty/LibIDN/src
INCLUDEPATH += $$PWD/../../3rdParty/LibIDN/src
INCLUDEPATH += $$PWD/../../3rdParty/LibIDN/stubs
win32 {
DEFINES += IDNA_STATIC
INCLUDEPATH += $$PWD/../../3rdParty/LibIDN/stubs/win32
DEFINES += strcasecmp=stricmp
DEFINES += strncasecmp=strnicmp
INCLUDEPATH += $$PWD/../../3rdParty/LibIDN/stubs/win32
}
SOURCES += $$PWD/../../3rdParty/LibIDN/src/stringprep.c
SOURCES += $$PWD/../../3rdParty/LibIDN/src/profiles.c
SOURCES += $$PWD/../../3rdParty/LibIDN/src/rfc3454.c
SOURCES += $$PWD/../../3rdParty/LibIDN/src/punycode.c
SOURCES += $$PWD/../../3rdParty/LibIDN/src/idna.c
SOURCES += $$PWD/../../3rdParty/LibIDN/src/toutf8.c
SOURCES += $$PWD/../../3rdParty/LibIDN/src/nfkc.c
## End file
## Begin File: 3rdParty/ZLib/Makefile.inc
INCLUDEPATH += $$PWD/../../3rdParty/ZLib/src
INCLUDEPATH += $$PWD/../../3rdParty/ZLib/src
SOURCES += $$PWD/../../3rdParty/ZLib/src/adler32.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/compress.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/crc32.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/deflate.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/gzio.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/infback.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/inffast.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/inflate.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/inftrees.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/trees.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/uncompr.c
SOURCES += $$PWD/../../3rdParty/ZLib/src/zutil.c
## End file
## Begin File: 3rdParty/Expat/Makefile.inc
SOURCES += $$PWD/../../3rdParty/Expat/src/xmltok.c
SOURCES += $$PWD/../../3rdParty/Expat/src/xmlparse.c
SOURCES += $$PWD/../../3rdParty/Expat/src/xmlrole.c
!isEmpty(USE_BUNDLED_EXPAT) {
INCLUDEPATH += $$PWD/../../3rdParty/Expat
INCLUDEPATH += $$PWD/../../3rdParty/Expat/src
DEFINES += HAVE_EXPAT_CONFIG_H
DEFINES += XML_STATIC
}
## End file
## Begin File: 3rdParty/SQLite/Makefile.inc
INCLUDEPATH += $$PWD/../../3rdParty/SQLite
SOURCES += $$PWD/../../3rdParty/SQLite/sqlite3.c
## End file
## Begin File: Swiften/Makefile.inc
## Begin File: Swiften/Base/Makefile.inc
SOURCES += $$PWD/../../Swiften/Base/String.cpp
SOURCES += $$PWD/../../Swiften/Base/ByteArray.cpp
SOURCES += $$PWD/../../Swiften/Base/IDGenerator.cpp
SOURCES += $$PWD/../../Swiften/Base/sleep.cpp
## Begin File: Swiften/Base/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Application/Makefile.inc
SOURCES += $$PWD/../../Swiften/Application/Application.cpp
SOURCES += $$PWD/../../Swiften/Application/ApplicationMessageDisplay.cpp
mac {
## Begin File: Swiften/Application/MacOSX/Makefile.inc
SOURCES += $$PWD/../../Swiften/Application/MacOSX/MacOSXApplication.cpp
## End file
}
## Begin File: Swiften/Application/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/EventLoop/Makefile.inc
SOURCES += $$PWD/../../Swiften/EventLoop/EventOwner.cpp
SOURCES += $$PWD/../../Swiften/EventLoop/EventLoop.cpp
SOURCES += $$PWD/../../Swiften/EventLoop/SimpleEventLoop.cpp
SOURCES += $$PWD/../../Swiften/EventLoop/MainEventLoop.cpp
mac {
## Begin File: Swiften/EventLoop/Cocoa/Makefile.inc
## End file
}
## Begin File: Swiften/EventLoop/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/StringCodecs/Makefile.inc
SOURCES += $$PWD/../../Swiften/StringCodecs/Base64.cpp
SOURCES += $$PWD/../../Swiften/StringCodecs/SHA1.cpp
## Begin File: Swiften/StringCodecs/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/JID/Makefile.inc
SOURCES += $$PWD/../../Swiften/JID/JID.cpp
## Begin File: Swiften/JID/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Elements/Makefile.inc
SOURCES += $$PWD/../../Swiften/Elements/RosterPayload.cpp
SOURCES += $$PWD/../../Swiften/Elements/Payload.cpp
SOURCES += $$PWD/../../Swiften/Elements/Stanza.cpp
SOURCES += $$PWD/../../Swiften/Elements/Element.cpp
SOURCES += $$PWD/../../Swiften/Elements/DiscoInfo.cpp
SOURCES += $$PWD/../../Swiften/Elements/IQ.cpp
## Begin File: Swiften/Elements/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Events/Makefile.inc
## End file
## Begin File: Swiften/StreamStack/Makefile.inc
SOURCES += $$PWD/../../Swiften/StreamStack/XMPPLayer.cpp
SOURCES += $$PWD/../../Swiften/StreamStack/StreamStack.cpp
SOURCES += $$PWD/../../Swiften/StreamStack/LowLayer.cpp
SOURCES += $$PWD/../../Swiften/StreamStack/HighLayer.cpp
SOURCES += $$PWD/../../Swiften/StreamStack/WhitespacePingLayer.cpp
SOURCES += $$PWD/../../Swiften/StreamStack/TLSLayerFactory.cpp
SOURCES += $$PWD/../../Swiften/StreamStack/PlatformTLSLayerFactory.cpp
!isEmpty(HAVE_OPENSSL) {
SOURCES += $$PWD/../../Swiften/StreamStack/OpenSSLLayer.cpp
}
## Begin File: Swiften/StreamStack/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Serializer/Makefile.inc
SOURCES += $$PWD/../../Swiften/Serializer/ElementSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/CompressRequestSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/AuthRequestSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/StreamFeaturesSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/XMPPSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/StanzaSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PresenceSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/MessageSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializerCollection.cpp
## Begin File: Swiften/Serializer/UnitTest/Makefile.inc
## End file
## Begin File: Swiften/Serializer/XML/Makefile.inc
SOURCES += $$PWD/../../Swiften/Serializer/XML/XMLNode.cpp
SOURCES += $$PWD/../../Swiften/Serializer/XML/XMLElement.cpp
## Begin File: Swiften/Serializer/XML/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Serializer/PayloadSerializers/Makefile.inc
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/SoftwareVersionSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/ErrorSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/RosterSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/DiscoInfoSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/CapsInfoSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/ResourceBindSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/SecurityLabelSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/VCardSerializer.cpp
SOURCES += $$PWD/../../Swiften/Serializer/PayloadSerializers/VCardUpdateSerializer.cpp
## Begin File: Swiften/Serializer/PayloadSerializers/UnitTest/Makefile.inc
## End file
## End file
## End file
## Begin File: Swiften/Parser/Makefile.inc
SOURCES += $$PWD/../../Swiften/Parser/XMLParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/XMLParserClient.cpp
SOURCES += $$PWD/../../Swiften/Parser/XMLParserFactory.cpp
SOURCES += $$PWD/../../Swiften/Parser/PlatformXMLParserFactory.cpp
SOURCES += $$PWD/../../Swiften/Parser/XMPPParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/XMPPParserClient.cpp
SOURCES += $$PWD/../../Swiften/Parser/MessageParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/IQParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PresenceParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/StreamFeaturesParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/CompressParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/AuthRequestParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/StanzaParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/ElementParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParserFactory.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParserFactoryCollection.cpp
SOURCES += $$PWD/../../Swiften/Parser/SerializingParser.cpp
!isEmpty(HAVE_LIBXML) {
SOURCES += $$PWD/../../Swiften/Parser/LibXMLParser.cpp
}
!isEmpty(HAVE_EXPAT) {
SOURCES += $$PWD/../../Swiften/Parser/ExpatParser.cpp
}
## Begin File: Swiften/Parser/PayloadParsers/Makefile.inc
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/BodyParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/PriorityParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/StatusParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/StatusShowParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/SoftwareVersionParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/SecurityLabelParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/DiscoInfoParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/ErrorParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/RosterParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/ResourceBindParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/VCardUpdateParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/VCardParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/RawXMLPayloadParser.cpp
SOURCES += $$PWD/../../Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.cpp
## Begin File: Swiften/Parser/PayloadParsers/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Parser/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/MUC/Makefile.inc
SOURCES += $$PWD/../../Swiften/MUC/MUCRegistry.cpp
SOURCES += $$PWD/../../Swiften/MUC/MUC.cpp
SOURCES += $$PWD/../../Swiften/MUC/MUCOccupant.cpp
## End file
## Begin File: Swiften/Network/Makefile.inc
SOURCES += $$PWD/../../Swiften/Network/HostAddress.cpp
SOURCES += $$PWD/../../Swiften/Network/DomainNameResolver.cpp
SOURCES += $$PWD/../../Swiften/Network/ConnectionFactory.cpp
SOURCES += $$PWD/../../Swiften/Network/BoostConnection.cpp
SOURCES += $$PWD/../../Swiften/Network/BoostConnectionFactory.cpp
SOURCES += $$PWD/../../Swiften/Network/BoostConnectionServer.cpp
SOURCES += $$PWD/../../Swiften/Network/ConnectionServer.cpp
SOURCES += $$PWD/../../Swiften/Network/BoostIOServiceThread.cpp
SOURCES += $$PWD/../../Swiften/Network/Timer.cpp
## Begin File: Swiften/Network/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Session/Makefile.inc
SOURCES += $$PWD/../../Swiften/Session/Session.cpp
## End file
## Begin File: Swiften/Client/Makefile.inc
SOURCES += $$PWD/../../Swiften/Client/Client.cpp
SOURCES += $$PWD/../../Swiften/Client/ClientSession.cpp
## Begin File: Swiften/Client/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/TLS/Makefile.inc
!isEmpty(HAVE_OPENSSL) {
## Begin File: Swiften/TLS/OpenSSL/Makefile.inc
SOURCES += $$PWD/../../Swiften/TLS/OpenSSL/OpenSSLContext.cpp
## End file
}
## End file
## Begin File: Swiften/SASL/Makefile.inc
SOURCES += $$PWD/../../Swiften/SASL/PLAINMessage.cpp
## Begin File: Swiften/SASL/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Compress/Makefile.inc
SOURCES += $$PWD/../../Swiften/Compress/ZLibCodecompressor.cpp
## Begin File: Swiften/Compress/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Queries/Makefile.inc
SOURCES += $$PWD/../../Swiften/Queries/IQRouter.cpp
SOURCES += $$PWD/../../Swiften/Queries/IQHandler.cpp
SOURCES += $$PWD/../../Swiften/Queries/IQChannel.cpp
SOURCES += $$PWD/../../Swiften/Queries/Request.cpp
## Begin File: Swiften/Queries/Responders/Makefile.inc
SOURCES += $$PWD/../../Swiften/Queries/Responders/SoftwareVersionResponder.cpp
SOURCES += $$PWD/../../Swiften/Queries/Responders/DiscoInfoResponder.cpp
## Begin File: Swiften/Queries/Responders/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Queries/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Roster/Makefile.inc
SOURCES += $$PWD/../../Swiften/Roster/ContactRosterItem.cpp
SOURCES += $$PWD/../../Swiften/Roster/Roster.cpp
SOURCES += $$PWD/../../Swiften/Roster/XMPPRoster.cpp
## Begin File: Swiften/Roster/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Disco/Makefile.inc
SOURCES += $$PWD/../../Swiften/Disco/CapsInfoGenerator.cpp
## Begin File: Swiften/Disco/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Presence/Makefile.inc
SOURCES += $$PWD/../../Swiften/Presence/PresenceOracle.cpp
## Begin File: Swiften/Presence/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Notifier/Makefile.inc
SOURCES += $$PWD/../../Swiften/Notifier/Notifier.cpp
!isEmpty(HAVE_GROWL) {
}
## End file
## Begin File: Swiften/History/Makefile.inc
SOURCES += $$PWD/../../Swiften/History/HistoryManager.cpp
SOURCES += $$PWD/../../Swiften/History/SQLiteHistoryManager.cpp
## Begin File: Swiften/History/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Avatars/Makefile.inc
SOURCES += $$PWD/../../Swiften/Avatars/AvatarManager.cpp
SOURCES += $$PWD/../../Swiften/Avatars/AvatarStorage.cpp
SOURCES += $$PWD/../../Swiften/Avatars/AvatarFileStorage.cpp
## Begin File: Swiften/Avatars/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Server/Makefile.inc
SOURCES += $$PWD/../../Swiften/Server/ServerSession.cpp
SOURCES += $$PWD/../../Swiften/Server/ServerFromClientSession.cpp
SOURCES += $$PWD/../../Swiften/Server/ServerStanzaRouter.cpp
SOURCES += $$PWD/../../Swiften/Server/UserRegistry.cpp
SOURCES += $$PWD/../../Swiften/Server/SimpleUserRegistry.cpp
## Begin File: Swiften/Server/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/LinkLocal/Makefile.inc
SOURCES += $$PWD/../../Swiften/LinkLocal/LinkLocalServiceBrowser.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/LinkLocalService.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/LinkLocalServiceInfo.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/IncomingLinkLocalSession.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/OutgoingLinkLocalSession.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/LinkLocalConnector.cpp
## Begin File: Swiften/LinkLocal/DNSSD/Makefile.inc
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/DNSSDServiceID.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/DNSSDQuerier.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/DNSSDBrowseQuery.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/DNSSDRegisterQuery.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/DNSSDResolveServiceQuery.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/DNSSDResolveHostnameQuery.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/PlatformDNSSDQuerierFactory.cpp
!isEmpty(HAVE_BONJOUR) {
## Begin File: Swiften/LinkLocal/DNSSD/Bonjour/Makefile.inc
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuery.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/Bonjour/BonjourQuerier.cpp
## End file
}
!isEmpty(HAVE_AVAHI) {
## Begin File: Swiften/LinkLocal/DNSSD/Avahi/Makefile.inc
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/Avahi/AvahiQuerier.cpp
## End file
}
## Begin File: Swiften/LinkLocal/DNSSD/Fake/Makefile.inc
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuery.cpp
SOURCES += $$PWD/../../Swiften/LinkLocal/DNSSD/Fake/FakeDNSSDQuerier.cpp
## End file
## End file
## Begin File: Swiften/LinkLocal/UnitTest/Makefile.inc
## End file
## End file
CPPFLAGS += $(SQLITE_CPPFLAGS)
## Begin File: Swiften/QA/Makefile.inc
## Begin File: Swiften/QA/NetworkTest/Makefile.inc
## End file
## Begin File: Swiften/QA/ClientTest/Makefile.inc
## End file
## End file
## Begin File: Swiften/Examples/Makefile.inc
## Begin File: Swiften/Examples/EchoBot/Makefile.inc
## End file
## End file
## End file
true {
## Begin File: Swift/Makefile.inc
## Begin File: Swift/Controllers/Makefile.inc
SOURCES += $$PWD/../../Swift/Controllers/ChatController.cpp
SOURCES += $$PWD/../../Swift/Controllers/ChatControllerBase.cpp
SOURCES += $$PWD/../../Swift/Controllers/MainController.cpp
SOURCES += $$PWD/../../Swift/Controllers/NickResolver.cpp
SOURCES += $$PWD/../../Swift/Controllers/RosterController.cpp
SOURCES += $$PWD/../../Swift/Controllers/XMPPRosterController.cpp
SOURCES += $$PWD/../../Swift/Controllers/MUCController.cpp
SOURCES += $$PWD/../../Swift/Controllers/EventController.cpp
SOURCES += $$PWD/../../Swift/Controllers/SoundEventController.cpp
SOURCES += $$PWD/../../Swift/Controllers/SystemTrayController.cpp
## Begin File: Swift/Controllers/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Swift/QtUI/Makefile.inc
win32 {
} else {
}
## End file
## Begin File: Swift/Packaging/Debian/Makefile.inc
## End file
## End file
}
## Begin File: QA/Makefile.inc
DUMMY {
}
## Begin File: QA/UnitTest/Makefile.inc
## End file
## End file
DUMMY {
DUMMY {
}
}
## End file
