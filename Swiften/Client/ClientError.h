#pragma once

namespace Swift {
	class ClientError {
		public:
			enum Type {
				UnknownError,
				DomainNameResolveError,
				ConnectionError,
				ConnectionReadError,
				ConnectionWriteError,
				XMLError,
				AuthenticationFailedError,
				ServerVerificationFailedError,
				NoSupportedAuthMechanismsError,
				UnexpectedElementError,
				ResourceBindError,
				SessionStartError,
				TLSError,
				ClientCertificateLoadError,
				ClientCertificateError
			};

			ClientError(Type type = UnknownError) : type_(type) {}

			Type getType() const { return type_; }

		private:
			Type type_;
	};
}
