/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "QtCachedImageScaler.h"

#include <QImage>
#include <boost/lexical_cast.hpp>

namespace Swift {

QtCachedImageScaler::QtCachedImageScaler() {
}

boost::filesystem::path QtCachedImageScaler::getScaledImage(const boost::filesystem::path& imagePath, int size) {
	boost::filesystem::path scaledImagePath(imagePath.string() + "." + boost::lexical_cast<std::string>(size));
	if (!boost::filesystem::exists(scaledImagePath)) {
		QImage image(imagePath.string().c_str());
		if (!image.isNull()) {
			if (image.width() > size || image.height() > size) {
				QImage scaledImage = image.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				scaledImage.save(QString::fromUtf8(scaledImagePath.string().c_str()), "PNG");
			}
			else {
				image.save(QString::fromUtf8(scaledImagePath.string().c_str()), "PNG");
			}
		}
		else {
			return imagePath;
		}
	}
	return scaledImagePath;

}

}
