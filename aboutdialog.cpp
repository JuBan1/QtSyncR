#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QStyle>
#include <QDebug>

#include "globals.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    const auto size = ui->lbIcon->size();
    qDebug() << size;
    ui->lbIcon->setPixmap( style()->standardIcon(QStyle::SP_MessageBoxInformation).pixmap(size) );

    ui->lbText->setText(QString(
                            "<b><h1>QtSyncR</h1></b>"
                            "v%3 - <a href='%1'>Visit Website</a><br/><br/><br/>"
                            "Copyright @ %2, Julian Bansen").arg(GITHUB_ADDRESS).arg(COPYRIGHT_YEAR).arg(VERSION)
                );

    ui->lbText->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
