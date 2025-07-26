#include "balancegraph.h"
#include <QPainter>
#include <QPen>
#include <QString>
#include <QPainterPath>
#include <sstream>


int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

BalanceGraph::BalanceGraph(QWidget *parent) : QWidget(parent) {}

void BalanceGraph::setTransactions(const std::vector<std::string>& transactions) {
    balanceHistory.clear();
    double balance = 0;

    // Iterate in reverse to build history (oldest to newest)
    for (auto it = transactions.rbegin(); it != transactions.rend(); ++it) {
        std::stringstream ss(*it);
        std::string date, type, amountStr, comment;

        getline(ss, date, '|');
        getline(ss, type, '|');
        getline(ss, amountStr, '|');
        getline(ss, comment); // ignore for graph

        double amount = std::stod(amountStr);
        if (type == "+")
            balance += amount;
        else
            balance -= amount;

        balanceHistory.push_back(balance);
    }


    update(); // Trigger repaint
}


void BalanceGraph::setBalanceHistory(const std::vector<double>& balances){
    balanceHistory = balances;
    update();
}

void BalanceGraph::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int leftMargin = 40;
    int bottomMargin = 30;
    int topPadding = 20;
    int rightPadding = 20;

    int usableWidth = width() - leftMargin - rightPadding;
    int usableHeight = height() - topPadding - bottomMargin;

    // Background
    painter.fillRect(rect(), Qt::white);

    // Axis lines
    painter.setPen(Qt::black);
    painter.drawLine(leftMargin, topPadding, leftMargin, height() - bottomMargin); // Y axis
    painter.drawLine(leftMargin, height() - bottomMargin, width() - rightPadding, height() - bottomMargin); // X axis

    // Y Axis (ticks and labels)
    int maxY = 7;
    int tickLength = 5;
    for (int i = 0; i <= maxY; ++i) {
        int y = topPadding + usableHeight - static_cast<int>((i / static_cast<double>(maxY)) * usableHeight);

        // Tick
        painter.drawLine(leftMargin - tickLength, y, leftMargin, y);

        // Label
        QString label = QString::number(i);
        QRect textRect(0, y - 10, leftMargin - tickLength - 2, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // Bar Graph Drawing
    if (balanceHistory.empty())
        return;

    int barCount = static_cast<int>(balanceHistory.size());
    double barSpacing = usableWidth / static_cast<double>(barCount);
    int barWidth = static_cast<int>(barSpacing * 0.6);  // 60% of space for bar, 40% for padding

    painter.setBrush(QColor("#4A90E2")); // Blue bars
    painter.setPen(Qt::NoPen);

    for (int i = 0; i < barCount; ++i) {
        int value = clamp(balanceHistory[i], 0, 7);
        double valueRatio = value / 7.0;

        int barHeight = static_cast<int>(valueRatio * usableHeight);
        if (value == 0) {
            barHeight = 2;  // Draw a minimal height bar
        }

        int x = leftMargin + static_cast<int>(i * barSpacing + (barSpacing - barWidth) / 2);
        int y = topPadding + usableHeight - barHeight;

        painter.drawRect(x, y, barWidth, barHeight);
    }
}


void BalanceGraph::setGraphWithInt(const std::vector<int>& values){
    std::vector<double> newVector;
    for(auto &value:values){
        double newDouble = static_cast<double>(value);
        newVector.push_back(newDouble);
    }

    balanceHistory = newVector;
    update();
}
