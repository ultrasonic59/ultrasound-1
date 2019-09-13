#pragma once

#include <QImage>
#include <QString>

class QrCodeGenerator {
public:
	static bool GenerateImage(QString url, QImage* image);
};