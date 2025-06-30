#include "inexport.h"
#include "ui_inexport.h"

InExport::InExport(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InExport)
{
    ui->setupUi(this);
}

InExport::~InExport()
{
    delete ui;
}
