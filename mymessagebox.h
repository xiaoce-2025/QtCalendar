#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>

class MyMessageBox : public QMessageBox
{
    Q_OBJECT
public:
    explicit MyMessageBox(QWidget *parent = nullptr,
                              const QString &title = "提示",
                              const QString &text = "",
                              Icon icon = Information,
                              StandardButtons buttons = Ok);

    static void info(QWidget *parent, const QString& text);
    static void warning(QWidget *parent, const QString& text);

protected:
    void setupUI();
    void applyStyleSheet();

};

#endif // MYMESSAGEBOX_H
