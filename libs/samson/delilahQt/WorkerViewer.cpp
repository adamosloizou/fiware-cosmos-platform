/* ****************************************************************************
*
* FILE                     WorkerViewer.cpp
*
* DESCRIPTION			   Widget to visualize samson workers' data
*
*/

#include "WorkerViewer.h"

#include "au/string.h"

#include <iostream>
#include <stdlib.h>

namespace samson{

WorkerData::WorkerData()
{
}

bool WorkerData::operator==(const WorkerData &other) const 
{
    if(worker_id != other.worker_id) return false;
    if(type != other.type) return false;
    if(mem_used != other.mem_used) return false;
    if(mem_total != other.mem_total) return false;
    if(cores_used != other.cores_used) return false;
    if(cores_total != other.cores_total) return false;
    if(disk_ops != other.disk_ops) return false;
    if(disk_in_rate != other.disk_in_rate) return false;
    if(disk_out_rate != other.disk_out_rate) return false;
    if(net_in != other.net_in) return false;
    if(net_out != other.net_out) return false;

    return true;
}


WorkerViewer::WorkerViewer(std::string _title, QWidget* parent): QWidget(parent)
{
    //QVBoxLayout* layout;
    
    worker_id = _title;
    data.worker_id = _title;
    
    groupBox = new QGroupBox(QString(("Worker " + worker_id).c_str()), this);
    mainLayout = new QGridLayout;
    line1Layout = new QHBoxLayout;
    line2Layout = new QHBoxLayout;
    line3Layout = new QHBoxLayout;
    line4Layout = new QHBoxLayout;

    
    QFont bigFont;
    QFont boldFont;
    worker_idLabel = new QLabel("Worker id:", this);
    //boldFont = worker_idLabel->font();
    //boldFont.setBold(true);

    worker_idValue = new QLabel(worker_id.c_str(), this);
    //worker_idValue->setFont(boldFont);
    worker_idValue->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    typeLabel = new QLabel("Type:", groupBox);
    type = new QLabel("", groupBox);
    type->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    mem_usedLabel = new QLabel("Used memory:", groupBox);
    mem_used = new QLabel("", groupBox);
    mem_used->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    coresLabel = new QLabel("Cores Used/Total:", groupBox);
    cores = new QLabel("0/0", groupBox);
    cores->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    
    disk_opsLabel = new QLabel("Disk Operations:", groupBox);
    disk_ops = new QLabel("0", groupBox);
    disk_ops->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    disk_in_rateLabel = new QLabel("Disk input(B/s):", groupBox);
    disk_in_rate = new QLabel("0", groupBox);
    disk_in_rate->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    
    disk_out_rateLabel = new QLabel("Disk output(B/s):", groupBox);
    disk_out_rate = new QLabel("0", groupBox);
    disk_out_rate->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    net_in_rateLabel = new QLabel("Network input(B/s):", groupBox);
    net_in_rate = new QLabel("0", groupBox);
    net_in_rate->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    net_out_rateLabel = new QLabel("Network output(B/s):", groupBox);
    net_out_rate = new QLabel("0", groupBox);
    net_out_rate->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    mainLayout->addWidget(worker_idLabel, 0, 0, Qt::AlignRight);
    mainLayout->addWidget(worker_idValue, 0, 1);
    mainLayout->addWidget(typeLabel, 0, 3, Qt::AlignRight);
    mainLayout->addWidget(type, 0, 4);
    mainLayout->addWidget(mem_usedLabel, 1, 0, Qt::AlignRight);
    mainLayout->addWidget(mem_used, 1, 1);
    mainLayout->addWidget(coresLabel, 1, 3, Qt::AlignRight);
    mainLayout->addWidget(cores, 1, 4);
    mainLayout->addWidget(disk_opsLabel, 2, 0, Qt::AlignRight);
    mainLayout->addWidget(disk_ops, 2, 1);
    mainLayout->addWidget(disk_in_rateLabel, 2, 3, Qt::AlignRight);
    mainLayout->addWidget(disk_in_rate, 2, 4);
    mainLayout->addWidget(disk_out_rateLabel, 2, 6, Qt::AlignRight);
    mainLayout->addWidget(disk_out_rate, 2, 7);
    mainLayout->addWidget(net_in_rateLabel, 3, 0,  Qt::AlignRight);
    mainLayout->addWidget(net_in_rate, 3, 1);
    mainLayout->addWidget(net_out_rateLabel, 3, 3, Qt::AlignRight);
    mainLayout->addWidget(net_out_rate, 3, 4);

    groupBox->setLayout(mainLayout);
  
}

void WorkerViewer::setData(WorkerData newData)
{
    data = newData;
    worker_idValue->setText(QString(data.worker_id.c_str()));
    type->setText(QString(data.type.c_str()));
    mem_used->setText(QString(au::str(strtoul(data.mem_used.c_str(), NULL, 0)).c_str()));
    std::string coresString(au::str(strtoul(data.cores_used.c_str(), NULL, 0)) + "/" + au::str(strtoul(data.cores_total.c_str(), NULL, 0)));
    cores->setText(QString(coresString.c_str()));
    disk_ops->setText(QString(au::str(strtoul(data.disk_ops.c_str(), NULL, 0)).c_str()));
    disk_in_rate->setText(QString(au::str(strtoul(data.disk_in_rate.c_str(), NULL, 0)).c_str()));
    disk_out_rate->setText(QString(au::str(strtoul(data.disk_out_rate.c_str(), NULL, 0)).c_str()));
    net_in_rate->setText(QString(au::str(strtoul(data.net_in.c_str(), NULL, 0)).c_str()));
    net_out_rate->setText(QString(au::str(strtoul(data.net_out.c_str(), NULL, 0)).c_str()));
}

void WorkerViewer::setTitle(std::string title)
{
    if (groupBox) groupBox->setTitle(QString(title.c_str()));
}


} //namespace
