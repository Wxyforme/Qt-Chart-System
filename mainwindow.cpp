#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "histogram.h"
#include "linechart.h"
#include "piechart.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QStyle>

//构造 / 析构

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dataSource(0)
    , m_chartView(0)
    , m_statusCount(0)
    , m_statusSum(0)
    , m_statusMin(0)
    , m_statusMax(0)
{
    ui->setupUi(this);

    // ---- 图表容器布局 ----
    // 为 chartContainer 添加布局，用于承载图表控件
    QVBoxLayout *chartLayout = new QVBoxLayout(ui->chartContainer);
    chartLayout->setContentsMargins(0, 0, 0, 0);

    // ---- 数据源（预置初始数据）----
    m_dataSource = new DataSource(this);
    QVector<double> initData;
    initData << 25 << 48 << 32 << 60 << 42 << 55 << 38 << 70 << 29 << 51;
    m_dataSource->setData(initData);

    // ---- 初始图表 ----
    setupChart();

    // ---- 状态栏标签 ----
    m_statusCount = new QLabel();
    m_statusSum   = new QLabel();
    m_statusMin   = new QLabel();
    m_statusMax   = new QLabel();

    ui->statusBar->addWidget(m_statusCount);
    ui->statusBar->addWidget(new QLabel("  │  "));
    ui->statusBar->addWidget(m_statusSum);
    ui->statusBar->addWidget(new QLabel("  │  "));
    ui->statusBar->addWidget(m_statusMin);
    ui->statusBar->addWidget(new QLabel("  │  "));
    ui->statusBar->addWidget(m_statusMax);

    refreshStatusBar();

    // ---- 图表按钮列表（用于高亮管理）----
    m_chartButtons.append(ui->btnHistogram);
    m_chartButtons.append(ui->btnLineChart);
    m_chartButtons.append(ui->btnPieChart);
    highlightChartButton(ui->btnHistogram);

    // ==================== 信号连接 ====================

    // 数据变更 → 刷新状态栏
    connect(m_dataSource, &DataSource::dataChanged, this, &MainWindow::refreshStatusBar);

    // 数据操作按钮
    connect(ui->btnLoad,   &QPushButton::clicked, this, &MainWindow::onLoadFile);
    connect(ui->btnRandom, &QPushButton::clicked, this, &MainWindow::onRandomGenerate);
    connect(ui->btnClear,  &QPushButton::clicked, this, &MainWindow::onClearData);

    // 图表类型切换按钮（统一走 onChartTypeClicked 分发）
    connect(ui->btnHistogram,  &QPushButton::clicked, this, &MainWindow::onChartTypeClicked);
    connect(ui->btnLineChart,  &QPushButton::clicked, this, &MainWindow::onChartTypeClicked);
    connect(ui->btnPieChart,   &QPushButton::clicked, this, &MainWindow::onChartTypeClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ==================== 初始化 ====================

void MainWindow::setupChart()
{
    m_chartView = new Histogram(ui->chartContainer);
    m_chartView->setDataSource(m_dataSource);
    ui->chartContainer->layout()->addWidget(m_chartView);
}

// ==================== 图表切换 ====================

void MainWindow::onChartTypeClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    Chart *newChart = 0;

    if (btn == ui->btnHistogram) {
        newChart = new Histogram(ui->chartContainer);
    } else if (btn == ui->btnLineChart) {
        newChart = new LineChart(ui->chartContainer);
    } else if (btn == ui->btnPieChart) {
        newChart = new PieChart(ui->chartContainer);
    }

    if (newChart) {
        switchChart(newChart, btn);
    }
}

void MainWindow::switchChart(Chart *newChart, QPushButton *activeBtn)
{
    if (m_chartView) {
        ui->chartContainer->layout()->removeWidget(m_chartView);
        m_chartView->deleteLater();
        m_chartView = 0;
    }
    m_chartView = newChart;
    m_chartView->setDataSource(m_dataSource);
    ui->chartContainer->layout()->addWidget(m_chartView);
    highlightChartButton(activeBtn);
}

void MainWindow::highlightChartButton(QPushButton *active)
{
    for (int i = 0; i < m_chartButtons.size(); ++i) {
        QPushButton *btn = m_chartButtons[i];
        btn->setProperty("active", btn == active ? "true" : "false");
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}

// ==================== 数据操作 ====================

void MainWindow::onLoadFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择数据文件",
        "",
        "文本文件 (*.txt *.csv *.dat);;所有文件 (*)"
    );
    if (filePath.isEmpty()) return;

    if (!m_dataSource->loadFromFile(filePath)) {
        QMessageBox::warning(this, "加载失败", m_dataSource->lastError());
    }
}

void MainWindow::onRandomGenerate()
{
    bool ok = false;
    int count = QInputDialog::getInt(
        this,
        "随机生成数据",
        "请输入数据个数 (1~50):",
        10,
        1,
        50,
        1,
        &ok
    );
    if (!ok) return;

    m_dataSource->generateRandom(count, 10.0, 100.0);
}

void MainWindow::onClearData()
{
    QVector<double> emptyData;
    m_dataSource->setData(emptyData);
}

//状态栏刷新
void MainWindow::refreshStatusBar()
{
    int n = m_dataSource->count();
    if (n == 0) {
        m_statusCount->setText("数据条数: 0");
        m_statusSum->setText("合计: —");
        m_statusMin->setText("最小值: —");
        m_statusMax->setText("最大值: —");
    } else {
        m_statusCount->setText(QString("数据条数: %1").arg(n));
        m_statusSum->setText(QString("合计: %1").arg(m_dataSource->sum(), 0, 'f', 1));
        m_statusMin->setText(QString("最小值: %1").arg(m_dataSource->minValue(), 0, 'f', 1));
        m_statusMax->setText(QString("最大值: %1").arg(m_dataSource->maxValue(), 0, 'f', 1));
    }
}
