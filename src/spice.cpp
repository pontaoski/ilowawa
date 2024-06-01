#include <spice-client.h>

#include <QImage>
#include <QSGImageNode>
#include <QPainter>

#include "channel-display.h"
#include "channel-inputs.h"
#include "spice.h"

struct Spice::Private
{
    QString url;
    SpiceSession* session = nullptr;
    SpiceMainChannel* main = nullptr;
    SpiceInputsChannel* inputs = nullptr;

    const uchar* imageData = nullptr;
    int stride = -1;
    QImage::Format format = QImage::Format_Invalid;

    int displayId = -1;
    int width = -1;
    int height = -1;
    bool mouseReset = false;
    int px = -1;
    int py = -1;
};

static void channelNewThunk(SpiceSession *session, SpiceChannel *channel, gpointer data)
{
    reinterpret_cast<Spice*>(data)->channelNew(session, channel);
}

static void channelEventThunk(SpiceChannel *channel, SpiceChannelEvent event, gpointer data)
{
    reinterpret_cast<Spice*>(data)->channelEvent(channel, event);
}

static void primaryCreateThunk(
    SpiceDisplayChannel* channel,
    SpiceSurfaceFmt format,
    int width,
    int height,
    int stride,
    int shmid,
    gpointer imgdata,
    gpointer userdata
)
{
    reinterpret_cast<Spice*>(userdata)->primaryCreate(
        channel,
        format,
        width,
        height,
        stride,
        shmid,
        imgdata
    );
}

static void primaryDestroyThunk(
    SpiceDisplayChannel* channel,
    gpointer userdata
)
{
    reinterpret_cast<Spice*>(userdata)->primaryDestroy(channel);
}

static void displayMarkThunk(
    SpiceDisplayChannel* channel,
    int mark,
    gpointer userdata
)
{
    reinterpret_cast<Spice*>(userdata)->displayMark(channel, mark);
}

static void displayInvalidateThunk(
    SpiceDisplayChannel* channel,
    int x,
    int y,
    int width,
    int height,
    gpointer userdata
)
{
    reinterpret_cast<Spice*>(userdata)->displayInvalidate(channel, x, y, width, height);
}

Spice::Spice(QQuickItem* parent) : QQuickItem(parent), d(new Private)
{
    setFlags(QQuickItem::ItemHasContents);
    d->session = spice_session_new();
    qWarning() << "made session" << d->session;
    g_signal_connect(d->session, "channel-new", G_CALLBACK(channelNewThunk), this);

    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
}

Spice::~Spice()
{

}

void Spice::channelNew(SpiceSession* session, SpiceChannel* channel)
{
    gint kind;
    g_object_get(G_OBJECT(channel), "channel-type", &kind, nullptr);
    qWarning() << "new channel in session" << spice_channel_type_to_string(kind);
    g_signal_connect(channel, "channel-event", G_CALLBACK(channelEventThunk), this);

    switch (kind) {
    case SPICE_CHANNEL_MAIN:
        d->main = SPICE_MAIN_CHANNEL(channel);
        spice_channel_connect(channel);
        break;
    case SPICE_CHANNEL_INPUTS:
        d->inputs = SPICE_INPUTS_CHANNEL(channel);
        spice_channel_connect(channel);
        break;
    case SPICE_CHANNEL_DISPLAY:
        g_signal_connect(channel, "display-primary-create", G_CALLBACK(primaryCreateThunk), this);
        g_signal_connect(channel, "display-primary-destroy", G_CALLBACK(primaryDestroyThunk), this);
        g_signal_connect(channel, "display-invalidate", G_CALLBACK(displayInvalidateThunk), this);
        g_signal_connect(channel, "display-mark", G_CALLBACK(displayMarkThunk), this);

        spice_channel_connect(channel);

        int id;
        g_object_get(channel, "channel-id", &id, nullptr);
        spice_main_channel_update_display_enabled(SPICE_MAIN_CHANNEL(d->main), id, true, true);
        d->displayId = id;

        break;
    }
}

void Spice::channelEvent(SpiceChannel* channel, SpiceChannelEvent event)
{
    switch (event) {
    case SPICE_CHANNEL_ERROR_CONNECT:
    case SPICE_CHANNEL_ERROR_TLS:
    case SPICE_CHANNEL_ERROR_LINK:
    case SPICE_CHANNEL_ERROR_AUTH:
    case SPICE_CHANNEL_ERROR_IO:
        qWarning() << spice_channel_get_error(channel)->message;
    default:
        break;
    }

    switch (event) {
    case SPICE_CHANNEL_NONE:
        qWarning() << channel << "NONE";
        break;
    case SPICE_CHANNEL_OPENED:
        qWarning() << channel << "OPENED";
        break;
    case SPICE_CHANNEL_SWITCHING:
        qWarning() << channel << "SWITCHING";
        break;
    case SPICE_CHANNEL_CLOSED:
        qWarning() << channel << "CLOSED";
        break;
    case SPICE_CHANNEL_ERROR_CONNECT:
        qWarning() << channel << "ERROR_CONNECT";
        break;
    case SPICE_CHANNEL_ERROR_TLS:
        qWarning() << channel << "ERROR_TLS";
        break;
    case SPICE_CHANNEL_ERROR_LINK:
        qWarning() << channel << "ERROR_LINK";
        break;
    case SPICE_CHANNEL_ERROR_AUTH:
        qWarning() << channel << "ERROR_AUTH";
        break;
    case SPICE_CHANNEL_ERROR_IO:
        qWarning() << channel << "ERROR_IO";
        break;
    }
}

