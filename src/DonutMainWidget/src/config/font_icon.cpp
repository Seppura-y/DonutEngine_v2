#include "font_icon.h"
#include <QDebug>
#include <QFontDatabase>
FontIcon* FontIcon::mInstance = Q_NULLPTR;

bool FontIcon::addFont(const QString& filename)
{
    int id = QFontDatabase::addApplicationFont(filename);

    if (id == -1) {
        qWarning() << "Cannot load font";
        return false;
    }

    QString family = QFontDatabase::applicationFontFamilies(id).first();
    instance()->addFamily(family);
    return true;
}

FontIcon* FontIcon::instance()
{
    if (!mInstance)
        mInstance = new FontIcon;

    return mInstance;
}

QIcon FontIcon::icon(const QChar& code, const QColor& baseColor, const QString& family)
{
    if (instance()->families().isEmpty())
    {
        qWarning() << Q_FUNC_INFO << "No font family installed";
        return QIcon();
    }

    QString useFamily = family;
    if (useFamily.isEmpty())
        useFamily = instance()->families().first();


    FontIconEngine* engine = new FontIconEngine;
    engine->setFontFamily(useFamily);
    engine->setLetter(code);
    engine->setBaseColor(baseColor);
    return QIcon(engine);


}


const QStringList& FontIcon::families() const
{
    return mfamilies;
}

void FontIcon::addFamily(const QString& family)
{
    mfamilies.append(family);
}

FontIcon::FontIcon(QObject* parent)
    :QObject(parent)
{

}

FontIcon::~FontIcon()
{

}

//=======================================================================================================


FontIconEngine::FontIconEngine() : QIconEngine()
{

}

FontIconEngine::~FontIconEngine()
{
}

void FontIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state);
    QFont font = QFont(mFontFamily);
    int drawSize = qRound(rect.height() * 1.2);
    font.setPixelSize(drawSize);
    //font.setPointSize(20);

    QColor penColor;
    if (!mBaseColor.isValid())
        penColor = QApplication::palette("QWidget").color(QPalette::Normal, QPalette::ButtonText);
    else
        penColor = mBaseColor;

    if (mode == QIcon::Disabled)
        penColor = QApplication::palette("QWidget").color(QPalette::Disabled, QPalette::ButtonText);


    if (mode == QIcon::Selected)
        penColor = QApplication::palette("QWidget").color(QPalette::Active, QPalette::ButtonText);


    painter->save();
    painter->setPen(QPen(penColor));
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter | Qt::AlignVCenter, mLetter);

    painter->restore();
}

QPixmap FontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    QPixmap pix(size);
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    paint(&painter, QRect(QPoint(0, 0), size), mode, state);
    return pix;

}

void FontIconEngine::setFontFamily(const QString& family)
{
    mFontFamily = family;
}

void FontIconEngine::setLetter(const QChar& letter)
{
    mLetter = letter;
}

void FontIconEngine::setBaseColor(const QColor& baseColor)
{
    mBaseColor = baseColor;
}


QIconEngine* FontIconEngine::clone() const
{
    FontIconEngine* engine = new FontIconEngine;
    engine->setFontFamily(mFontFamily);
    engine->setBaseColor(mBaseColor);
    return engine;
}
