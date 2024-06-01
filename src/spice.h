#pragma once

#include <spice-client.h>

#include <QQuickPaintedItem>

class Spice : public QQuickItem
{

    Q_OBJECT
    QML_NAMED_ELEMENT(Spice)

    struct Private;
    QScopedPointer<Private> d;

public:
    explicit Spice(QQuickItem* parent = nullptr);
    ~Spice();

    void channelNew(SpiceSession* session, SpiceChannel* channel);
    void channelEvent(SpiceChannel* channel, SpiceChannelEvent event);

    QSGNode *updatePaintNode(QSGNode * oldNode, QQuickItem::UpdatePaintNodeData *data) override;

    Q_INVOKABLE void start();

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    QString url() const;
    void setUrl(const QString& url);
    Q_SIGNAL void urlChanged();

    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    bool running() const;
    Q_SIGNAL void runningChanged();

    void primaryCreate(SpiceDisplayChannel* display, SpiceSurfaceFmt format, int width, int height, int stride, int shmid, gpointer imgData);
    void primaryDestroy(SpiceDisplayChannel* display);
    void displayMark(SpiceDisplayChannel* display, int mark);
    void displayInvalidate(SpiceDisplayChannel* display, int x, int y, int width, int height);

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

};
