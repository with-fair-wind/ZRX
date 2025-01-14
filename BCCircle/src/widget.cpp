#include "widget.h"
#include "./ui_form.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, [this]
            { QMessageBox::information(this, "info", "hello world", QMessageBox::Ok); });

    setWindowIcon(QIcon(":/resource/images/safe.png"));
}

Widget::~Widget()
{
    delete ui;
}
