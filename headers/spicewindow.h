#ifndef SPICEWINDOW_H
#define SPICEWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>

extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <glib-object.h>
#include <spice-client.h>
#include <spice-session.h>
#include <channel-main.h>
#include <channel-display.h>
#include <channel-inputs.h>
#include <spice-audio.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
};

#include <spice-widget.h>

namespace Ui {
class SpiceWindow;
}

class SpiceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SpiceWindow(QWidget *parent = nullptr);
    ~SpiceWindow();
    static SpiceWindow *getSpice(QWidget *parent = NULL);
    void connectToGuest(const QString &host, const QString &port);
    void disconnectFromGuest();
    bool getAgentConnected() { return agentConnected; }
    void setAgentConnected(bool connected) { agentConnected = connected; }
    void settingsChanged(int width, int height, int bpp);
    void spiceResize(int w, int h);
    void setKbdModifiers(int m);
    void setSpiceAudioGLib(SpiceAudio *sa);
    void setSpiceDisplayGLib(SpiceDisplay *sd);
    SpiceAudio *spiceAudioGLib() { return audio; }
    SpiceDisplay *spiceDisplayGLib() { return display; }
    void clearImage();
    void updateImage(uchar *data, int x, int y, int width, int height);
    quint32 getKeyboardLockModifiers();
    void showCursor(bool visible)
    {
        if (visible)
            setCursor(Qt::ArrowCursor);
        else
            setCursor(Qt::BlankCursor);
    }

Q_SIGNALS:
    void imageSize(int, int);

private:
    Ui::SpiceWindow *ui;
    bool           agentConnected;
    static QMap<int, int> * getKeymap();
    void prepareMouseData();
    static SpiceWindow *instance;
    SpiceSession * SGsession;
    SpiceDisplay * display;
    SpiceAudio   * audio;
    uchar        * buf;
    QImage         img;
    int            dataWidth;
    int            dataHeight;
    double         rateW;
    double         rateH;
    bool           scrollLock;
    bool           numLock;
    bool           capsLock;
    static QMap<int, int>* keymap;
};

#endif // SPICEWINDOW_H
