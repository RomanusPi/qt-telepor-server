#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTextEdit>

MainWindow::MainWindow( QWidget *parent) :
    QMainWindow(parent),ui(new Ui::MainWindow)
{


    ui->setupUi(this);
    nDate = QDate::currentDate();
    ToFile.fileTermOpen();
    ToFile.read_from_file(DateMap);
    ToFile.file_close(ToFile.FileReadDes);
    strDate=QDate::currentDate().toString("yyyy/M/dd");
    ui->label_2->setText(strDate);
    if (DateMap[QDate::currentDate().toString("yyyy/M/dd")].isReserved==false)
    {
        ui->label_3->setText(" Term possible for reservation");
    }
    else ui->label_3->setText("Term was reserved");

    QDir cat;
    FilePath = cat.currentPath();//take current directory

    //set month to arrows
    arrowDate = QDate::currentDate();
    ui->label->setText(QDate::longMonthName(arrowDate.toString("M").toInt()));
    //message
    ui->lineEdit->setText(strDate + " I'm booking" );
    SmsR.smstext = ui->lineEdit->text();
    //database
    DBase.DatabaseConnect(FilePath);
    DBase.DatabaseInit();
    //tcp
    server = new ServerStuff(this);
    connect(server, &ServerStuff::gotNewMesssage,
          this, &MainWindow::gotNewMesssage);
    connect(server->tcpServer, &QTcpServer::newConnection,
          this, &MainWindow::smbConnectedToServer);
    connect(server, &ServerStuff::smbDisconnected,
          this, &MainWindow::smbDisconnectedFromServer);
    //create buttons
MainWindow::createButtons();
  //MainWindow::on_pushButton_server_test_clicked(); //test the server

}


MainWindow::~MainWindow()
{
    //save to file
    ToFile.fileTermOpen();
    ToFile.write_to_file(DateMap);
    ToFile.file_close(ToFile.FileReadDes);
    delete ui;
    delete server;

}

void MainWindow::on_calendarWidget_selectionChanged()
{

   ui->label_4->setText("");
   nDate = ui->calendarWidget->selectedDate();//nDate for selected on calender day
   strDate = nDate.toString("yyyy/M/dd");

   if (!(DateMap.contains(strDate)) || (DateMap[strDate].isReserved == false))
   {
           ui->label_3->setText("Term for reservation");
   }
   else { ui->label_3->setText("Term was reserved");}

   QMapIterator <QString,rdata> Iter(DateMap);
   while(Iter.hasNext()){ Iter.next(); qDebug()<<Iter.key()<<" "<<Iter.value().isReserved;};

   ui->label_2->setText(strDate);

}


void MainWindow::on_pushButton_clicked()
{

    if (DateMap[strDate].isReserved == false)
    {
        DateMap[strDate].isReserved = true;
        ui->label_4->setText("You reserved this date");
    }
    else { ui->label_4->setText("You cannot reserve again");}

}

void MainWindow::on_pushButton_2_clicked()
{

    if (DateMap[strDate].isReserved == true)
    {
      DateMap[strDate].isReserved = false;
      ui->label_4->setText("You canceled this term");
    }
    else { ui->label_4->setText("You don't have a reservation yet");}

}

void MainWindow::on_commandLinkButton_clicked()//button with months
{
    arrowDate = arrowDate.addMonths(1);//one month later
    QString strMonth = arrowDate.toString("M");
    bool ok;
    int k = strMonth.toInt(&ok);

    ui->calendarWidget->setSelectedDate(arrowDate);

    if (ok == false) {qDebug() << "not converted";}
    strMonth = QDate::longMonthName(k);
    ui->label->setText(strMonth);
}
void MainWindow::on_commandLinkButton_2_clicked()
{

    arrowDate = arrowDate.addMonths(-1);//one month earlier (left)
    ui->calendarWidget->setSelectedDate(arrowDate);
    QString strMonth = arrowDate.toString("M");
    bool ok;
    int k = strMonth.toInt(&ok);
    if (ok == false) {qDebug()<<"not converted";}
    strMonth = QDate::longMonthName(k);
    ui->label->setText(strMonth);

}

