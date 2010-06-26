/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "DelegateCommons.h"

namespace Swift {


void DelegateCommons::drawElidedText(QPainter* painter, const QRect& region, const QString& text) {
	QString adjustedText(painter->fontMetrics().elidedText(text, Qt::ElideRight, region.width(), Qt::TextShowMnemonic));
	painter->drawText(region, Qt::AlignTop, adjustedText);
}


const int DelegateCommons::horizontalMargin = 2;
const int DelegateCommons::verticalMargin = 2;
const int DelegateCommons::farLeftMargin = 2;



}
