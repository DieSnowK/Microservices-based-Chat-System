#ifndef SOUNDRECORDER_H
#define SOUNDRECORDER_H

#include <QObject>
#include <QStandardPaths>
#include <QFile>
#include <QAudioSource>
#include <QAudioSink>
#include <QMediaDevices>

class SoundRecorder : public QObject
{
    Q_OBJECT
public:
    const QString RECORD_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sound/tmpRecord.pcm";
    const QString PLAY_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/sound/tmpPlay.pcm";

public:
    static SoundRecorder* GetInstance();

    void StartRecord();
    void StopRecord();

    void StartPlay(const QByteArray& content);
    void StopPlay();

private:
    static SoundRecorder* instance;
    explicit SoundRecorder(QObject *parent = nullptr);

    QFile soundFile;
    QAudioSource *audioSource;

    QAudioSink *audioSink;
    QMediaDevices *outputDevices;
    QAudioDevice outputDevice;
    QFile inputFile;

signals:
    void SoundRecordDone(const QString& path);
    void SoundPlayDone();
};

#endif // SOUNDRECORDER_H
