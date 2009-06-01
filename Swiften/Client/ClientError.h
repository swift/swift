#ifndef SWIFTEN_ClientError_H
#define SWIFTEN_ClientError_H

namespace Swift {
	class ClientError {
		public:
			enum Type {
				NoError,
				DomainNameResolveError,
				ConnectionError,
				ConnectionReadError,
				XMLError,
				AuthenticationFailedError,
				NoSupportedAuthMechanismsError,
				UnexpectedElementError,
				ResourceBindError,
				SessionStartError,
				TLSError,
				ClientCertificateLoadError,
				ClientCertificateError
			};

			ClientError(Type type = NoError) : type_(type) {}

			Type getType() const { return type_; }

		private:
			Type type_;
	};
}

#endif
