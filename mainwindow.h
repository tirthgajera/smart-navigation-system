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
    int finalDistance = 0;
    QString finalRoute;
    int finalWaypoint = -1;
    std::queue<QString> recentSearches;
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QMap<int,QPointF> cityPosition;
    void drawMap(const std::vector<int>& path, int src, int dest);
    std::vector<std::vector<std::pair<int,int>>> graph;
    std::vector<QString> cities;
    QTimer *animationTimer;
    std::vector<int> dfsOrder;
    std::vector<bool> visitedDFS;
    void DFS(int node);
    std::vector<std::vector<int>> bfsLevels;

    QTimer *bfsTimer;

    int bfsLevelIndex = 0;

    bool bfsBlinkState = false;
    std::vector<int> parentDSU;
    std::vector<int> rankDSU;
    void restoreRoute();
    int Find(int x);
    void Union(int a,int b);
    std::vector<std::pair<int,int>> mstEdges;
    std::vector<int> currentPath;
    int animationIndex;
    void initializeGraph();
    void on_backButton_clicked();
    bool shortestPath(
        int src,
        int dest,
        std::vector<int> &path,
        int &distance
        );
};

#endif