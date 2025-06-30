#ifndef SHOWCONFLICTS_H
#define SHOWCONFLICTS_H

#include <QWidget>

namespace Ui {
class ShowConflicts;
}

class ShowConflicts : public QWidget
{
    Q_OBJECT

public:
    explicit ShowConflicts(QWidget *parent = nullptr);
    ~ShowConflicts();

private:
    Ui::ShowConflicts *ui;
};

#endif // SHOWCONFLICTS_H
