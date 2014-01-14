

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



#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <stdlib.h>
#include <iostream>
#include <qpen.h>
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>

using namespace std;

#include "maininterface.h"
#include "alsaStream.h"
#include "sample.h"

#define ALSA_THRESHOLD (200)
#define ALSA_SIGNAL_INVERT TRUE

//these are static so they are never destroyed
static QTime wallClock;
static QDateTime acqDateTime;
static QDateTime sampleDateTime;
static int samplePeriod;        // calls capture() every N milliseconds
static long sampleRate;
static long liveTime;       // time spent sampling, in mS
static long realTime;       // elapsed wall clock time, in ms
static bool signalInvert;   // true = sign invert data
static int threshold;
static QTimer *timerAlsa;   // timer to regularly read raw data
static snd_pcm_t *pcm;
static snd_pcm_uframes_t bufferSize;
static Intlst spectrumTbl;     // histogram of pulse engergies
static long totalWidth, totalPulses, avgWidth;
static long over, under, coincidence, overFlow;
static bool waveform = FALSE;
int HWdev = 0;           // sound card
int HWchan = 0;           // channel of sound card



bool AlsaStream::setParams(void)
{
    int err;
    long tmp;

    err = snd_pcm_hw_params_malloc(&hwParams);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    err = snd_pcm_hw_params_any(pcm, hwParams);	/* get full configuration space for a PCM */
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    /* Needed only for xrun problems
    err = snd_pcm_hw_params_set_rate_resample(handle, hwparams,resample);
    if (err < 0) return err;
    */
    err = snd_pcm_hw_params_set_access(pcm, hwParams, (snd_pcm_access_t) SND_PCM_ACCESS_RW_INTERLEAVED );
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    err = snd_pcm_hw_params_set_format(pcm, hwParams, (snd_pcm_format_t) SND_PCM_FORMAT_S16_LE);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    /* Not used
    err = snd_pcm_hw_params_set_subformat(handle, hwParams, (snd_pcm_subformat_t) ALSA_SUBFORMAT);
    if (err < 0) return err;
    */
    unsigned int usr;
    err =  snd_pcm_hw_params_get_rate_max(hwParams, &usr, 0);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    cout << "Max supported sample rate is " << usr << endl;
    err = snd_pcm_hw_params_set_rate_near(pcm, hwParams, &usr, 0);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    sampleRate = (int) usr;
    cout << "sample rate set to: " << sampleRate << endl;
    //err = snd_pcm_hw_params_set_channels(handle, hwParams, (int) ALSA_CHANNELS);
    //if (err < 0) return err;
    err= snd_pcm_hw_params_set_periods(pcm, hwParams, (int) ALSA_PERIODS, 0);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    err = snd_pcm_hw_params_get_buffer_size_max(hwParams, &bufferSize);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    cout << "max supported buffer size: " << bufferSize << " frames" << endl;
    if(bufferSize > ALSA_BUFFER_SIZE_F) // 2^16, sample rate typ 96000 so use 1 sec updates
        bufferSize = ALSA_BUFFER_SIZE_F;
    tmp = bufferSize;
    err = snd_pcm_hw_params_set_buffer_size(pcm, hwParams, bufferSize);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    cout << "buffer size set to: " << bufferSize << " frames" << endl;
    /* Apply HW parameter settings to PCM device and prepare device  */
    err = snd_pcm_hw_params(pcm, hwParams);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    err = snd_pcm_prepare(pcm);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    // set up timer for periodic data capture
    timerAlsa = new QTimer(this);
    connect(timerAlsa, SIGNAL(timeout()), this, SLOT(capture()));
    // set up the raw buffer
    //buffer = (frame_t *) malloc(bufferSize * sizeof(frame_t));
    return true;
}



bool AlsaStream::setScheduler(void)
{
    int err;
    struct sched_param sched_param;
    err = sched_getparam(0, &sched_param);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    sched_param.sched_priority = sched_get_priority_max(SCHED_OTHER);  //SCHED_RR perfered
    //sched_param.sched_priority = 0;
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    return TRUE;
}

