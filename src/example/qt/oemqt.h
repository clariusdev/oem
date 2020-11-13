#pragma once

#include "ble.h"

namespace Ui
{
    class Oem;
}

class UltrasoundImage;
class RfSignal;

#define CONNECT_EVENT   static_cast<QEvent::Type>(QEvent::User + 1)
#define POWER_EVENT     static_cast<QEvent::Type>(QEvent::User + 2)
#define SWUPDATE_EVENT  static_cast<QEvent::Type>(QEvent::User + 3)
#define LIST_EVENT      static_cast<QEvent::Type>(QEvent::User + 4)
#define IMAGE_EVENT     static_cast<QEvent::Type>(QEvent::User + 5)
#define PRESCAN_EVENT   static_cast<QEvent::Type>(QEvent::User + 6)
#define RF_EVENT        static_cast<QEvent::Type>(QEvent::User + 7)
#define IMAGING_EVENT   static_cast<QEvent::Type>(QEvent::User + 8)
#define BUTTON_EVENT    static_cast<QEvent::Type>(QEvent::User + 9)
#define ERROR_EVENT     static_cast<QEvent::Type>(QEvent::User + 10)
#define PROGRESS_EVENT  static_cast<QEvent::Type>(QEvent::User + 11)

namespace event
{
    /// wrapper for connection events that can be posted from the api callbacks
    class Connection : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] code the connection code
        /// @param[in] port the connection port
        /// @param[in] msg connection message
        Connection(int code, int port, const QString& msg) : QEvent(CONNECT_EVENT), code_(code), port_(port), message_(msg) { }
        /// retrieves the connection code
        /// @return the connection code
        int code() const { return code_; }
        /// retrieves the port
        /// @return the connection port
        int port() const { return port_; }
        /// retrieves the message
        /// @return the connection message
        QString message() const { return message_; }

    protected:
        int code_;          ///< connection code
        int port_;          ///< connection port
        QString message_;   ///< message
    };

    /// wrapper for power down events that can be posted from the api callbacks
    class PowerDown : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] code the power down code
        /// @param[in] tm any timeout associated
        PowerDown(int code, int tm) : QEvent(POWER_EVENT), code_(code), timeOut_(tm) { }
        /// retrieves the power down code
        /// @return the power down code
        int code() const { return code_; }
        /// retrieves the timeout
        /// @return any associated timeout
        int timeOut() const { return timeOut_; }

    protected:
        int code_;      ///< power down code
        int timeOut_;   ///< associated timeout
    };

    /// wrapper for software update that can be posted from the api callbacks
    class SwUpdate : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] code the sw update code
        SwUpdate(int code) : QEvent(SWUPDATE_EVENT), code_(code)  { }
        /// retrieves the sw update code
        /// @return the sw update code
        int code() const { return code_; }

    private:
        int code_;  ///< the software update code
    };

    /// wrapper for list events
    class List : public QEvent
    {
    public:
        /// default constructor
        List(const char* list, bool probes) : QEvent(LIST_EVENT), probes_(probes)
        {
            QString buf = QString::fromLatin1(list);
            list_ = buf.split(',');
        }
        /// retrieves the list
        /// @return the list
        QStringList list() const { return list_; }
        bool probes() const { return probes_; }

    private:
        QStringList list_;  ///< resultant list
        bool probes_;       ///< flag for probes vs applications
    };

    /// wrapper for new image events that can be posted from the api callbacks
    class Image : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] data the image data
        /// @param[in] w the image width
        /// @param[in] h the image height
        /// @param[in] bpp the image bits per pixel
        Image(QEvent::Type evt, const void* data, int w, int h, int bpp) : QEvent(evt), data_(data), width_(w), height_(h), bpp_(bpp) { }
        /// retrieves the image data from the event
        /// @return the event's encapsulated image data
        const void* data() const { return data_; }
        /// retrieves the image width from the event
        /// @return the event's encapsulated image width
        int width() const { return width_; }
        /// retrieves the image height from the event
        /// @return the event's encapsulated image height
        int height() const { return height_; }
        /// retrieves the bits per pixel from the event
        /// @return the event's encapsulated bits per pixel
        int bpp() const { return bpp_; }

    protected:
        const void* data_;  ///< pointer to the image data
        int width_;         ///< width of the image
        int height_;        ///< height of the image
        int bpp_;           ///< bits per pixel of the image (should always be 32)
    };

    /// wrapper for new data events that can be posted from the api callbacks
    class PreScanImage : public Image
    {
    public:
        /// default constructor
        /// @param[in] data the image data
        /// @param[in] w the image width
        /// @param[in] h the image height
        /// @param[in] bpp the image bits per sample
        /// @param[in] jpg the jpeg compression flag for the data
        PreScanImage(const void* data, int w, int h, int bpp, int jpg) : Image(PRESCAN_EVENT, data, w, h, bpp), jpeg_(jpg) { }
        /// retrieves the jpeg compression flag for the image
        /// @return the event's encapsulated jpeg compression flag for the image
        bool jpeg() const { return jpeg_; }

    private:
        bool jpeg_; ///< size of jpeg compressed image
    };

    /// wrapper for new rf events that can be posted from the api callbacks
    class RfImage : public Image
    {
    public:
        /// default constructor
        /// @param[in] data the rf data
        /// @param[in] l # of rf lines
        /// @param[in] s # of samples per line
        /// @param[in] bps bits per sample
        /// @param[in] lateral lateral spacing between lines
        /// @param[in] axial sample size
        RfImage(const void* data, int l, int s, int bps, double lateral, double axial) : Image(RF_EVENT, data, l, s, bps), lateral_(lateral), axial_(axial) { }
        /// retrieves the lateral spacing
        /// @return the lateral spacing
        double lateral() const { return lateral_; }
        /// retrieves the axial spacing
        /// @return the axial spacing
        double axial() const { return axial_; }

    private:
        double lateral_;    ///< spacing between each line
        double axial_;      ///< sample size
    };

    /// wrapper for imaging state events that can be posted from the api callbacks
    class Imaging : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] rdy the ready state
        /// @param[in] imaging the imaging state
        Imaging(bool rdy, bool imaging) : QEvent(IMAGING_EVENT), ready_(rdy), imaging_(imaging) { }
        /// retrieves the ready state from the event
        /// @return the event's encapsulated ready state
        bool ready() const { return ready_; }
        /// retrieves the imaging state from the event
        /// @return the event's encapsulated imaging state
        bool imaging() const { return imaging_; }

    private:
        bool ready_;    ///< the ready state
        bool imaging_;  ///< the imaging state
    };

    /// wrapper for button press events that can be posted from the api callbacks
    class Button : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] btn the button pressed
        /// @param[in] clicks # of clicks
        Button(int btn, int clicks) : QEvent(BUTTON_EVENT), button_(btn), clicks_(clicks) { }
        /// retrieves the button pressed
        /// @return the event's encapsulated which button was pressed
        int button() const { return button_; }
        /// retrieves the # of clicks used
        /// @return the event's encapsulated # of clicks used
        int clicks() const { return clicks_; }

    private:
        int button_;    ///< button pressed, 0 = up, 1 = down
        int clicks_;    ///< # of clicks
    };

    /// wrapper for error events that can be posted from the api callbacks
    class Error : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] err the error message
        Error(const QString& err) : QEvent(ERROR_EVENT), error_(err) { }
        /// retrieves the error message from the event
        /// @return the event's encapsulated error message
        QString error() const { return error_; }

    private:
        QString error_;     ///< the error message
    };

    /// wrapper for progress events that can be posted from the api callbacks
    class Progress : public QEvent
    {
    public:
        /// default constructor
        /// @param[in] progress the current progress
        Progress(int progress) : QEvent(PROGRESS_EVENT), progress_(progress) { }
        /// retrieves the current progress from the event
        /// @return the event's encapsulated progress
        int progress() const { return progress_; }

    private:
        int progress_;  ///< the current progress
    };
}

