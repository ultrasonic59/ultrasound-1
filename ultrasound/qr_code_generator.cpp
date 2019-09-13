#include "qr_code_generator.h"

#include <string>
#include <vector>

#include <QPainter>
#include <QSvgRenderer>

#include "QrCode.hpp"

bool QrCodeGenerator::GenerateImage(QString url, QImage* image) {
	if (!image)
		return false;

	// Simple operation
	qrcodegen::QrCode qr0 = qrcodegen::QrCode::encodeText(url.toUtf8(), qrcodegen::QrCode::Ecc::MEDIUM);
	std::string std_svg = qr0.toSvgString(4);
	QString svg = QString::fromUtf8(std_svg.c_str());
	QSvgRenderer renderer(svg.toUtf8());
	QPainter painter(image);
	renderer.render(&painter);
	return true;
}