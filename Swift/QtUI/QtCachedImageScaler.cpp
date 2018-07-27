/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtCachedImageScaler.h>

#include <boost/lexical_cast.hpp>

#include <QImage>

#include <Swiften/Base/Path.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

QtCachedImageScaler::QtCachedImageScaler() {
}

boost::filesystem::path QtCachedImageScaler::getScaledImage(const boost::filesystem::path& imagePath, int size) {
    boost::filesystem::path scaledImagePath(imagePath);
    std::string suffix = "." + std::to_string(size);
    scaledImagePath = stringToPath(pathToString(scaledImagePath) + suffix);
    if (!boost::filesystem::exists(scaledImagePath)) {
        QImage image(P2QSTRING(pathToString(imagePath)));
        if (!image.isNull()) {
            if (image.width() > size || image.height() > size) {
                QImage scaledImage = image.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                scaledImage.save(P2QSTRING(pathToString(scaledImagePath)), "PNG");
            }
            else {
                image.save(P2QSTRING(pathToString(scaledImagePath)), "PNG");
            }
        }
        else {
            return imagePath;
        }
    }
    return scaledImagePath;

}

}
