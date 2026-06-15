#include "chart.h"

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QStatusBar>
#include <QLabel>
#include <QSplitter>
#include <QFrame>
#include <QGroupBox>
#include <QStyle>

// ========== 全局样式表 ==========
static const char *APP_STYLE = R"(
    QMainWindow {
        background-color: #F5F6FA;
    }
    QPushButton {
        padding: 8px 18px;
        border-radius: 6px;
        border: 1px solid #C8CCD4;
        background-color: #FFFFFF;
        color: #333333;
        font-size: 13px;
        font-weight: 500;
    }
    QPushButton:hover {
        background-color: #E8EDF5;
        border-color: #A0A8B8;
    }
    QPushButton:pressed {
        background-color: #D0D7E5;
    }
    QPushButton#btnHistogram[active="true"],
    QPushButton#btnLineChart[active="true"],
    QPushButton#btnPieChart[active="true"] {
        background-color: #3B7CBF;
        color: #FFFFFF;
        border-color: #2E6096;
    }
    QTableWidget {
        background-color: #FFFFFF;
        border: 1px solid #D8DCE4;
        border-radius: 4px;
        gridline-color: #EAECF0;
        font-size: 12px;
    }
    QTableWidget::item {
        padding: 3px 8px;
    }
    QHeaderView::section {
        background-color: #EEF1F7;
        border: none;
        border-bottom: 2px solid #D8DCE4;
        padding: 6px 10px;
        font-weight: bold;
        font-size: 12px;
        color: #555555;
    }
    QStatusBar {
        background-color: #EEF1F7;
        border-top: 1px solid #D8DCE4;
        font-size: 12px;
        color: #555555;
        padding: 3px;
    }
    QGroupBox {
        font-weight: bold;
        color: #444444;
        border: 1px solid #D8DCE4;
        border-radius: 6px;
        margin-top: 8px;
        padding-top: 14px;
        background-color: #FFFFFF;
    }
    QGroupBox::title {
        subcontrol-origin: margin;
        left: 12px;
        padding: 0 6px;
    }
    QSplitter::handle {
        background-color: #D8DCE4;
        height: 2px;
    }
)";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet(APP_STYLE);

    // ========== 主窗口 ==========
    QMainWindow w;
    w.setWindowTitle("面向对象图表生成系统（Qt版）");
    w.resize(1000, 750);

    // ========== 中央部件（垂直布局）==========
    QWidget *centralWidget = new QWidget(&w);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(12, 10, 12, 8);
    mainLayout->setSpacing(8);

    // ========== 创建共享数据源，预置初始数据 ==========
    DataSource *dataSource = new DataSource(centralWidget);
    dataSource->setData({25, 48, 32, 60, 42, 55, 38, 70, 29, 51});

    // ========== 顶部工具栏 ==========
    QGroupBox *toolGroup = new QGroupBox("图表操作");
    QHBoxLayout *toolLayout = new QHBoxLayout(toolGroup);
    toolLayout->setSpacing(10);

    // 图表类型按钮组
    QLabel *chartTypeLabel = new QLabel("图表类型:");
    chartTypeLabel->setStyleSheet("font-weight:bold; color:#555;");
    toolLayout->addWidget(chartTypeLabel);

    QPushButton *btnHist = new QPushButton("📊 柱状图");
    btnHist->setObjectName("btnHistogram");
    QPushButton *btnLine  = new QPushButton("📈 折线图");
    btnLine->setObjectName("btnLineChart");
    QPushButton *btnPie   = new QPushButton("🥧 饼图");
    btnPie->setObjectName("btnPieChart");

    toolLayout->addWidget(btnHist);
    toolLayout->addWidget(btnLine);
    toolLayout->addWidget(btnPie);

    // 分隔线
    QFrame *sep1 = new QFrame();
    sep1->setFrameShape(QFrame::VLine);
    sep1->setStyleSheet("color:#D8DCE4;");
    toolLayout->addWidget(sep1);

    // 数据操作按钮组
    QLabel *dataOpLabel = new QLabel("数据操作:");
    dataOpLabel->setStyleSheet("font-weight:bold; color:#555;");
    toolLayout->addWidget(dataOpLabel);

    QPushButton *btnLoad   = new QPushButton("📂 加载文件");
    QPushButton *btnRandom = new QPushButton("🎲 随机生成");
    QPushButton *btnClear  = new QPushButton("🗑 清空数据");

    toolLayout->addWidget(btnLoad);
    toolLayout->addWidget(btnRandom);
    toolLayout->addWidget(btnClear);
    toolLayout->addStretch();

    mainLayout->addWidget(toolGroup);

    // ========== 上下分割器：图表区 + 数据表 ==========
    QSplitter *splitter = new QSplitter(Qt::Vertical);

    // 图表显示区域（占据大部分空间）
    QWidget *chartContainer = new QWidget();
    QVBoxLayout *chartContainerLayout = new QVBoxLayout(chartContainer);
    chartContainerLayout->setContentsMargins(0, 0, 0, 0);

    Chart *chartView = nullptr;  // 多态基类指针
    // 初始默认显示柱状图
    chartView = new Histogram(chartContainer);
    chartView->setDataSource(dataSource);
    chartContainerLayout->addWidget(chartView);

    splitter->addWidget(chartContainer);

    // 底部数据表格
    QWidget *tableContainer = new QWidget();
    QVBoxLayout *tableLayout = new QVBoxLayout(tableContainer);
    tableLayout->setContentsMargins(0, 4, 0, 0);

    QLabel *tableTitle = new QLabel("📋 当前数据");
    tableTitle->setStyleSheet("font-weight:bold; font-size:13px; color:#444; padding:2px 4px;");
    tableLayout->addWidget(tableTitle);

    QTableWidget *dataTable = new QTableWidget();
    dataTable->setColumnCount(2);
    dataTable->setHorizontalHeaderLabels({"序号", "数值"});
    dataTable->setRowCount(0);
    dataTable->setMaximumHeight(160);
    dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    dataTable->setAlternatingRowColors(true);
    dataTable->horizontalHeader()->setStretchLastSection(true);
    dataTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    dataTable->verticalHeader()->setVisible(false);

    // 刷新数据表的函数
    auto refreshDataTable = [dataSource, dataTable]() {
        const QVector<double> &d = dataSource->data();
        dataTable->setRowCount(d.size());
        for (int i = 0; i < d.size(); ++i) {
            // 序号
            QTableWidgetItem *indexItem = new QTableWidgetItem(QString::number(i + 1));
            indexItem->setTextAlignment(Qt::AlignCenter);
            dataTable->setItem(i, 0, indexItem);
            // 数值
            QTableWidgetItem *valueItem = new QTableWidgetItem(
                QString::number(d[i], 'f', 1));
            valueItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            dataTable->setItem(i, 1, valueItem);
        }
    };
    // 初始加载
    refreshDataTable();

    tableLayout->addWidget(dataTable);
    splitter->addWidget(tableContainer);

    // 设置分割比例（图表占 75%，表格占 25%）
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter, 1);

    // ========== 状态栏 ==========
    QStatusBar *statusBar = w.statusBar();

    QLabel *statusCount  = new QLabel();
    QLabel *statusSum    = new QLabel();
    QLabel *statusMin    = new QLabel();
    QLabel *statusMax    = new QLabel();

    statusBar->addWidget(statusCount);
    statusBar->addWidget(new QLabel("  │  "));
    statusBar->addWidget(statusSum);
    statusBar->addWidget(new QLabel("  │  "));
    statusBar->addWidget(statusMin);
    statusBar->addWidget(new QLabel("  │  "));
    statusBar->addWidget(statusMax);

    auto refreshStatusBar = [dataSource, statusCount, statusSum, statusMin, statusMax]() {
        int n = dataSource->count();
        if (n == 0) {
            statusCount->setText("数据条数: 0");
            statusSum->setText("合计: —");
            statusMin->setText("最小值: —");
            statusMax->setText("最大值: —");
        } else {
            statusCount->setText(QString("数据条数: %1").arg(n));
            statusSum->setText(QString("合计: %1").arg(dataSource->sum(), 0, 'f', 1));
            statusMin->setText(QString("最小值: %1").arg(dataSource->minValue(), 0, 'f', 1));
            statusMax->setText(QString("最大值: %1").arg(dataSource->maxValue(), 0, 'f', 1));
        }
    };
    refreshStatusBar();

    // ========== 图表按钮高亮辅助 ==========
    QList<QPushButton*> chartButtons = {btnHist, btnLine, btnPie};
    auto highlightChartButton = [chartButtons](QPushButton *active) {
        for (auto *btn : chartButtons) {
            btn->setProperty("active", btn == active ? "true" : "false");
            // 强制 Qt 重新计算样式
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
        }
    };
    // 初始高亮柱状图
    highlightChartButton(btnHist);

    // ========== 数据变更 → 刷新表格和状态栏 ==========
    QObject::connect(dataSource, &DataSource::dataChanged, [&]() {
        refreshDataTable();
        refreshStatusBar();
    });

    // ========== 数据操作槽函数 ==========

    // 从文件加载数据
    QObject::connect(btnLoad, &QPushButton::clicked, [&]() {
        QString filePath = QFileDialog::getOpenFileName(
            &w,
            "选择数据文件",
            "",
            "文本文件 (*.txt *.csv *.dat);;所有文件 (*)"
            );
        if (filePath.isEmpty()) return;

        if (!dataSource->loadFromFile(filePath)) {
            QMessageBox::warning(&w, "加载失败", dataSource->lastError());
        }
        // 加载成功 → dataChanged 信号自动驱动刷新
    });

    // 随机生成数据
    QObject::connect(btnRandom, &QPushButton::clicked, [&]() {
        bool ok = false;
        int count = QInputDialog::getInt(
            &w,
            "随机生成数据",
            "请输入数据个数 (1~50):",
            10,   // 默认值
            1,    // 最小值
            50,   // 最大值
            1,    // 步长
            &ok
            );
        if (!ok) return;

        dataSource->generateRandom(count, 10.0, 100.0);
        // 生成成功 → dataChanged 信号自动驱动刷新
    });

    // 清空数据
    QObject::connect(btnClear, &QPushButton::clicked, [&]() {
        dataSource->setData({});
    });

    // ========== 图表切换槽函数（多态）==========

    auto switchChart = [&](Chart *newChart, QPushButton *activeBtn) {
        // 移除旧的图表控件
        if (chartView) {
            chartContainerLayout->removeWidget(chartView);
            chartView->deleteLater();
            chartView = nullptr;
        }
        chartView = newChart;
        chartView->setDataSource(dataSource);
        chartContainerLayout->addWidget(chartView);
        highlightChartButton(activeBtn);
    };

    QObject::connect(btnHist, &QPushButton::clicked, [&]() {
        switchChart(new Histogram(chartContainer), btnHist);
    });

    QObject::connect(btnLine, &QPushButton::clicked, [&]() {
        switchChart(new LineChart(chartContainer), btnLine);
    });

    QObject::connect(btnPie, &QPushButton::clicked, [&]() {
        switchChart(new PieChart(chartContainer), btnPie);
    });

    w.setCentralWidget(centralWidget);
    w.show();

    return a.exec();
}