void MainWindow::on_pushButton_4_clicked()
{

 SmsR.smstext=ui->lineEdit->text();
 QString strDate = SmsR.smstext.left(9);
 QString Rezor = SmsR.smstext.mid(10,17);
 if(Rezor =="I'm booking")
 {
     if (DateMap[strDate].isReserved == false)
     {
         DateMap[strDate].isReserved = true;
         ui->label_4->setText("You reserved the term");
     }
     else { ui->label_4->setText("<font color=\"red\">You cannot re-book</font>");}
  }
 if(Rezor=="cancel")
 {
     if (DateMap[strDate].isReserved == true)
     {
       DateMap[strDate].isReserved = false;
       ui->label_4->setText("You canceled this term");
     }
     else { ui->label_4->setText("You don't have a reservation yet ");}

 }
 qDebug()<<strDate<<Rezor;
}

void MainWindow::on_save_to_file_clicked()
{
        //write to file
        ToFile.fileTermOpen();
        ToFile.write_to_file(DateMap);
        ToFile.file_close(ToFile.FileReadDes);
        ui->label_4->setText("Data has been written to a file");
}

void MainWindow::on_read_from_file_clicked()
{
    //read data saved in file
    ToFile.fileTermOpen();
    ToFile.read_from_file(DateMap);
    ToFile.file_close(ToFile.FileReadDes);
    ui->label_4->setText("Data has been read from the file ");
}

void MainWindow::on_pushButton_save_to_bd_clicked()
{
    DBase.Databasewrite(DateMap);
    ui->label_4->setText("Saved into the database");
}

void MainWindow::on_pushButton_read_from_bd_clicked()
{
    DBase.Databaseread(DateMap);
    ui->label_4->setText("");
}

//tcp server
void MainWindow::smbConnectedToServer()
{
    ui->textEdit_log->append(tr("Somebody has connected"));
}

void MainWindow::smbDisconnectedFromServer()
{
    ui->textEdit_log->append(tr("Somebody has disconnected"));
}

void MainWindow::gotNewMesssage(QString Msgc,QTcpSocket *clientSocket)
{

   QString Order = "", Date = "";
   bool NextInfo = false;
   for(int i = 0;i < Msgc.length();i++)
   {
        if (Msgc[i] != '@' && !NextInfo)
        {
           Order += Msgc[i];
           continue;
        }
        if (Msgc[i] == '@')
        {
            NextInfo = true;
            continue;
        }

        Date += Msgc[i];

    }
   qDebug()<<Order<<Date;
   //server->sendToClient(clientSocket, QString("received msg "));

   if(Order == "isfree"){
   //check the date is valid and exist

       if (QDate::fromString(Date,"yyyy/M/dd").isValid())
       {

           if (DateMap[Date].isReserved)
                 server->sendToClient(clientSocket, QString("isreserved@[%1]").arg(Date)) ;
           else  server->sendToClient(clientSocket, QString("notreserved@[%1]").arg(Date)) ;
       }
       else{
             if (server->sendToClient(clientSocket, QString("not walid date!: [%1]").arg(Date)) == -1)
           {
            qDebug() << "some error occured";
           }
       }

   }

   if(Order == "reserve"){
       //check the date is valid and exist
       if (QDate::fromString(Date,"yyyy/M/dd").isValid())
       {

           if (!DateMap[Date].isReserved)
           {
                 server->sendToClient(clientSocket, QString("Ireserved@[%1]").arg(Date));
                 DateMap[Date].isReserved = true;//make reservation
           }
           else  server->sendToClient(clientSocket, QString("Inotreserved@[%1]").arg(Date));
       }
       else{
           if (server->sendToClient(clientSocket, QString("not walid date!: [%1]").arg(Date)) == -1)
           {
               qDebug() << "some error occured";
           }
       }
   }

   if(Order == "release")
   {
       //check whether the date is valid and exist
       if (QDate::fromString(Date,"yyyy/M/dd").isValid())
       {
           if (DateMap[Date].isReserved)
           {
               server->sendToClient(clientSocket, QString("isreleased@[%1]").arg(Date)) ;
               DateMap[Date].isReserved = false;//make release
           }
           else  server->sendToClient(clientSocket, QString("isnotreleased@[%1]").arg(Date)) ;
       }
       else
       {
          if (server->sendToClient(clientSocket, QString("not walid date!: [%1]").arg(Date)) == -1)
            {
                qDebug() << "some error occured";
            }
       }

   }
    ui->textEdit_log->append(QString("New message: %1").arg(Msgc));
}


