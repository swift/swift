SWIFTEN_SOURCES += \
	Swiften/Serializer/ElementSerializer.cpp \
	Swiften/Serializer/CompressRequestSerializer.cpp \
	Swiften/Serializer/AuthRequestSerializer.cpp \
	Swiften/Serializer/StreamFeaturesSerializer.cpp \
	Swiften/Serializer/XMPPSerializer.cpp \
	Swiften/Serializer/StanzaSerializer.cpp \
	Swiften/Serializer/PresenceSerializer.cpp \
	Swiften/Serializer/MessageSerializer.cpp \
	Swiften/Serializer/PayloadSerializer.cpp \
	Swiften/Serializer/PayloadSerializerCollection.cpp

include Swiften/Serializer/UnitTest/Makefile.inc
include Swiften/Serializer/XML/Makefile.inc
include Swiften/Serializer/PayloadSerializers/Makefile.inc