/// oem gui application
class Oem : public QMainWindow
{
    Q_OBJECT

public:
    explicit Oem(QWidget *parent = nullptr);
    ~Oem();

    static Oem* instance();

protected:
    virtual bool event(QEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    void loadProbes(const QStringList& probes);
    void loadApplications(const QStringList& probes);
    void newProcessedImage(const void* img, int w, int h, int bpp);
    void newPrescanImage(const void* img, int w, int h, int bpp, bool jpg);
    void newRfImage(const void* rf, int l, int s, int ss);
    void setConnected(int code, int port, const QString& msg);
    void poweringDown(int code, int tm);
    void softwareUpdate(int code);
    void imagingState(bool ready, bool imaging);
    void onButton(int btn, int clicks);
    void setProgress(int progress);
    void setError(const QString& err);
    void getParams();

public slots:
    void onBleProbe(int);
    void onBleConnect();
    void onBleSearch();
    void onPowerOn();
    void onPowerOff();
    void onWiFi();
    void onAp();
    void onConnect();
    void onFreeze();
    void onUpdate();
    void onLoad();
    void onProbeSelected(const QString& probe);
    void onMode(int);
    void onZoom(int);
    void incDepth();
    void decDepth();
    void onGain(int);
    void onColorGain(int);
    void tgcTop(int);
    void tgcMid(int);
    void tgcBottom(int);

private:
    bool connected_;            ///< connection state
    bool imaging_;              ///< imaging state
    Ui::Oem *ui_;               ///< ui controls, etc.
    UltrasoundImage* image_;    ///< image display
    RfSignal* signal_;          ///< rf signal display
    QImage prescan_;            ///< pre-scan converted image
    QTimer timer_;              ///< timer for updating probe status
    Ble ble_;                   ///< bluetooth module
    static std::unique_ptr<Oem> oem_;
};
