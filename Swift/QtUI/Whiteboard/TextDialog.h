/*
 * Copyright (c) 2012 Mateusz Piękos
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QGraphicsTextItem>
#include <QSpinBox>

#include <iostream>

namespace Swift {
	class TextDialog : public QDialog
	{
		Q_OBJECT
	public:
		TextDialog(QGraphicsTextItem* item, QWidget* parent = 0);

	private:
		QGraphicsTextItem* item;
		QLineEdit* editor;
		QDialogButtonBox* buttonBox;
		QVBoxLayout* layout;
		QHBoxLayout* hLayout;
		QSpinBox* fontSizeBox;

	signals:
		void accepted(QGraphicsTextItem* item);

	private slots:
		void accept();
		void changeItemText(const QString &text);
		void changeItemFontSize(int i);
	};
}