/* turn the sound device on */
bool AlsaStream::open(const char* deviceName, Sample & sam)
{
    int err;
    struct pollfd *pfd;
    FILE tmpfd;

    tmpfd._fileno = 0;
    spectrumTbl.clear();
    for(int n = 0; n < DISPLAY_CHANNELS; n++)   // this is to populate array for later indexing
        spectrumTbl << 1;
    signalInvert = ALSA_SIGNAL_INVERT;
    realTime = liveTime = 0;
    threshold = ALSA_THRESHOLD;
    totalWidth = avgWidth = totalPulses = over = under = coincidence = overFlow = 0;

    // check to see if already open  
    //if(fd != tmpfd)
    //    return fd;
    err = snd_pcm_open(&pcm, deviceName, (snd_pcm_stream_t)SND_PCM_STREAM_CAPTURE, SND_PCM_ASYNC);
    if (err < 0)
    {
        cout << snd_strerror(err);
        return FALSE;
    }
    err = setParams();  // this sets up the sound card
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    err = setScheduler();
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    /*
    // get handle for use of poll()
    err = snd_pcm_poll_descriptors(handle, descs, sizeof(descs) );
    if (err < 0) return false;
    pfd = descs;
    fd._fileno = pfd->fd;
    */
    startCapture();  // kick off pulse collection
    return TRUE;
}


/* close the sound device */
bool AlsaStream::close(void)
{
    int err;

    stopCapture();
    err = snd_pcm_close(pcm);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    err = snd_pcm_hw_params_malloc(&hwParams);
    if (err < 0)
    {
        cout << snd_strerror(err) << endl;
        return FALSE;
    }
    return true;
}


bool AlsaStream::setSettings(bool nuInvert, int nuThreshold){
    signalInvert = nuInvert;
    threshold = nuThreshold;
}

bool AlsaStream::clear(void)
{
    wallClock.restart();
    acqDateTime.currentDateTime();
    sampleDateTime.currentDateTime();
    liveTime = 0;       // time spent sampling, in mS
    realTime = 0;       // elapsed wall clock time, in ms
    spectrumTbl.clear();     // histogram of pulse engergies
    totalWidth = totalPulses = avgWidth = 0;
    over = under = coincidence = overFlow =0;
    waveform = FALSE;
    return TRUE;
}


// ADC resolution, # channels in spectrum
int AlsaStream::nchans(void)
{
    return (DISPLAY_CHANNELS);
}

bool AlsaStream::readSpectrum(Sample & sam)
{
    waveform = FALSE;
    spectrumTbl.clear();
    for(int n = 0; n < DISPLAY_CHANNELS; n++)   // this is to populate array for later indexing
       spectrumTbl << 1;
    sam.counts->updateCounts(spectrumTbl);
    sam.liveTime = liveTime;
    sam.realTime = realTime;
    sam.acqDateTime = acqDateTime;
    return TRUE;
}


bool AlsaStream::readWaveforms(Sample & sam)
{
    waveform = TRUE;
    sam.counts->updateCounts(spectrumTbl);
    return TRUE;
}

bool AlsaStream::reRead(Sample & sam)
{
    sam.counts->updateCounts(spectrumTbl);
    sam.liveTime = liveTime;
    sam.realTime = realTime;
    sam.acqDateTime = acqDateTime;
    return TRUE;
}

bool AlsaStream::startCapture(void)
{

    spectrumTbl.clear();
    for(int n = 0; n < DISPLAY_CHANNELS; n++)   // this is to populate array for later indexing
        spectrumTbl << 1;   // non-zero to force display of entire range
    realTime = 0;
    liveTime = 0;
    long time = bufferSize;
    time *= 1000;
    time /= sampleRate;
    samplePeriod = time;
    //capture();  // call this to initiallize with a sample
    timerAlsa->start(((int) samplePeriod) - 50);
    wallClock.start();
    acqDateTime.currentDateTime();
    return true;
}

bool AlsaStream::stopCapture(void)
{
    timerAlsa->stop();
    realTime = wallClock.elapsed();
    return true;
}

bool AlsaStream::capture(void)
{ 
    int tmp,n;

    // raw ADC data
    int sampSz;   // number of frames actually read in current sample
    frame_t rawBuf[bufferSize], *fp;
    fp = &rawBuf[0];
    overFlow = 0;
    // get a valid sample of data
    while ((sampSz = snd_pcm_readi(pcm, fp, (snd_pcm_uframes_t) bufferSize)) <= 0) {
        //cout << "Error reading sound card: " << sampSz << " = "  << snd_strerror(sampSz) << endl;
        snd_pcm_prepare(pcm);
        if(overFlow++ > 20)
            break;
    }
   // cout << "Read " << sampSz << " frames from sound card"<< endl;
    // recalculate sample time as size of read can vary
    long time = (long) sampSz;
    time *= 1000;
    time /= sampleRate;         // samples per second
    liveTime += time;           // time in mS
    // set up for processing the data
    Intlst lessRaw;
    //lessRaw.clear();
    if(HWchan == 0){
        for(n = 0, fp = &rawBuf[0];  n < sampSz;  fp++, n++){
            if(signalInvert)
               lessRaw << (0 - fp->chan0);
            else
                lessRaw << fp->chan0;
        }
    }
    if(HWchan == 1){
        for(n = 0, fp = &rawBuf[0];  n < sampSz;  fp++, n++){
            if(signalInvert)
               lessRaw << (0 - fp->chan1);
            else
                lessRaw << fp->chan1;
        }
    }
    // call the desired discriminator
    if(waveform)
        return (captureWave(lessRaw));
     else
        return(captureSpectrum(lessRaw));
return TRUE;
}


