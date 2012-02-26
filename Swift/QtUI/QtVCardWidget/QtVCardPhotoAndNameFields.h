/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QMenu>
#include <QSignalMapper>
#include <QWidget>
#include <Swiften/Base/ByteArray.h>

namespace Ui {
	class QtVCardPhotoAndNameFields;
}


namespace Swift {

	class QtVCardPhotoAndNameFields : public QWidget {
		Q_OBJECT
		Q_PROPERTY(bool editable READ isEditable WRITE setEditable)

		public:
			explicit QtVCardPhotoAndNameFields(QWidget* parent = 0);
			~QtVCardPhotoAndNameFields();

			bool isEditable() const;
			void setEditable(bool);

			QMenu* getAddFieldMenu() const;

			void setAvatar(const ByteArray& data, const std::string& type);
			ByteArray getAvatarData() const;
			std::string getAvatarType() const;

			void setFormattedName(const QString formattedName);
			QString getFormattedName() const;

			void setNickname(const QString nickname);
			QString getNickname() const;

			void setPrefix(const QString prefix);
			QString getPrefix() const;

			void setGivenName(const QString givenName);
			QString getGivenName() const;

			void setMiddleName(const QString middleName);
			QString getMiddleName() const;

			void setFamilyName(const QString familyName);
			QString getFamilyName() const;

			void setSuffix(const QString suffix);
			QString getSuffix() const;

		public slots:
			void showField(const QString& widgetName);

		private:
			void prepareAddFieldMenu();

		private:
			Ui::QtVCardPhotoAndNameFields* ui;
			bool editable;

			QMenu* addFieldMenu;
			QSignalMapper* actionSignalMapper;
	};

}
