#include <QDebug>
#include <QPainter>
#include <QX11Info>
#include "spicewindow.h"
#include "ui_spicewindow.h"
#include "spice-widget.h"
#include "spice-widget-priv.h"

#define SPICE_MAIN_CHANNEL_GET_PRIVATE(obj)                             \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), SPICE_TYPE_MAIN_CHANNEL, SpiceMainChannelPrivate))

SpiceWindow::SpiceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SpiceWindow), agentConnected(true)
{
    ui->setupUi(this);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->ToolBar->addWidget(spacer);
    QAction* actionfullscreen = new QAction(QIcon(":/icons/icons/full-screen.png"), "fullscreen", this);
    QAction* actionmenu = new QAction(QIcon(":/icons/icons/caidan.png"), "menu", this);
    ui->ToolBar->addAction(actionfullscreen);
    ui->ToolBar->addAction(actionmenu);
}

static void main_channel_event(SpiceChannel *channel, SpiceChannelEvent event, gpointer data)
{
    switch (event) {
    case SPICE_CHANNEL_OPENED:
        printf("main channel: connected\n");
        break;
    case SPICE_CHANNEL_CLOSED:
        printf("main channel: connection lost\n");
        SpiceWindow::getSpice()->clearImage();
        break;
    case SPICE_CHANNEL_ERROR_CONNECT:
        printf("main channel: failed to connect\n");
        break;
    }
}

static void main_agent_update(SpiceChannel *channel, gpointer data)
{
    bool agent_connected;
    gboolean ac;
    g_object_get(channel, "agent-connected", &ac, NULL);
    agent_connected = ac ? true : false;
    SpiceWindow::getSpice()->setAgentConnected(agent_connected);
}

static void inputs_modifiers(SpiceChannel *channel, gpointer data)
{
    int m;

    g_object_get(channel, "key-modifiers", &m, NULL);
    SpiceWindow::getSpice()->setKbdModifiers(m);
}

static void channel_new(SpiceSession *session, SpiceChannel *channel)
{
    int id;
    g_object_get(channel, "channel-id", &id, NULL);
    if (SPICE_IS_MAIN_CHANNEL(channel)) {
       g_signal_connect(channel, "channel-event",
                         G_CALLBACK(main_channel_event), 0);
       g_signal_connect(channel, "main-agent-update",
                         G_CALLBACK(main_agent_update), channel);
    }
    if (SPICE_IS_DISPLAY_CHANNEL(channel)) {
        qDebug() << "display channel new";
        SpiceDisplay *display = SpiceWindow::getSpice()->spiceDisplayGLib();
        if (display)
            return;
        display = spice_display_new(session, id);
        SpiceWindow::getSpice()->setSpiceDisplayGLib(display);
    }
    if (SPICE_IS_INPUTS_CHANNEL(channel)) {
        g_signal_connect(channel, "inputs-modifiers",
                         G_CALLBACK(inputs_modifiers), 0);

    }
    if (SPICE_IS_PLAYBACK_CHANNEL(channel)) {
        SpiceAudio *audio = SpiceWindow::getSpice()->spiceAudioGLib();
        if (audio)
            return;
        audio = spice_audio_new(session, NULL, NULL);
        SpiceWindow::getSpice()->setSpiceAudioGLib(audio);
    }
    if (SPICE_IS_USBREDIR_CHANNEL(channel)){
        qDebug()<< "New USBREDIR Channel!" <<endl;
    }
}

static void channel_destroy(SpiceSession *session, SpiceChannel *channel)
{
    qDebug()<<"channel destory!"<<endl;
}

void SpiceWindow::spiceResize(int w, int h)
{
    SpiceDisplayPrivate *d = SPICE_DISPLAY_GET_PRIVATE(display);
    if (!d)
        return;
    spice_main_set_display(d->main, d->channel_id, 0, 0, w, h);
}

SpiceWindow *SpiceWindow::instance = NULL;
SpiceWindow * SpiceWindow::getSpice(QWidget* parent)
{
    if (instance)
        return instance;
    instance = new SpiceWindow(parent);
    return instance;
}

