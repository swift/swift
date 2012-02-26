/*
 * Copyright (c) 2012 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QGridLayout>
#include <QObject>
#include <QString>
#include <typeinfo>

#define GENERIC_QT_VCARD_FIELD_INFO(MENU_NAME, ALLOWED_INSTANCES, FIELD_CLASS) \
	class FieldInfo : public QtVCardFieldInfo { \
		public: \
			virtual ~FieldInfo() { \
			} \
	\
			virtual QString getMenuName() const { \
				return QObject::tr(MENU_NAME); \
			} \
	\
			virtual int getAllowedInstances() const { \
				return ALLOWED_INSTANCES; \
			} \
	\
			virtual QWidget* createFieldInstance(QWidget* parent, QGridLayout* layout, bool editable) const { \
				return new FIELD_CLASS(parent, layout, editable); \
			} \
	\
			virtual bool testInstance(QWidget* widget) const { \
				return dynamic_cast<FIELD_CLASS*>(widget) != 0; \
			} \
	};

class QWidget;

namespace Swift {

	class QtVCardFieldInfo {
		public:
			static const int UNLIMITED_INSTANCES = -1;

			virtual ~QtVCardFieldInfo() {
			}
			virtual QString getMenuName() const = 0;
			virtual int getAllowedInstances() const = 0;
			virtual QWidget* createFieldInstance(QWidget* parent, QGridLayout* layout, bool editable) const = 0;
			virtual bool testInstance(QWidget*) const = 0;
	};
}
