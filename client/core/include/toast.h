#ifndef TOAST_H
#define TOAST_H

#include <QDialog>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>

class Toast : public QDialog
{
    Q_OBJECT

public:
    // The parent window of the global notification is the desktop
        // So don't need to specify a parent window
    Toast(const QString& text);

    static void ShowMessage(const QString& text);
};

#endif // TOAST_H
