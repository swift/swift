#include "Swiften/Parser/PayloadParsers/FullPayloadParserFactoryCollection.h"
#include "Swiften/Base/foreach.h"
#include "Swiften/Parser/GenericPayloadParser.h"
#include "Swiften/Parser/PayloadParserFactory.h"
#include "Swiften/Parser/PayloadParsers/ErrorParserFactory.h"
#include "Swiften/Parser/PayloadParsers/BodyParserFactory.h"
#include "Swiften/Parser/PayloadParsers/PriorityParserFactory.h"
#include "Swiften/Parser/PayloadParsers/ResourceBindParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StartSessionParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StatusParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StatusShowParserFactory.h"
#include "Swiften/Parser/PayloadParsers/RosterParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SoftwareVersionParserFactory.h"
#include "Swiften/Parser/PayloadParsers/StorageParserFactory.h"
#include "Swiften/Parser/PayloadParsers/DiscoInfoParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SecurityLabelParserFactory.h"
#include "Swiften/Parser/PayloadParsers/SecurityLabelsCatalogParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardUpdateParserFactory.h"
#include "Swiften/Parser/PayloadParsers/VCardParserFactory.h"
#include "Swiften/Parser/PayloadParsers/RawXMLPayloadParserFactory.h"

using namespace boost;

namespace Swift {

FullPayloadParserFactoryCollection::FullPayloadParserFactoryCollection() {
	factories_.push_back(shared_ptr<PayloadParserFactory>(new StatusParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new StatusShowParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new BodyParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new PriorityParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new ErrorParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new SoftwareVersionParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new StorageParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new RosterParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new DiscoInfoParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new ResourceBindParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new StartSessionParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new SecurityLabelParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new SecurityLabelsCatalogParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new VCardUpdateParserFactory()));
	factories_.push_back(shared_ptr<PayloadParserFactory>(new VCardParserFactory()));
	foreach(shared_ptr<PayloadParserFactory> factory, factories_) {
		addFactory(factory.get());
	}
	defaultFactory_ = new RawXMLPayloadParserFactory();
	setDefaultFactory(defaultFactory_);
}

FullPayloadParserFactoryCollection::~FullPayloadParserFactoryCollection() {
	setDefaultFactory(NULL);
	delete defaultFactory_;
	foreach(shared_ptr<PayloadParserFactory> factory, factories_) {
		removeFactory(factory.get());
	}
}

}