bool AlsaStream::captureWave(Intlst & lessRaw)
{
    int n, m, lim;
    lim = lessRaw.size();
    spectrumTbl.clear();
    // identify a peak and display it
    for(n = 10;  n < lim;  n++){
        if(lessRaw.at(n) > threshold && lessRaw.at(n-1) < threshold){       // find the rising edge
            for(m = 40, n -= 10; m > 0;  n++, m--){
                if(lessRaw.at(n) < threshold)     // display exactly as software sees the pulses
                    spectrumTbl << 1;           // set to 1 otherwise gaps in display
                else
                    spectrumTbl << lessRaw.at(n) - threshold;
                //for(;n < lim && lessRaw.at(n) > threshold;  n++)
                //   spectrumTbl << lessRaw.at(n);
                //for(m = ; n < lim && m < 10; n++, m--)
                //spectrumTbl << lessRaw.at(n);
                //break;
            }
            break;
        }
    }
    return TRUE;
}

    bool AlsaStream::captureSpectrum(Intlst  & lessRaw)
    {
    int n, m, maxWidth;
    // Isolate pulses
    for(n = 1; n < lessRaw.size(); n++){    // start at 1 to allow looking backwards on pulse capture
        if(lessRaw.at(n) > threshold){
            Intlst *pulse = new Intlst();
            pulse->clear();
            *pulse << lessRaw.at(n-1) - threshold;         // capture points before and after pulse detected
            for(m = 0; n < lessRaw.size() && lessRaw.at(n) > threshold  && m < 100; m++, n++){
                *pulse << lessRaw[n] - threshold;
            }
            if(n < lessRaw.size())
                *pulse << lessRaw.at(n) - threshold;           // points before and after are used to remove partial pulses
            pulses.append(*pulse);
        }
    }
    // Filter pulses like a discriminator circuit
    if(avgWidth > 0)
        maxWidth = avgWidth + (avgWidth/3);
    Intlst pulse;
    while(!(pulses.empty())){
        pulse = pulses.takeFirst();
        // check for partial pulse
        if( pulse.first() > 0 || pulse.last() > 0 )
            continue;
        // check for overlapping pulses
        if( maxWidth > 0 && pulse.size() > maxWidth)
            continue;
        n = pulse.max() - threshold;
        if(n > 0){
            // map from 14  bits down to 12 for 4096 channels
            n = ((n >> 2) & 0x0fff);
            // update the spectrum table
            spectrumTbl[n]++;
        }
        totalWidth += pulse.size();
        totalPulses++;
    }
    return TRUE;
}

#if 0

bool AlsaStream::sample(void)
{

    // get data to io
    QString centryMinus1 = "20";
    const int decadeLimit = 2;

    // setting members
    m_LT = liveTIme;    // seconds
    m_RT = realTime;    // seconds

    acqDate = ddMMMyyyy;
    acqTime = hhmm;
    nchs = ALSA_CHANNELS;

    QLocale usdate( QLocale::English, QLocale::UnitedStates );
    m_acqDateTime = usdate.toDateTime(acqDate + " " + acqTime, "ddMMMyyyy hh:mm:ss");
    if ( !m_acqDateTime.isValid() )
        m_acqDateTime.fromString("01Jan2001 00:00:01", "ddMMMyyyy hh:mm:ss");

    ioEnFitType = 0;    // in CHN is always poly ( gr. 2 (3 params) or 1 (2 params) )
    ioEnParms.clear();
    ioEnParms << m_en0 << m_en1 << m_en2;
    if ( m_en2 < 1.0e-30 ) {
        ioEnParms.removeLast();
    }
    ioFwhmFitType = 1;
    ioFwhmParms.clear();
    ioFwhmParms << m_fw0 << m_fw1 << m_fw2;
    if ( m_fw2 < 1.0e-30 ) {
        ioFwhmParms.removeLast();
    }
    ioEfficParms;
    ioEfficEnPts;

    ioSamDesc = QString::fromAscii( samDes, samLen ) + "\n\n" + QString::fromAscii( detDes, detLen );
    return true;
}

#endif