QSGNode *Spice::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data)

    auto node = static_cast<QSGImageNode*>(oldNode);
    if (node == nullptr) {
        node = window()->createImageNode();
    }

    if (node->texture() != nullptr) {
        node->texture()->deleteLater();
    }

    QImage img(d->imageData, d->width, d->height, d->stride, d->format);
    img.detach();
    img.convertTo(QImage::Format_ARGB32);
    auto texture = window()->createTextureFromImage(img, QQuickWindow::TextureIsOpaque);

    node->setTexture(texture);
    node->setRect(QRect(0, 0, d->width, d->height));
    return node;
}

QString Spice::url() const
{
    return d->url;
}

void Spice::setUrl(const QString& url)
{
    if (d->url == url)
        return;

    d->url = url;
    g_object_set(d->session, "uri", qPrintable(url), nullptr);
}

void Spice::start()
{
    Q_ASSERT(spice_session_connect(d->session));
}

bool Spice::running() const
{
    return true;
}

void Spice::primaryCreate(SpiceDisplayChannel* display, SpiceSurfaceFmt format, int width, int height, int stride, int shmid, gpointer imgData)
{
    Q_UNUSED(display)
    Q_UNUSED(shmid)
    Q_UNUSED(format)

    d->imageData = reinterpret_cast<const uchar*>(imgData);
    d->width = width;
    d->height = height;
    d->stride = stride;
    switch (format) {
    case SPICE_SURFACE_FMT_1_A: d->format = QImage::Format_Mono; break;
    case SPICE_SURFACE_FMT_8_A: d->format = QImage::Format_Alpha8; break;
    case SPICE_SURFACE_FMT_16_555: d->format = QImage::Format_RGB555; break;
    case SPICE_SURFACE_FMT_32_xRGB: d->format = QImage::Format_RGB32; break;
    case SPICE_SURFACE_FMT_16_565: qFatal("unhandled format rgb 565");
    case SPICE_SURFACE_FMT_32_ARGB: d->format = QImage::Format_ARGB32; break;
    case SPICE_SURFACE_FMT_INVALID: qFatal("invalid format");
    default: qFatal("unhandled format");
    }
    setImplicitSize(width, height);
}

void Spice::primaryDestroy(SpiceDisplayChannel* display)
{
    Q_UNUSED(display)

    d->imageData = nullptr;
    setImplicitSize(0, 0);
}

void Spice::displayInvalidate(SpiceDisplayChannel* display, int x, int y, int width, int height)
{
    Q_UNUSED(display)

    update();
}

void Spice::displayMark(SpiceDisplayChannel* display, int mark)
{
    int id;
    g_object_get(display, "channel-id", &id, nullptr);
    spice_main_channel_update_display_enabled(SPICE_MAIN_CHANNEL(d->main), id, mark != 0, true);
}

void Spice::keyPressEvent(QKeyEvent *event)
{
    if (!d->inputs) return;
}

void Spice::keyReleaseEvent(QKeyEvent *event)
{
    if (!d->inputs) return;
}

inline QPair<int, int> buttonsFromEvent(QMouseEvent *event)
{
    int button = 0;

    switch (event->button()) {
    case Qt::LeftButton:
        button = SPICE_MOUSE_BUTTON_LEFT; break;
    case Qt::RightButton:
        button = SPICE_MOUSE_BUTTON_RIGHT; break;
    case Qt::MiddleButton:
        button = SPICE_MOUSE_BUTTON_MIDDLE; break;
    case Qt::XButton1:
        button = SPICE_MOUSE_BUTTON_UP; break;
    case Qt::XButton2:
        button = SPICE_MOUSE_BUTTON_DOWN; break;
    default:
        break;
    }

    int buttonState = 0;

    if (event->buttons() & Qt::LeftButton)
        buttonState |= SPICE_MOUSE_BUTTON_MASK_LEFT;
    if (event->buttons() & Qt::MiddleButton)
        buttonState |= SPICE_MOUSE_BUTTON_MASK_MIDDLE;
    if (event->buttons() & Qt::RightButton)
        buttonState |= SPICE_MOUSE_BUTTON_MASK_RIGHT;
    if (event->buttons() & Qt::XButton1)
        buttonState |= SPICE_MOUSE_BUTTON_MASK_UP;
    if (event->buttons() & Qt::XButton2)
        buttonState |= SPICE_MOUSE_BUTTON_MASK_DOWN;

    return {button, buttonState};
}

void Spice::mousePressEvent(QMouseEvent *event)
{
    if (!d->inputs) return;

    auto [button, buttonState] = buttonsFromEvent(event);

    spice_inputs_channel_button_press(d->inputs, button, buttonState);

    d->px = event->x();
    d->py = event->y();

    setKeepMouseGrab(true);
    grabMouse();
}

void Spice::mouseMoveEvent(QMouseEvent *event)
{
    if (!d->inputs) return;

    auto [button, buttonState] = buttonsFromEvent(event);
    Q_UNUSED(button)

    spice_inputs_channel_motion(d->inputs, event->x() - d->px, event->y() - d->py, buttonState);

    d->px = event->x();
    d->py = event->y();
}

void Spice::mouseReleaseEvent(QMouseEvent *event)
{
    if (!d->inputs) return;

    auto [button, buttonState] = buttonsFromEvent(event);

    spice_inputs_channel_button_release(d->inputs, button, buttonState);
}

void Spice::hoverEnterEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
}

void Spice::hoverMoveEvent(QHoverEvent *event)
{
    spice_inputs_channel_motion(d->inputs, event->pos().x() - event->oldPos().x(), event->pos().y() - event->oldPos().y(), 0);
}

void Spice::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event)
}