void SpiceWindow::settingsChanged(int w, int h, int bpp)
{
    dataWidth = w;
    dataHeight = h;
    rateW = double(height()) / double(dataHeight);
    rateH = double(height()) / double(dataHeight);
    double scaleFactor = qMin(rateW, rateH);
    img = QImage(w, h, QImage::Format_RGB32);
    if (w != width() || h != height())
        spiceResize(w*rateW, h*rateH);
    //imageSize(w, h);
//    // 计算缩放因子
//     img = img.scaledToWidth(3000);
//    double scaleX = double(width()) / double(w);
//    double scaleY = double(height()) / double(h);
//    double scaleFactor = qMin(scaleX, scaleY); // 取较小的缩放因子

//    // 缩放图片
//    QImage scaledImage = img.scaled(w * scaleFactor, h * scaleFactor, Qt::KeepAspectRatio);

//    // 更新成员变量
//    img = scaledImage;

//    // 重新绘制窗口
//    update();

    Q_EMIT imageSize(w, h);
}

void SpiceWindow::connectToGuest(const QString &host, const QString &port)
{
    SGsession = spice_session_new();
    g_object_set(SGsession, "host", \
                 host.toLatin1().constData(), NULL);
    g_object_set(SGsession, "port", \
                 port.toLatin1().constData(), NULL);
    g_signal_connect(SGsession, "channel-new",
                                 G_CALLBACK(channel_new), 0);
    g_signal_connect(SGsession, "channel-destroy", G_CALLBACK(channel_destroy), 0);
    if (!spice_session_connect(SGsession)) {
        qCritical() << "Unable to connect to guest at" << host << ":" << port;

                // 通知用户连接失败，可以使用Qt的信号和槽来进行
//                Q_EMIT connectionFailed("Unable to connect to the SPICE server.");

                // 清理资源，如果有必要的话
                spice_session_disconnect(SGsession);
                g_object_unref(SGsession);
                SGsession = NULL;
    }
}

void SpiceWindow::clearImage()
{
    img = QImage(dataWidth, dataHeight, QImage::Format_RGB32);
    if (!img.isNull()) {
        QPainter painter(&img);
        painter.setBrush(QBrush(Qt::black));
        painter.drawRect(0, 0, dataWidth, dataHeight);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, tr("Connection lost, Please reconnect."));
        painter.end();
    }
}

void SpiceWindow::updateImage(uchar *data, int x, int y, int w, int h)
{
    uint *source = reinterpret_cast<uint*>(data);
    for (int i = y; i < y + h; i++)
        for (int j = x; j < x + w; j++)
            img.setPixel(j, i, source[dataWidth * i + j]);
    update(x, y, w, h);
}

void SpiceWindow::setKbdModifiers(int m)
{
    scrollLock = m & SPICE_KEYBOARD_MODIFIER_FLAGS_SCROLL_LOCK;
    capsLock   = m & SPICE_KEYBOARD_MODIFIER_FLAGS_CAPS_LOCK;
    numLock    = m & SPICE_KEYBOARD_MODIFIER_FLAGS_NUM_LOCK;
}

void SpiceWindow::setSpiceAudioGLib(SpiceAudio *sa)
{
    audio = sa;
}

void SpiceWindow::setSpiceDisplayGLib(SpiceDisplay *sd)
{
    display = sd;
    SpiceDisplayPrivate *d = SPICE_DISPLAY_GET_PRIVATE(display);
}

quint32 SpiceWindow::getKeyboardLockModifiers()
{
    XKeyboardState keyboard_state;
    Display *x_display = QX11Info::display();
    quint32 modifiers = 0;

    XGetKeyboardControl(x_display, &keyboard_state);

    if (keyboard_state.led_mask & 0x01)
        modifiers |= SPICE_INPUTS_CAPS_LOCK;
    if (keyboard_state.led_mask & 0x02)
        modifiers |= SPICE_INPUTS_NUM_LOCK;
    if (keyboard_state.led_mask & 0x04)
        modifiers |= SPICE_INPUTS_SCROLL_LOCK;

    return modifiers;
}

SpiceWindow::~SpiceWindow()
{
    delete ui;
}
