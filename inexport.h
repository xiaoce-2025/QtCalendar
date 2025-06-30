#ifndef INEXPORT_H
#define INEXPORT_H

#include <QWidget>

namespace Ui {
class InExport;
}

class InExport : public QWidget
{
    Q_OBJECT

public:
    explicit InExport(QWidget *parent = nullptr);
    ~InExport();

private:
    Ui::InExport *ui;
};

#endif // INEXPORT_H
