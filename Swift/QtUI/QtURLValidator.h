/*
 * Copyright (c) 2012 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <QValidator>

namespace Swift {
	class QtURLValidator : public QValidator {
		Q_OBJECT
		public:
			QtURLValidator(QObject* parent);
			virtual QValidator::State validate(QString& input, int& pos) const;
	};
}

