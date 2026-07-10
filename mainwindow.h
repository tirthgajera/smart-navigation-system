#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>
#include <QMainWindow>
#include <vector>
#include <QMap>
#include <QPointF>
#include <QTimer>
#include <queue>
#include <QListWidgetItem>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include "graph.h"
#include "tripmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_findroutebutton_clicked();
    void on_saveTripButton_clicked();
    void on_calculateFuelButton_clicked();
    void on_calculateTimeButton_clicked();
    void on_recentListWidget_itemClicked(QListWidgetItem *item);
    void on_dfsButton_clicked();
    void on_mstButton_clicked();
    void on_exportPdfButton_clicked();
    void on_bfsButton_clicked();
    void drawBFSLevel();

private:
    Graph graphManager;
    TripManager tripManager;
    int finalDistance = 0;
    QString finalRoute;
    int finalWaypoint = -1;
    std::queue<QString> recentSearches;
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QMap<int,QPointF> cityPosition;
    void drawMap(const std::vector<int>& path, int src, int dest);
    QTimer *animationTimer;
   std::vector<DFSStep> dfsSteps;
   std::vector<int> dfsActivePath;
   QTimer *routeTimer;

   std::vector<int> routeAnimationPath;

   int routeAnimationIndex = 0;
    std::vector<std::vector<int>> bfsLevels;
    QTimer *bfsTimer;

    int bfsLevelIndex = 0;

    bool bfsBlinkState = false;
    void restoreRoute();
    std::vector<std::pair<int,int>> mstEdges;
    std::vector<int> currentPath;
    std::vector<int> shortestPath;
    int animationIndex;
    void on_backButton_clicked();

};

#endif