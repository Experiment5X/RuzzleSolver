#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QPoint>
#include <QtGui/QTextEdit>
#include <QMessageBox>
#include <QSet>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QDesktopServices>

namespace Ui {
class MainWindow;
}

struct Solution
{
    QString word;
    QList<QPoint> path;
};

static int letterScores[26] = { 1, 5, 4, 2, 1, 4, 3, 4, 1, 10, 5, 1, 3, 1, 1, 4, 10, 1, 1, 1, 2, 4, 4, 8, 4, 10 };

// compare 2 solutions, returns true if s1 > s2
bool CompareSolutions(const Solution &s1, const Solution &s2);

// get the score for a given word
int WordScore(QString word);

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionSolve_triggered();
    void onTextChanged();
    void on_listWidget_currentRowChanged(int currentRow);
    void on_actionClear_triggered();
    void on_actionSave_Board_triggered();
    void on_actionOpen_Board_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QChar board[4][4];
    QList<QTextEdit*> textEdits;
    QString longestWord;
    QList<QString> words;
    QList<Solution> answers;

    // solve for all words, will be called recursively
    void Solve(Solution wordBuilder, QPoint cur);

    // verify that a give point is within the bounds of a boggle board
    bool ValidPoint(QPoint point, QList<QPoint> &points);

    // check if the word is possible
    bool WordPossible(QString wordBuilder);

    // take all the user input and put it into an internal array
    bool ParseBoard();
};

#endif // MAINWINDOW_H
