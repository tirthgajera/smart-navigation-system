#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <queue>
#include <climits>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->fuelCostLabel->setText("Fuel Cost : ₹0");
    ui->timeLabel->setText("Travel Time : 0 hrs");
    ui->vehicleComboBox->addItem("Bike");
    ui->vehicleComboBox->addItem("Car");
    ui->vehicleComboBox->addItem("Bus");
    ui->trafficComboBox->addItem("Normal");
    ui->trafficComboBox->addItem("Moderate");
    ui->trafficComboBox->addItem("Heavy");
    ui->stackedWidget->setCurrentIndex(0);
    ui->fuelPriceLineEdit->setText("95");
    scene = new QGraphicsScene(this);
    animationTimer = new QTimer(this);
    bfsTimer = new QTimer(this);
    setWindowState(Qt::WindowMaximized);
    connect(bfsTimer,
            &QTimer::timeout,
            this,
            [=]()
            {
                drawBFSLevel();
            });
    connect(animationTimer,
            &QTimer::timeout,
            this,
            [=]()
            {
                if(animationIndex >= currentPath.size())
                {
                    animationTimer->stop();
                    QTimer::singleShot(1500, this, [this]()
                                       {
                                           restoreRoute();
                                       });
                }

                std::vector<int> temp(
                    currentPath.begin(),
                    currentPath.begin() + animationIndex + 1);

                drawMap(
                    temp,
                    currentPath.front(),
                    currentPath.back());

                animationIndex++;
            });
    ui->routeGraphicsView->setScene(scene);
    ui->routeGraphicsView->setRenderHint(QPainter::Antialiasing);
    ui->routeGraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->routeGraphicsView->setTransformationAnchor(
        QGraphicsView::AnchorUnderMouse);
    scene->setSceneRect(0,0,400,320);

    ui->routeGraphicsView->setRenderHint(QPainter::Antialiasing);

    graphManager.initializeGraph();
    cityPosition[0] = QPointF(30,180);    // Chandigarh
    cityPosition[1] = QPointF(90,120);    // Shimla
    cityPosition[2] = QPointF(170,120);   // Narkanda
    cityPosition[3] = QPointF(240,180);   // Rampur
    cityPosition[4] = QPointF(310,120);   // Kalpa
    cityPosition[5] = QPointF(360,180);   // Kaza
    cityPosition[6] = QPointF(170,280);   // Manali
    ui->viaComboBox->addItem("None");
    for(auto &city : graphManager.getCities())
    {
        ui->sourcecomboBox->addItem(city);
        ui->destinationCombobox->addItem(city);
        ui->viaComboBox->addItem(city);
    }

    // connect(ui->findroutebutton,
    //         &QPushButton::clicked,
    //         this,
    //         &MainWindow::on_findroutebutton_clicked);

    connect(ui->calculateFuelButton,
            &QPushButton::clicked,
            this,
            &MainWindow::on_calculateFuelButton_clicked);

    connect(ui->calculateTimeButton,
            &QPushButton::clicked,
            this,
            &MainWindow::on_calculateTimeButton_clicked);
    connect(ui->backButton,
            &QPushButton::clicked,
            this,
            &MainWindow::on_backButton_clicked);
    // connect(ui->mstButton,
    //         &QPushButton::clicked,
    //         this,
    //         &MainWindow::on_mstButton_clicked);
    this->setWindowTitle("Smart Navigation System");
    ui->Distance->setStyleSheet(
        "color:#009933;"
        "font-size:28px;"
        "font-weight:900;"
        );
    ui->fuelCostLabel->setStyleSheet(
        "font-size:22px;"
        "font-weight:bold;"
        "color:#E65100;"
        );
    ui->timeLabel->setStyleSheet(
        "font-size:22px;"
        "font-weight:bold;"
        "color:#1565C0;"
        );
    ui->recentListWidget->setStyleSheet(
        "QListWidget {"
        "background:white;"
        "border:2px solid #1976D2;"
        "border-radius:10px;"
        "font-size:14px;"
        "padding:5px;"
        "}"
        "QListWidget::item {"
        "padding:6px;"
        "}"
        "QListWidget::item:selected {"
        "background:#BBDEFB;"
        "color:black;"
        "}"
        );
    QStringList defaults = {
        "Chandigarh → Shimla",
        "Shimla → Narkanda",
        "Chandigarh → Manali",
        "Kalpa → Kaza",
        "Kaza → manali"
    };
    connect(ui->recentListWidget,
            &QListWidget::itemClicked,
            this,
            &MainWindow::on_recentListWidget_itemClicked);
    connect(ui->dfsButton,
            &QPushButton::clicked,
            this,
            &MainWindow::on_dfsButton_clicked);
    connect(ui->bfsButton,
            &QPushButton::clicked,
            this,
            &MainWindow::on_bfsButton_clicked);
    for(const QString &s : defaults)
    {
        recentSearches.push(s);
        ui->recentListWidget->addItem(s);
    }
    this->setStyleSheet(
        "QGroupBox{"
        "border:2px solid #4A90E2;"
        "border-radius:15px;"
        "margin-top:12px;"
        "font-size:16px;"
        "font-weight:bold;"
        "color:#1F3A93;"
        "background:white;"
        "}"

        "QGroupBox::title{"
        "subcontrol-origin:margin;"
        "left:15px;"
        "padding:0 8px;"
        "background:white;"
        "}"

        "QPushButton {"
        "background-color:#1976D2;"
        "color:white;"
        "border-radius:10px;"
        "padding:8px;"
        "font-weight:bold;"
        "}"

        "QPushButton:hover {"
        "background-color:#1565C0;"
        "}"

        "QLineEdit,QComboBox,QTextEdit {"
        "border:1px solid #cccccc;"
        "border-radius:8px;"
        "padding:5px;"
        "}"
        "QMainWindow {"
        "background-color:#f4f6f9;"
        "}"

        "QPushButton {"
        "background-color:#1976D2;"
        "color:white;"
        "font-size:10px;"
        "font-weight:bold;"
        "padding:8px;"
        "border-radius:6px;"
        "}"

        "QPushButton:hover {"
        "background-color:#1565C0;"
        "}"

        "QComboBox {"
        "padding:5px;"
        "font-size:13px;"
        "}"

        "QTextEdit {"
        "background:white;"
        "font-size:13px;"
        "}"
        );

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_findroutebutton_clicked()
{
    scene->clear();
    animationTimer->stop();
    currentPath.clear();
    mstEdges.clear();
    animationIndex = 0;
    int src = ui->sourcecomboBox->currentIndex();
    int dest = ui->destinationCombobox->currentIndex();
    int via = ui->viaComboBox->currentIndex() - 1;
    finalWaypoint = via;
    if(src == dest)
    {
        QMessageBox::warning(
            this,
            "Invalid Route",
            "Source and Destination cannot be same."
            );
        return;
    }

    vector<int> path;
    int totalDistance;

    if(via == -1)
    {
        if(!graphManager.shortestPath(src,dest,path,totalDistance))
        {
            QMessageBox::warning(
                this,
                "Error",
                "No Route Found!"
                );
            return;
        }
    }
    else
    {
        vector<int> path1,path2;
        int d1,d2;

        if(!graphManager.shortestPath(src,via,path1,d1))
        {
            QMessageBox::warning(
                this,
                "Error",
                "No Route to Waypoint!"
                );
            return;
        }

        if(!graphManager.shortestPath(via,dest,path2,d2))
        {
            QMessageBox::warning(
                this,
                "Error",
                "No Route from Waypoint!"
                );
            return;
        }

        path2.erase(path2.begin());

        path = path1;

        path.insert(
            path.end(),
            path2.begin(),
            path2.end()
            );

        totalDistance = d1 + d2;
    }

    QString route;

    for(size_t i=0;i<path.size();i++)
    {
        route += graphManager.getCities()[path[i]];

        if(i+1<path.size())
            route += "\n↓\n";
    }

    ui->Distance->setText(
        "Distance : "
        + QString::number(totalDistance)
        + " km"
        );
    scene->clear();
    animationTimer->stop();
    currentPath = path;
    shortestPath = path;
    animationIndex = 0;

    animationTimer->start(500);
    // ui->routeGraphicsView->fitInView(
    //     scene->itemsBoundingRect(),
    //     Qt::KeepAspectRatio
    //     );
    // ui->routeGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // ui->routeGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    finalDistance = totalDistance;
    finalRoute = route;
    QString search =
        ui->sourcecomboBox->currentText();

    if(via != -1)
    {
        search += " → " + ui->viaComboBox->currentText();
    }

    search += " → " + ui->destinationCombobox->currentText();

    recentSearches.push(search);
    if(recentSearches.size() > 5)
    {
        recentSearches.pop();
    }

    ui->recentListWidget->clear();

    std::queue<QString> temp = recentSearches;
    std::vector<QString> history;

    while(!temp.empty())
    {
        history.push_back(temp.front());
        temp.pop();
    }

    for(int i = history.size() - 1; i >= 0; i--)
    {
        ui->recentListWidget->addItem(history[i]);
    }
    QGraphicsOpacityEffect *effect =
        new QGraphicsOpacityEffect(ui->stackedWidget);

    ui->stackedWidget->setGraphicsEffect(effect);

    QPropertyAnimation *anim =
        new QPropertyAnimation(effect,"opacity");

    anim->setDuration(600);
    anim->setStartValue(0);
    anim->setEndValue(1);

    ui->stackedWidget->setCurrentIndex(1);

    anim->start();
}
void MainWindow::on_saveTripButton_clicked()
{
    if(tripManager.saveTrip(
            ui->sourcecomboBox->currentText(),
            ui->destinationCombobox->currentText(),
            ui->Distance->text(),
            ui->mileageLineEdit->text(),
            ui->fuelPriceLineEdit->text(),
            ui->fuelCostLabel->text(),
            ui->timeLabel->text(),
            finalRoute))
    {
        QMessageBox::information(this,"Success","Trip Saved Successfully!");
    }
}
void MainWindow::on_calculateFuelButton_clicked()
{
    double mileage =
        ui->mileageLineEdit->text().toDouble();

    double fuelPrice =
        ui->fuelPriceLineEdit->text().toDouble();

    if(mileage <= 0)
        return;

    double fuelNeeded =
        (double)finalDistance / mileage;

    double fuelCost =
        fuelNeeded * fuelPrice;

    ui->fuelCostLabel->setText(
        "Fuel Cost : ₹" +
        QString::number(fuelCost,'f',2)
        );
}
void MainWindow::on_calculateTimeButton_clicked()
{
    double speed;

    QString vehicle =
        ui->vehicleComboBox->currentText();

    if(vehicle == "Bike")
        speed = 50;
    else if(vehicle == "Car")
        speed = 70;
    else
        speed = 40;

    QString traffic =
        ui->trafficComboBox->currentText();

    double factor = 1.0;

    if(traffic == "Moderate")
        factor = 1.15;
    else if(traffic == "Heavy")
        factor = 1.30;

    double time =
        (finalDistance * factor) / speed;

    ui->timeLabel->setText(
        "Travel Time : " +
        QString::number(time,'f',2) +
        " hrs"
        );
}
void MainWindow::drawMap(const std::vector<int>& path, int src, int dest)
{
    scene->clear();
    auto isHighlightedEdge = [&](int u,int v)
    {
        // Route edges
        for(size_t i=0;i+1<path.size();i++)
        {
            if((path[i]==u && path[i+1]==v) ||
                (path[i]==v && path[i+1]==u))
                return true;
        }

        // MST edges
        for(auto e : mstEdges)
        {
            if((e.first==u && e.second==v) ||
                (e.first==v && e.second==u))
                return true;
        }

        return false;
    };

    // auto isHighlightedEdge = [&](int u, int v)
    // {
    //     for(size_t i = 0; i + 1 < path.size(); i++)
    //     {
    //         if((path[i] == u && path[i+1] == v) ||
    //             (path[i] == v && path[i+1] == u))
    //         {
    //             return true;
    //         }
    //     }
    //     return false;
    // };
    // Draw all roads
    for(int u = 0; u < graphManager.getGraph().size(); u++)
    {
        for(auto edge : graphManager.getGraph()[u])
        {
            int v = edge.first;

            // Avoid drawing the same road twice
            if(u < v)
            {
                QPen pen;

                if(isHighlightedEdge(u,v))
                {
                    pen = QPen(QColor(0,120,255),4);
                    pen.setCapStyle(Qt::RoundCap);
                    pen.setJoinStyle(Qt::RoundJoin);
                }
                else
                {
                    pen = QPen(Qt::lightGray,1);
                }

                scene->addLine(
                    cityPosition[u].x(),
                    cityPosition[u].y(),
                    cityPosition[v].x(),
                    cityPosition[v].y(),
                    pen
                    );

                // Distance label
                int midX = (cityPosition[u].x() + cityPosition[v].x()) / 2;
                int midY = (cityPosition[u].y() + cityPosition[v].y()) / 2;

                QGraphicsTextItem *distance =
                    scene->addText(QString::number(edge.second));

                distance->setDefaultTextColor(Qt::darkGreen);

                QFont font;
                font.setPointSize(8);
                font.setBold(true);

                distance->setFont(font);

                distance->setPos(midX-5, midY-25);
            }
        }
    }

    // Draw all cities
    for(int i = 0; i < graphManager.getCities().size(); i++)
    {
        QPointF p = cityPosition[i];

        QBrush brush(QColor(70,130,255));

        if(i == src)
            brush=QBrush(QColor(0,220,0));
        else if(i==finalWaypoint)
            brush=QBrush(QColor(255,170,0));
        else if(i == dest)
            brush = QBrush(QColor(255,50,50));

        scene->addEllipse(
            p.x()-6,
            p.y()-6,
            12,
            12,
            QPen(Qt::black),
            brush
            );
        QGraphicsTextItem *cityName =
            scene->addText(graphManager.getCities()[i]);
        QFont font;
        font.setBold(true);
        font.setPointSize(8);

        cityName->setFont(font);
        cityName->setDefaultTextColor(Qt::black);
        cityName->setPos(p.x()-20,p.y()+8);
    }
}

