#ifndef VIRTVIEWERSESSION_H
#define VIRTVIEWERSESSION_H

#include <glib-object.h>
#include <spice-client.h>

static void spice_session_close(SpiceSession *session);
static bool spice_session_open_host(SpiceSession *session, const gchar *host, const gchar *port, const gchar *tlsport);
static void spice_session_usb_device_selection(SpiceSession *session);
static void spice_session_channel_new(SpiceChannel *channel, SpiceSession *session);
static void spice_session_channel_destory(SpiceChannel *channel, SpiceSession *session);
static void spice_session_disconnected(SpiceChannel *channel, SpiceSession *session);//Spice通道被销毁时执行清理和状态更新操作
static void spice_usb_device_reset(SpiceSession *session);//通过异步断开所有已连接的USB设备来实现USB设备的重置功能

#endif // VIRTVIEWERSESSION_H
