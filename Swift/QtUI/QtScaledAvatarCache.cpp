/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swift/QtUI/QtScaledAvatarCache.h>

#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QPainter>
#include <QPixmap>

#include <Swiften/Base/Log.h>

#include <Swift/QtUI/QtSwiftUtil.h>

namespace Swift {

namespace {
    // This number needs to be incremented whenever the avatar scaling procedure changes.
    const int QT_SCALED_AVATAR_CACHE_VERSION = 1;
}

QtScaledAvatarCache::QtScaledAvatarCache(int size) : size(size) {
}

static QPixmap cropToBiggestCenteredSquare(const QPixmap& input) {
    QPixmap squareCropped;
    if (input.width() != input.height()) {
        QRect centeredSquare;
        if (input.width() > input.height()) {
            int x = (input.width() - input.height()) / 2;
            centeredSquare = QRect(x, 0, input.height(), input.height());
        }
        else {
            int y = (input.height() - input.width()) / 2;
            centeredSquare = QRect(0, y, input.width(), input.width());
        }
        squareCropped = input.copy(centeredSquare);
    }
    else {
        squareCropped = input;
    }
    return squareCropped;
}

QString QtScaledAvatarCache::getScaledAvatarPath(const QString& path) {
    QFileInfo avatarFile(path);
    if (avatarFile.exists() && !avatarFile.absolutePath().startsWith(":/")) {
        QString cacheSubPath = QString("ScaledAvatarCacheV%1/%2").arg(QString::number(QT_SCALED_AVATAR_CACHE_VERSION), QString::number(size));
        if (!avatarFile.dir().mkpath(cacheSubPath)) {
            SWIFT_LOG(error) << "avatarFile.dir(): " << Q2PSTRING(avatarFile.dir().absolutePath()) << std::endl;
            SWIFT_LOG(error) << "Failed creating cache folder: " << Q2PSTRING(cacheSubPath) << std::endl;
            return path;
        }
        QDir targetDir(avatarFile.dir().absoluteFilePath(cacheSubPath));
        QString targetFile = targetDir.absoluteFilePath(avatarFile.baseName());
        if (!QFileInfo(targetFile).exists()) {
            QPixmap avatarPixmap;
            if (avatarPixmap.load(path)) {
                QPixmap squaredAvatarPixmap = cropToBiggestCenteredSquare(avatarPixmap);
                QPixmap maskedAvatar(squaredAvatarPixmap.size());
                maskedAvatar.fill(QColor(0, 0, 0, 0));
                QPainter maskPainter(&maskedAvatar);
                maskPainter.setBrush(Qt::black);
                maskPainter.drawRoundedRect(maskedAvatar.rect(), 25.0, 25.0, Qt::RelativeSize);
                maskPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
                maskPainter.drawPixmap(0, 0, squaredAvatarPixmap);
                maskPainter.end();
                if (!maskedAvatar.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation).save(targetFile, "PNG")) {
                    return path;
                }
            } else {
                SWIFT_LOG(warning) << "Failed to load " << Q2PSTRING(path) << std::endl;
            }
        }
        return targetFile;
    }
    else {
        return path;
    }
}


}