void MainWindow::on_backButton_clicked()
{
    animationTimer->stop();

    scene->clear();

    currentPath.clear();

    mstEdges.clear();

    animationIndex = 0;

    ui->Distance->setText("Distance : 0 km");
    ui->fuelCostLabel->setText("Fuel Cost : ₹0");
    ui->timeLabel->setText("Travel Time : 0 hrs");

    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_recentListWidget_itemClicked(QListWidgetItem *item)
{
    QString text = item->text();

    QStringList parts = text.split(" → ");

    if(parts.size() == 2)
    {
        ui->sourcecomboBox->setCurrentText(parts[0]);
        ui->destinationCombobox->setCurrentText(parts[1]);
        ui->viaComboBox->setCurrentIndex(0);   // None
    }
    else if(parts.size() == 3)
    {
        ui->sourcecomboBox->setCurrentText(parts[0]);
        ui->viaComboBox->setCurrentText(parts[1]);
        ui->destinationCombobox->setCurrentText(parts[2]);
    }
}
// void MainWindow::DFS(int node)(
// {
//     visitedDFS[node] = true;

//     dfsOrder.push_back(node);

//     for(auto edge : graph[node])
//     {
//         int next = edge.first;

//         if(!visitedDFS[next])
//         {
//             graphManager.DFS(next);
//         }
//     }
// }
 void MainWindow::on_dfsButton_clicked()
 {
     dfsOrder.clear();

     int start = ui->sourcecomboBox->currentIndex();

     graphManager.DFS(start, dfsOrder);

     currentPath = dfsOrder;

     animationIndex = 0;

     animationTimer->start(600);
}
// int MainWindow::Find(int x)
// {
//     if(parentDSU[x]==x)
//         return x;

//     return parentDSU[x]=Find(parentDSU[x]);


// void MainWindow::Union(int a,int b)
// {
//     a=Find(a);
//     b=Find(b);

//     if(a==b)
//         return;

//     if(rankDSU[a]<rankDSU[b])
//         swap(a,b);

//     parentDSU[b]=a;

//     if(rankDSU[a]==rankDSU[b])
//         rankDSU[a]++;
// }
void MainWindow::on_mstButton_clicked()
{
    animationTimer->stop();
    bfsTimer->stop();

    mstEdges.clear();

    int totalCost = graphManager.MST(mstEdges);

    drawMap({},-1,-1);

    QString result = "Minimum Spanning Tree\n\n";

    for(auto edge : mstEdges)
    {
        int weight = 0;

        for(auto e : graphManager.getGraph()[edge.first])
        {
            if(e.first == edge.second)
            {
                weight = e.second;
                break;
            }
        }

        result += graphManager.getCities()[edge.first]
                  + " ↔ "
                  + graphManager.getCities()[edge.second]
                  + " ("
                  + QString::number(weight)
                  + " km)\n";
    }

    result += "\n----------------------\n";
    result += "Total Cost : " + QString::number(totalCost) + " km";

    QMessageBox::information(this,"Minimum Spanning Tree",result);

    mstEdges.clear();

    QTimer::singleShot(300, this, [this]()
                       {
                           restoreRoute();
                       });
}
void MainWindow::on_exportPdfButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Trip Report",
        "TripReport.pdf",
        "PDF Files (*.pdf)");

    if(fileName.isEmpty())
        return;

    QPdfWriter pdf(fileName);

    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setResolution(300);

    QPainter painter(&pdf);

    QFont titleFont("Arial",18,QFont::Bold);
    painter.setFont(titleFont);

    painter.drawText(200,150,"SMART NAVIGATION SYSTEM");

    QFont heading("Arial",13,QFont::Bold);
    painter.setFont(heading);

    int y=300;

    painter.drawText(100,y,"Trip Report");
    y+=80;

    QFont normal("Arial",11);

    painter.setFont(normal);

    painter.drawText(100,y,
                     "Date : "
                         +QDateTime::currentDateTime().toString());

    y+=60;

    painter.drawText(100,y,
                     "Source : "
                         +ui->sourcecomboBox->currentText());

    y+=60;

    painter.drawText(100,y,
                     "Destination : "
                         +ui->destinationCombobox->currentText());

    y+=60;

    if(finalWaypoint!=-1)
    {
        painter.drawText(100,y,
                         "Waypoint : "
                             +ui->viaComboBox->currentText());

        y+=60;
    }

    painter.drawText(100,y,
                     ui->Distance->text());

    y+=60;

    painter.drawText(100,y,
                     ui->fuelCostLabel->text());

    y+=60;

    painter.drawText(100,y,
                     ui->timeLabel->text());

    y+=80;

    painter.setFont(heading);

    painter.drawText(100,y,"Route");

    y+=60;

    painter.setFont(normal);

    QStringList cities = finalRoute.split("\n");

    for(QString city : cities)
    {
        painter.drawText(120,y,city);

        y+=40;
    }

    painter.end();

    QMessageBox::information(
        this,
        "Success",
        "Trip Report exported successfully!");
}
void MainWindow::on_bfsButton_clicked()
{
    bfsLevels.clear();

    bfsLevelIndex = 0;

    bfsBlinkState = false;

    int start = ui->sourcecomboBox->currentIndex();

    graphManager.BFSLevels(start,bfsLevels);

    bfsTimer->start(700);
}
void MainWindow::drawBFSLevel()
{
    scene->clear();

    for(int u=0;u<graphManager.getGraph().size();u++)
    {
        for(auto edge:graphManager.getGraph()[u])
        {
            if(u<edge.first)
            {
                scene->addLine(
                    cityPosition[u].x(),
                    cityPosition[u].y(),
                    cityPosition[edge.first].x(),
                    cityPosition[edge.first].y(),
                    QPen(Qt::lightGray,2));
            }
        }
    }

    for(int i=0;i<graphManager.getCities().size();i++)
    {
        QColor color=Qt::blue;

        for(int l=0;l<bfsLevelIndex;l++)
        {
            for(int node:bfsLevels[l])
            {
                if(node==i)
                    color=Qt::green;
            }
        }

        if(bfsLevelIndex<bfsLevels.size())
        {
            for(int node:bfsLevels[bfsLevelIndex])
            {
                if(node==i)
                {
                    if(bfsBlinkState)
                        color=Qt::yellow;
                    else
                        color=Qt::green;
                }
            }
        }

        scene->addEllipse(
            cityPosition[i].x()-8,
            cityPosition[i].y()-8,
            16,
            16,
            QPen(Qt::black),
            QBrush(color));

        auto txt=scene->addText(graphManager.getCities()[i]);

        txt->setPos(
            cityPosition[i].x()-25,
            cityPosition[i].y()+10);
    }

    bfsBlinkState=!bfsBlinkState;

    if(!bfsBlinkState)
    {
        bfsLevelIndex++;

        if(bfsLevelIndex>=bfsLevels.size())
        {
            bfsTimer->stop();
            QTimer::singleShot(1500, this, [this]()
            {
               restoreRoute();
            });
        }
    }
}

void MainWindow::restoreRoute()
{
    animationTimer->stop();
    bfsTimer->stop();

    currentPath = shortestPath;

    drawMap(
        shortestPath,
        ui->sourcecomboBox->currentIndex(),
        ui->destinationCombobox->currentIndex()
        );
}