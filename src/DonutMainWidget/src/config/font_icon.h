#pragma once
#include <QObject>
#include <QPainter>
#include <QIconEngine>
#include <QApplication>
#include <QtCore>
#include <QPalette>

class FontIcon;
class FontIconEngine;
#define FIcon(code) FontIcon::icon(code)

class FontIconEngine : public QIconEngine
{
public:
    FontIconEngine();
    ~FontIconEngine();
    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
    void setFontFamily(const QString& family);
    // define icon code using QChar or implicit using ushort ...
    void setLetter(const QChar& letter);
    // You can set a base color. I don't advice. Keep system color
    void setBaseColor(const QColor& baseColor);
    virtual QIconEngine* clone() const;

private:
    QString mFontFamily;
    QChar mLetter;
    QColor mBaseColor;
};

class FontIcon : public QObject
{
    Q_OBJECT

public:
    // add Font. By default, the first one is used
    static bool addFont(const QString& filename);
    static FontIcon* instance();
    // main methods. Return icons from code
    static QIcon icon(const QChar& code, const QColor& baseColor = QColor(), const QString& family = QString());
    // return added fonts
    const QStringList& families() const;

protected:
    void addFamily(const QString& family);


private:
    explicit FontIcon(QObject* parent = 0);
    ~FontIcon();
    static FontIcon* mInstance;
    QStringList mfamilies;


};


