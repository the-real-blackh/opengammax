/***************************************************************************
* Copyright (C) 2011 by LeDoyle Roy Pingel
*
* This file is part of OpenGammaCapture (ogcapture).
* OpenGammaCapture is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* OpenGammaCapture is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with OpenGammaX. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/




#ifndef ALSASTREAM_H
#define ALSASTREAM_H
/*
#include <QtGui/QMainWindow>
#ifdef Q_OS_LINUX
#include <QtGui/QMdiSubWindow>
#endif
#ifdef Q_OS_WIN32
#include <QtGui/QMdiSubwindow>
#endif
*/
#include <QSignalMapper>
#include <QObject>
#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/control_external.h>
#include <qtimer.h>
#include <QtGui/QMainWindow>
#include <QSignalMapper>
#include <QTimer>
#include <doblst.h>
#include "sample.h"



/* For INTERLEAVED access, each frame in the buffer contains the
 * consecutive sample data for the channels. For 16 Bit stereo data,
 * this means that the buffer contains alternating words of sample data
 * for the left and right channel. For NONINTERLEAVED access, each
 *  period contains first all sample data for the first channel
 * followed by the sample data for the second channel and so on. */ 

#define ALSA_CHAN_SHIFT 2

// 10 uS sample rate
#define ALSA_TRIG_TOL 3
#define ALSA_SIG_INV FALSE
#define ALSA_PERIOD_SIZE_F  65536
#define ALSA_PERIODS 4
/* The unit of the buffersize depends on the function. Sometimes it is
 * given in bytes, sometimes the number of frames has to be specified.
 * One frame is the sample data vector for all channels. For 16 Bit
 * stereo data, one frame has a length of four bytes. */
#define ALSA_BUFFER_SIZE_F (ALSA_PERIOD_SIZE_F * ALSA_PERIODS)

#define ALSA_SIG_INV FALSE


typedef struct frame {
    short chan0;
    short chan1;
}frame_t;


class AlsaStream: public QObject
{
    Q_OBJECT
public:

    AlsaStream ( QObject *parent = 0 )
        : QObject( parent ) {}

    bool open(const char* deviceName, Sample  & samp);
    bool close(void);
    bool clear(void);
    int nchans(void);
    bool readSpectrum(Sample & samp);
    bool readWaveforms(Sample & samp);
    bool reRead(Sample & samp);
    bool startCapture(void);
    bool stopCapture(void);
    bool filterPulse(Intlst & pulse);
    bool setSettings(bool nuInvert, int nuThreshold);

public slots:
    bool capture(void);

private:
    bool setParamsHW(const char *id);
    bool setParams();
    bool setScheduler(void);
    bool captureWave(Intlst & lessRaw);
    bool captureSpectrum(Intlst & lessRaw);
    //QwtPlot qwtPulsePlot ();

    snd_pcm_hw_params_t *hwParams;
    snd_pcm_sw_params_t *swParams;
    QList<Intlst> pulses;
};

#endif      // ALSASTREAM_H

/*
PCM error codes

snd_strerror(err) returns the text below

error code 0=Success
error code 1=Operation not permitted
error code 2=No such file or directory
error code 3=No such process
error code 4=Interrupted system call
error code 5=Input/output error
error code 6=No such device or address
error code 7=Argument list too long
error code 8=Exec format error
error code 9=Bad file descriptor
error code 10=No child processes
error code 11=Resource temporarily unavailable
error code 12=Cannot allocate memory
error code 13=Permission denied
error code 14=Bad address
error code 15=Block device required
error code 16=Device or resource busy
error code 17=File exists
error code 18=Invalid cross-device link
error code 19=No such device
error code 20=Not a directory
error code 21=Is a directory
error code 22=Invalid argument
error code 23=Too many open files in system
error code 24=Too many open files
error code 25=Inappropriate ioctl for device
error code 26=Text file busy
error code 27=File too large
error code 28=No space left on device
error code 29=Illegal seek
error code 30=Read-only file system
error code 31=Too many links
error code 32=Broken pipe
error code 33=Numerical argument out of domain
error code 34=Numerical result out of range
error code 35=Resource deadlock avoided
error code 36=File name too long
error code 37=No locks available
error code 38=Function not implemented
error code 39=Directory not empty
error code 40=Too many levels of symbolic links
error code 41=Unknown error 41
error code 42=No message of desired type
error code 43=Identifier removed
error code 44=Channel number out of range
error code 45=Level 2 not synchronized
error code 46=Level 3 halted
error code 47=Level 3 reset
error code 48=Link number out of range
error code 49=Protocol driver not attached
error code 50=No CSI structure available
error code 51=Level 2 halted
error code 52=Invalid exchange
error code 53=Invalid request descriptor
error code 54=Exchange full
error code 55=No anode
error code 56=Invalid request code
error code 57=Invalid slot
error code 58=Unknown error 58
error code 59=Bad font file format
error code 60=Device not a stream
error code 61=No data available
error code 62=Timer expired
error code 63=Out of streams resources
*/




