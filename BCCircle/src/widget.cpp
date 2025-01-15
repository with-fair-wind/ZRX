#include "widget.h"
#include "./ui_form.h"
#include <BCCircle.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, [this]
            { QMessageBox::information(this, "info", "hello world", QMessageBox::Ok); });

    setWindowIcon(QIcon(":/resource/images/safe.png"));

    connect(ui->pushButton_2, &QPushButton::clicked, this, &Widget::on_clicked);
}

void Widget::on_clicked()
{
    test_mycircle();
}

Widget::~Widget()
{
    delete ui;
}
