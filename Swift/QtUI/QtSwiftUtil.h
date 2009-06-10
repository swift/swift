#ifndef SWIFT_QtSwiftUtil_H
#define SWIFT_QtSwiftUtil_H

#define P2QSTRING(a) QString::fromUtf8(a.getUTF8Data())
#define Q2PSTRING(a) Swift::String(a.toUtf8())

#endif
