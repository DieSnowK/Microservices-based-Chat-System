#include <QDir>
#include <QMediaDevices>

#include "sound_recorder.h"
#include "data.hpp"
#include "toast.h"

SoundRecorder *SoundRecorder::instance = nullptr;
SoundRecorder *SoundRecorder::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new SoundRecorder();
    }

    return instance;
}

// Refer to: https://www.cnblogs.com/tony-yang-flutter/p/16477212.html
//           https://doc.qt.io/qt-6/qaudiosource.html
SoundRecorder::SoundRecorder(QObject *parent)
    : QObject{parent}
{
    // 1.Create directory
    QDir soundRootPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    soundRootPath.mkdir("sound");

    // 2.Initialize the recording module
    soundFile.setFileName(RECORD_PATH);

    QAudioFormat inputFormat;
    inputFormat.setSampleRate(16000);
    inputFormat.setChannelCount(1);
    inputFormat.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice info = QMediaDevices::defaultAudioInput();
    if (!info.isFormatSupported(inputFormat))
    {
        LOG() << "Recording device, format not supported";
        return;
    }

    audioSource = new QAudioSource(inputFormat, this);
    connect(audioSource, &QAudioSource::stateChanged, this, [=](QtAudio::State state)
    {
        if (state == QtAudio::StoppedState)
        {
            if (audioSource->error() != QAudio::NoError)
            {
                LOG() << audioSource->error();
            }
        }
    });

    // 3.Initialize the playback module
    outputDevices = new QMediaDevices(this);
    outputDevice = outputDevices->defaultAudioOutput();

    QAudioFormat outputFormat;
    outputFormat.setSampleRate(16000);
    outputFormat.setChannelCount(1);
    outputFormat.setSampleFormat(QAudioFormat::Int16);

    if (!outputDevice.isFormatSupported(outputFormat))
    {
        LOG() << "Playback device, format not supported";
        return;
    }

    audioSink = new QAudioSink(outputDevice, outputFormat);
    connect(audioSink, &QAudioSink::stateChanged, this, [=](QtAudio::State state)
    {
        if (state == QtAudio::IdleState)
        {
            LOG() << "IdleState";
            this->StopPlay();
            emit this->SoundPlayDone();
        }
        else if (state == QAudio::ActiveState)
        {
            LOG() << "ActiveState";
        }
        else if (state == QAudio::StoppedState)
        {
            LOG() << "StoppedState";
            if (audioSink->error() != QtAudio::NoError)
            {
                LOG() << audioSink->error();
            }
        }
    });
}

void SoundRecorder::StartRecord()
{
    soundFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    audioSource->start(&soundFile);
}

void SoundRecorder::StopRecord()
{
    audioSource->stop();
    soundFile.close();
    emit this->SoundRecordDone(RECORD_PATH);
}

void SoundRecorder::StartPlay(const QByteArray& content)
{
    if (content.isEmpty())
    {
        Toast::ShowMessage("Data is loading, please play later");
        return;
    }

    // 1.Write data to temporary file
    model::Util::WriteByteArrayToFile(PLAY_PATH, content);

    // 2.Play voice
    inputFile.setFileName(PLAY_PATH);
    inputFile.open(QIODevice::ReadOnly);
    audioSink->start(&inputFile);
}

void SoundRecorder::StopPlay()
{
    audioSink->stop();
    inputFile.close();
}
