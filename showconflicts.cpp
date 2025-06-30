#include "showconflicts.h"
#include "ui_showconflicts.h"

ShowConflicts::ShowConflicts(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShowConflicts)
{
    ui->setupUi(this);
}

ShowConflicts::~ShowConflicts()
{
    delete ui;
}
