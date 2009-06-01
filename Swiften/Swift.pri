INCLUDEPATH += $$PWD/..
!DUMMY {
} else {
}
DUMMY {
DUMMY {
!DUMMY {
} else {
}
}
}
## Begin File: 3rdParty/Boost/Makefile.inc
DEFINES += BOOST_ALL_NO_LIB
INCLUDEPATH += $$PWD/3rdParty/Boost
DEFINES += BOOST_SIGNALS_NAMESPACE=bsignals
SOURCES += $$PWD/3rdParty/Boost/libs/date_time/src/gregorian/date_generators.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/date_time/src/gregorian/greg_month.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/date_time/src/gregorian/greg_weekday.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/date_time/src/gregorian/gregorian_types.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/date_time/src/posix_time/posix_time_types.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/system/src/error_code.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/tss_null.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/signals/src/connection.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/signals/src/named_slot_map.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/signals/src/signal_base.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/signals/src/slot.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/signals/src/trackable.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/filesystem/src/operations.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/filesystem/src/path.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/filesystem/src/portability.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/filesystem/src/utf8_codecvt_facet.cpp
win32 {
DEFINES += _WIN32_WINNT=0x0501
DEFINES += __USE_W32_SOCKETS
SOURCES += $$PWD/3rdParty/Boost/win32_stubs.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/win32/exceptions.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/win32/thread.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/win32/tss_dll.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/win32/tss_pe.cpp
} else {
LIBS += -lpthread
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/pthread/exceptions.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/pthread/once.cpp
SOURCES += $$PWD/3rdParty/Boost/libs/thread/src/pthread/thread.cpp
}
## End file
## Begin File: 3rdParty/CppUnit/Makefile.inc
INCLUDEPATH += $$PWD/3rdParty/CppUnit
## End file
## Begin File: 3rdParty/LibIDN/Makefile.inc
INCLUDEPATH += $$PWD/3rdParty/LibIDN/src
INCLUDEPATH += $$PWD/3rdParty/LibIDN/src
INCLUDEPATH += $$PWD/3rdParty/LibIDN/stubs
win32 {
DEFINES += IDNA_STATIC
INCLUDEPATH += $$PWD/3rdParty/LibIDN/stubs/win32
DEFINES += strcasecmp=stricmp
DEFINES += strncasecmp=strnicmp
INCLUDEPATH += $$PWD/3rdParty/LibIDN/stubs/win32
}
SOURCES += $$PWD/3rdParty/LibIDN/src/stringprep.c
SOURCES += $$PWD/3rdParty/LibIDN/src/profiles.c
SOURCES += $$PWD/3rdParty/LibIDN/src/rfc3454.c
SOURCES += $$PWD/3rdParty/LibIDN/src/punycode.c
SOURCES += $$PWD/3rdParty/LibIDN/src/idna.c
SOURCES += $$PWD/3rdParty/LibIDN/src/toutf8.c
SOURCES += $$PWD/3rdParty/LibIDN/src/nfkc.c
## End file
## Begin File: 3rdParty/ZLib/Makefile.inc
INCLUDEPATH += $$PWD/3rdParty/ZLib/src
INCLUDEPATH += $$PWD/3rdParty/ZLib/src
SOURCES += $$PWD/3rdParty/ZLib/src/adler32.c
SOURCES += $$PWD/3rdParty/ZLib/src/compress.c
SOURCES += $$PWD/3rdParty/ZLib/src/crc32.c
SOURCES += $$PWD/3rdParty/ZLib/src/deflate.c
SOURCES += $$PWD/3rdParty/ZLib/src/gzio.c
SOURCES += $$PWD/3rdParty/ZLib/src/infback.c
SOURCES += $$PWD/3rdParty/ZLib/src/inffast.c
SOURCES += $$PWD/3rdParty/ZLib/src/inflate.c
SOURCES += $$PWD/3rdParty/ZLib/src/inftrees.c
SOURCES += $$PWD/3rdParty/ZLib/src/trees.c
SOURCES += $$PWD/3rdParty/ZLib/src/uncompr.c
SOURCES += $$PWD/3rdParty/ZLib/src/zutil.c
## End file
## Begin File: Base/Makefile.inc
SOURCES += $$PWD/Base/String.cpp
SOURCES += $$PWD/Base/ByteArray.cpp
SOURCES += $$PWD/Base/IDGenerator.cpp
SOURCES += $$PWD/Base/sleep.cpp
## Begin File: Base/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Application/Makefile.inc
SOURCES += $$PWD/Application/Application.cpp
SOURCES += $$PWD/Application/ApplicationMessageDisplay.cpp
mac {
## Begin File: Application/MacOSX/Makefile.inc
SOURCES += $$PWD/Application/MacOSX/MacOSXApplication.cpp
## End file
}
## Begin File: Application/UnitTest/Makefile.inc
## End file
## End file
## Begin File: EventLoop/Makefile.inc
SOURCES += $$PWD/EventLoop/EventLoop.cpp
SOURCES += $$PWD/EventLoop/SimpleEventLoop.cpp
SOURCES += $$PWD/EventLoop/MainEventLoop.cpp
## Begin File: EventLoop/UnitTest/Makefile.inc
## End file
## End file
## Begin File: StringCodecs/Makefile.inc
SOURCES += $$PWD/StringCodecs/Base64.cpp
SOURCES += $$PWD/StringCodecs/SHA1.cpp
## Begin File: StringCodecs/UnitTest/Makefile.inc
## End file
## End file
## Begin File: JID/Makefile.inc
SOURCES += $$PWD/JID/JID.cpp
## Begin File: JID/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Elements/Makefile.inc
SOURCES += $$PWD/Elements/RosterPayload.cpp
SOURCES += $$PWD/Elements/Payload.cpp
SOURCES += $$PWD/Elements/Stanza.cpp
SOURCES += $$PWD/Elements/Element.cpp
SOURCES += $$PWD/Elements/DiscoInfo.cpp
SOURCES += $$PWD/Elements/IQ.cpp
## Begin File: Elements/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Events/Makefile.inc
## End file
## Begin File: StreamStack/Makefile.inc
SOURCES += $$PWD/StreamStack/XMPPLayer.cpp
SOURCES += $$PWD/StreamStack/StreamStack.cpp
SOURCES += $$PWD/StreamStack/LowLayer.cpp
SOURCES += $$PWD/StreamStack/HighLayer.cpp
SOURCES += $$PWD/StreamStack/WhitespacePingLayer.cpp
SOURCES += $$PWD/StreamStack/TLSLayerFactory.cpp
SOURCES += $$PWD/StreamStack/PlatformTLSLayerFactory.cpp
!isEmpty(HAVE_OPENSSL) {
SOURCES += $$PWD/StreamStack/OpenSSLLayer.cpp
}
## Begin File: StreamStack/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Serializer/Makefile.inc
SOURCES += $$PWD/Serializer/ElementSerializer.cpp
SOURCES += $$PWD/Serializer/CompressRequestSerializer.cpp
SOURCES += $$PWD/Serializer/AuthRequestSerializer.cpp
SOURCES += $$PWD/Serializer/StreamFeaturesSerializer.cpp
SOURCES += $$PWD/Serializer/XMPPSerializer.cpp
SOURCES += $$PWD/Serializer/StanzaSerializer.cpp
SOURCES += $$PWD/Serializer/PresenceSerializer.cpp
SOURCES += $$PWD/Serializer/MessageSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializerCollection.cpp
## Begin File: Serializer/UnitTest/Makefile.inc
## End file
## Begin File: Serializer/XML/Makefile.inc
SOURCES += $$PWD/Serializer/XML/XMLNode.cpp
SOURCES += $$PWD/Serializer/XML/XMLElement.cpp
## Begin File: Serializer/XML/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Serializer/PayloadSerializers/Makefile.inc
SOURCES += $$PWD/Serializer/PayloadSerializers/SoftwareVersionSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/FullPayloadSerializerCollection.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/ErrorSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/MUCPayloadSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/RosterSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/DiscoInfoSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/CapsInfoSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/ResourceBindSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/SecurityLabelSerializer.cpp
SOURCES += $$PWD/Serializer/PayloadSerializers/SecurityLabelsCatalogSerializer.cpp
## Begin File: Serializer/PayloadSerializers/UnitTest/Makefile.inc
## End file
## End file
## End file
## Begin File: Parser/Makefile.inc
SOURCES += $$PWD/Parser/XMLParser.cpp
SOURCES += $$PWD/Parser/XMLParserClient.cpp
SOURCES += $$PWD/Parser/XMLParserFactory.cpp
SOURCES += $$PWD/Parser/PlatformXMLParserFactory.cpp
SOURCES += $$PWD/Parser/XMPPParser.cpp
SOURCES += $$PWD/Parser/XMPPParserClient.cpp
SOURCES += $$PWD/Parser/MessageParser.cpp
SOURCES += $$PWD/Parser/IQParser.cpp
SOURCES += $$PWD/Parser/PresenceParser.cpp
SOURCES += $$PWD/Parser/StreamFeaturesParser.cpp
SOURCES += $$PWD/Parser/CompressParser.cpp
SOURCES += $$PWD/Parser/AuthRequestParser.cpp
SOURCES += $$PWD/Parser/StanzaParser.cpp
SOURCES += $$PWD/Parser/ElementParser.cpp
SOURCES += $$PWD/Parser/PayloadParser.cpp
SOURCES += $$PWD/Parser/PayloadParserFactory.cpp
SOURCES += $$PWD/Parser/PayloadParserFactoryCollection.cpp
SOURCES += $$PWD/Parser/SerializingParser.cpp
!isEmpty(HAVE_LIBXML) {
SOURCES += $$PWD/Parser/LibXMLParser.cpp
}
!isEmpty(HAVE_EXPAT) {
SOURCES += $$PWD/Parser/ExpatParser.cpp
}
## Begin File: Parser/PayloadParsers/Makefile.inc
SOURCES += $$PWD/Parser/PayloadParsers/BodyParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/PriorityParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/StatusParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/StatusShowParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/SoftwareVersionParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/SecurityLabelParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/SecurityLabelsCatalogParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/DiscoInfoParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/ErrorParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/RosterParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/ResourceBindParser.cpp
SOURCES += $$PWD/Parser/PayloadParsers/FullPayloadParserFactoryCollection.cpp
## Begin File: Parser/PayloadParsers/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Parser/UnitTest/Makefile.inc
## End file
## End file
## Begin File: MUC/Makefile.inc
SOURCES += $$PWD/MUC/MUC.cpp
SOURCES += $$PWD/MUC/MUCOccupant.cpp
## End file
## Begin File: Network/Makefile.inc
SOURCES += $$PWD/Network/HostAddress.cpp
SOURCES += $$PWD/Network/DomainNameResolver.cpp
SOURCES += $$PWD/Network/ConnectionFactory.cpp
SOURCES += $$PWD/Network/BoostConnection.cpp
SOURCES += $$PWD/Network/BoostConnectionFactory.cpp
SOURCES += $$PWD/Network/Timer.cpp
## Begin File: Network/UnitTest/Makefile.inc
## End file
!win32 {
LIBS += -lresolv
}
## End file
## Begin File: Client/Makefile.inc
SOURCES += $$PWD/Client/Client.cpp
SOURCES += $$PWD/Client/Session.cpp
## Begin File: Client/UnitTest/Makefile.inc
## End file
## End file
## Begin File: TLS/Makefile.inc
!isEmpty(HAVE_OPENSSL) {
## Begin File: TLS/OpenSSL/Makefile.inc
SOURCES += $$PWD/TLS/OpenSSL/OpenSSLContext.cpp
## End file
}
## End file
## Begin File: SASL/Makefile.inc
SOURCES += $$PWD/SASL/PLAINMessage.cpp
## Begin File: SASL/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Compress/Makefile.inc
SOURCES += $$PWD/Compress/ZLibCodecompressor.cpp
## Begin File: Compress/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Queries/Makefile.inc
SOURCES += $$PWD/Queries/IQRouter.cpp
SOURCES += $$PWD/Queries/IQHandler.cpp
SOURCES += $$PWD/Queries/IQChannel.cpp
SOURCES += $$PWD/Queries/Request.cpp
## Begin File: Queries/Responders/Makefile.inc
SOURCES += $$PWD/Queries/Responders/SoftwareVersionResponder.cpp
SOURCES += $$PWD/Queries/Responders/DiscoInfoResponder.cpp
## Begin File: Queries/Responders/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Queries/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Controllers/Makefile.inc
SOURCES += $$PWD/Controllers/ChatController.cpp
SOURCES += $$PWD/Controllers/ChatControllerBase.cpp
SOURCES += $$PWD/Controllers/MainController.cpp
SOURCES += $$PWD/Controllers/NickResolver.cpp
SOURCES += $$PWD/Controllers/RosterController.cpp
SOURCES += $$PWD/Controllers/XMPPRosterController.cpp
SOURCES += $$PWD/Controllers/MUCController.cpp
SOURCES += $$PWD/Controllers/EventController.cpp
## Begin File: Controllers/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Roster/Makefile.inc
SOURCES += $$PWD/Roster/ContactRosterItem.cpp
SOURCES += $$PWD/Roster/Roster.cpp
SOURCES += $$PWD/Roster/XMPPRoster.cpp
## Begin File: Roster/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Disco/Makefile.inc
SOURCES += $$PWD/Disco/CapsInfoGenerator.cpp
## Begin File: Disco/UnitTest/Makefile.inc
## End file
## End file
## Begin File: Examples/Makefile.inc
## Begin File: Examples/TuneBot/Makefile.inc
## End file
## End file
## Begin File: QA/Makefile.inc
DUMMY {
}
## Begin File: QA/UnitTest/Makefile.inc
## End file
## Begin File: QA/NetworkTest/Makefile.inc
## End file
## Begin File: QA/ClientTest/Makefile.inc
## End file
## End file
DUMMY {
DUMMY {
}
}
## End file