void MainWindow::on_pushButton_server_on_2_clicked()
{
    if (!server->tcpServer->listen(QHostAddress::Any, 6547))
    {
        ui->textEdit_log->append(tr("<font color=\"red\"><b>Error!</b> The port is taken by some other service.</font>"));
        return;
    }
    connect(server->tcpServer, &QTcpServer::newConnection, server, &ServerStuff::newConnection);
    ui->textEdit_log->append(tr("<font color=\"green\"><b>Server started</b>, port is openned.</font>"));
}

void MainWindow::on_pushButton_server_off_clicked()
{
    if(server->tcpServer->isListening())
    {
        disconnect(server->tcpServer, &QTcpServer::newConnection, server, &ServerStuff::newConnection);

        QList<QTcpSocket *> clients = server->getClients();
        for(int i = 0; i < clients.count(); i++)
        {
            //server->sendToClient(clients.at(i), "Connection closed");
            server->sendToClient(clients.at(i), "0");
        }

        server->tcpServer->close();
        ui->textEdit_log->append(tr("<b>Server stopped</b>, post is closed"));
    }
    else
    {
        ui->textEdit_log->append(tr("<b>Error!</b> Server was not running"));
    }
}

void MainWindow::on_pushButton_server_test_clicked()
{
    if(server->tcpServer->isListening())
    {
        ui->textEdit_log->append(
                    QString("%1 %2")
                    .arg("Server is listening, number of connected clients:")
                    .arg(QString::number(server->getClients().count()))
                    );
    }
    else
    {
        ui->textEdit_log->append(
                    QString("%1 %2")
                    .arg("Server is not listening, number of connected clients:")
                    .arg(QString::number(server->getClients().count()))
                    );
    }
}


//----

void MainWindow::createButtons(){//used in the future
    qDebug()<<"create";

    QVBoxLayout *layout = new QVBoxLayout();
    QString buttontext;
    const int howmanybuttons = 8;
    QPushButton *timeButton[howmanybuttons];
    QTime timetoReserve = QTime::fromString("15:00","hh:mm");


    for (int i = 0; i<howmanybuttons; i += 2)
    {

       layout = ui->verticalLayout;
       buttontext = timetoReserve.toString("hh:mm");//"<font color=\"red\">DISCONNECTED</font>"
       timeButton[i] = new QPushButton(buttontext, ui->frame_2);
       layout->insertWidget(i/2,timeButton[i]);
       timeButton[i]->show();
       QObject::connect( timeButton[i], &QPushButton::clicked,this,&MainWindow::onRemoveWidget);
       timetoReserve = timetoReserve.addSecs(60 * 60);

       //create next button column
       layout = ui->verticalLayout_2;//choose layout
       buttontext = timetoReserve.toString("hh:mm");
       timeButton[i] = new QPushButton(buttontext, ui->frame_2);
       layout->insertWidget(i/2, timeButton[i]);//insert button
       timeButton[i]->show();
       QObject::connect(timeButton[i], &QPushButton::clicked,this,&MainWindow::onRemoveWidget);
       timetoReserve = timetoReserve.addSecs(60 * 60);

    }
}
void MainWindow::onRemoveWidget(){//use in the future

    QPushButton* przyc =  qobject_cast<QPushButton*>(sender());
    przyc->setStyleSheet("background-color: darkRed");
    przyc->setText("booked");
    delete przyc;

}


