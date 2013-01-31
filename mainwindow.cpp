#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), longestWord("")
{
    ui->setupUi(this);

    // load the word list
    QFile file(QCoreApplication::applicationDirPath() + "/Dictionary.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream wordList(&file);

    QString current = "";

    do
    {
        current = wordList.readLine();
        words.append(current.toUpper());

        // find the biggest word
        if (current.length() > longestWord.length())
            longestWord = current.toUpper();
    }
    while (current != "");
    file.close();

    // add all the text edits to a list for ease of access later
    textEdits.push_back(ui->textEdit00);
    textEdits.push_back(ui->textEdit01);
    textEdits.push_back(ui->textEdit02);
    textEdits.push_back(ui->textEdit03);
    textEdits.push_back(ui->textEdit10);
    textEdits.push_back(ui->textEdit11);
    textEdits.push_back(ui->textEdit12);
    textEdits.push_back(ui->textEdit13);
    textEdits.push_back(ui->textEdit20);
    textEdits.push_back(ui->textEdit21);
    textEdits.push_back(ui->textEdit22);
    textEdits.push_back(ui->textEdit23);
    textEdits.push_back(ui->textEdit30);
    textEdits.push_back(ui->textEdit31);
    textEdits.push_back(ui->textEdit32);
    textEdits.push_back(ui->textEdit33);

    // connect all the text changed signals of the text edits to the slot onTextChanged
    // so we can set a maximum length of 1 for all of them
    foreach (QTextEdit *textEdit, textEdits)
        connect(textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSolve_triggered()
{
    ui->statusBar->showMessage("");

    if (!ParseBoard())
        return;;

    // clear the board for the next round
    ui->listWidget->clear();
    answers.clear();

    // disable all the input boxes for now
    foreach (QTextEdit *edit, textEdits)
        edit->setEnabled(false);
    ui->listWidget->setEnabled(false);

    // start solving, dawg
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            Solution s;
            s.word = QString(board[y][x]);
            QPoint p(x, y);
            s.path.append(p);

            Solve(s, p);
        }
    }

    foreach (QTextEdit *edit, textEdits)
        edit->setEnabled(true);
    ui->listWidget->setEnabled(true);

    // sort the words by score
    qSort(answers.begin(), answers.end(), CompareSolutions);

    // load all the words into the list
    foreach (Solution sol, answers)
        ui->listWidget->addItem(new QListWidgetItem(sol.word));
}

void MainWindow::onTextChanged()
{
    // force the length to always be 1
    QTextEdit *textEdit = reinterpret_cast<QTextEdit*>(sender());
    if (textEdit->toPlainText().length() > 1)
        textEdit->setPlainText(textEdit->toPlainText().at(0));
}

void MainWindow::Solve(Solution wordBuilder, QPoint cur)
{
    // Base case: if the word we're building is longer than the longest, then
    // we've hit a dead end and can return
    if (wordBuilder.word.length() > longestWord.length())
        return;

    QApplication::processEvents();

    if (!WordPossible(wordBuilder.word))
        return;

    if (words.contains(wordBuilder.word))
    {
        answers.append(wordBuilder);
        ui->statusBar->showMessage("Found " + QString::number(answers.size()) + " words");
    }

    // solve to the right
    if (ValidPoint(QPoint(cur.x() + 1, cur.y()), wordBuilder.path))
    {
        QPoint point(cur.x() + 1, cur.y());

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y()][cur.x() + 1];

        Solve(sol, point);
    }

    // solve down right
    if (ValidPoint(QPoint(cur.x() + 1, cur.y() + 1), wordBuilder.path))
    {
        QPoint point(cur.x() + 1, cur.y() + 1);

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y() + 1][cur.x() + 1];

        Solve(sol, point);
    }

    // solve down
    if (ValidPoint(QPoint(cur.x(), cur.y() + 1), wordBuilder.path))
    {
        QPoint point(cur.x(), cur.y() + 1);

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y() + 1][cur.x()];

        Solve(sol, point);
    }

    // solve down left
    if (ValidPoint(QPoint(cur.x() - 1, cur.y() + 1), wordBuilder.path))
    {
        QPoint point(cur.x() - 1, cur.y() + 1);

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y() + 1][cur.x() - 1];

        Solve(sol, point);
    }

    // solve left
    if (ValidPoint(QPoint(cur.x() - 1, cur.y()), wordBuilder.path))
    {
        QPoint point(cur.x() - 1, cur.y());

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y()][cur.x() - 1];

        Solve(sol, point);
    }

    // solve up left
    if (ValidPoint(QPoint(cur.x() - 1, cur.y() - 1), wordBuilder.path))
    {
        QPoint point(cur.x() - 1, cur.y() - 1);

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y() - 1][cur.x() - 1];

        Solve(sol, point);
    }

    // solve up
    if (ValidPoint(QPoint(cur.x(), cur.y() - 1), wordBuilder.path))
    {
        QPoint point(cur.x(), cur.y() - 1);

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y() - 1][cur.x()];

        Solve(sol, point);
    }

    // solve up right
    if (ValidPoint(QPoint(cur.x() + 1, cur.y() - 1), wordBuilder.path))
    {
        QPoint point(cur.x() + 1, cur.y() - 1);

        Solution sol = wordBuilder;
        sol.path.append(point);
        sol.word += board[cur.y() - 1][cur.x() + 1];

        Solve(sol, point);
    }
}

bool MainWindow::ValidPoint(QPoint point, QList<QPoint> &points)
{
    return (point.x() < 4 && point.x() >= 0) && (point.y() < 4 && point.y() >= 0) && !points.contains(point);
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    if (currentRow < 0 || currentRow >= answers.size())
        return;

    Solution sol = answers.at(currentRow);

    // set all to white
    foreach (QTextEdit *textEdit, textEdits)
        textEdit->setStyleSheet("background-color: white;");

    textEdits.at(sol.path.at(0).y() * 4 + sol.path.at(0).x())->setStyleSheet("background-color: #E8C45F");
    for (int i = 1; i < sol.path.size(); i++)
    {
        QString newRgb = "rgb(232, " + QString::number(((unsigned int)0xC4 - i * 0x10) & 0xFF) + ", 95)";
        textEdits.at(sol.path.at(i).y() * 4 + sol.path.at(i).x())->setStyleSheet("background-color: " + newRgb);
    }
}

bool MainWindow::WordPossible(QString wordBuilder)
{
    // do a binary search to check if a word starts with the builder
    int start = 0, end = words.count() - 1, pos = 0;
    while (!words.at(pos).startsWith(wordBuilder))
    {
        pos = (start + end) / 2;
        if (words.at(pos).compare(wordBuilder) < 0)
            start = pos + 1;
        else if (words.at(pos).compare(wordBuilder) > 0)
            end = pos - 1;

        if (start > end)
            return false;
    }
    return true;
}

int WordScore(QString word)
{
    int score = 0;

    // count the score for each letter
    foreach (QChar c, word)
        score += letterScores[c.toAscii() - 65];

    // add on the word bonus
    if (word.length() >= 5 && word.length() <= 9)
        score += (word.length() - 4) * 5;
    else if (word.length() > 9)
        score += 25;

    return score;
}

void MainWindow::on_actionClear_triggered()
{
    foreach (QTextEdit *edit, textEdits)
    {
        edit->setStyleSheet("background-color: white;");
        edit->setPlainText("");
    }

    foreach (QListWidgetItem *item, ui->listWidget->selectedItems())
        item->setSelected(false);

    ui->listWidget->clear();
    ui->statusBar->showMessage("");
}

bool CompareSolutions(const Solution &s1, const Solution &s2)
{
    return WordScore(s1.word) > WordScore(s2.word);
}

void MainWindow::on_actionSave_Board_triggered()
{
    if (!ParseBoard())
        return;

    QString savePath = QFileDialog::getSaveFileName(this, "", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation).replace("\\", "/") + "/board", "*.txt");
    if (savePath == "")
        return;

    // create a new file
    QFile file(savePath);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);

    // write the board
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
            stream << board[y][x];
        stream << '\n';
    }

    // clean and close
    file.flush();
    file.close();

    QMessageBox::information(this, "Success", "Successfully wrote board to file.");
}

bool MainWindow::ParseBoard()
{
    // parse the board
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            // make sure cell is valid
            if (textEdits.at(y * 4 + x)->toPlainText().length() == 1 && textEdits.at(y * 4 + x)->toPlainText().at(0).isLetter())
                board[y][x] = textEdits.at(y * 4 + x)->toPlainText().at(0).toUpper();
            else
            {
                QMessageBox::warning(this, "Invalid Cell", "Invalid cell at (" + QString::number(x) + ", " + QString::number(y) + "). It must contain one letter.");
                return false;
            }
        }
    }

    return true;
}

void MainWindow::on_actionOpen_Board_triggered()
{
    QString path = QFileDialog::getOpenFileName(this, "", QDesktopServices::storageLocation(QDesktopServices::DesktopLocation).replace("\\", "/"), "*.txt");
    if (path == "")
        return;

    // open the file
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&file);

    QString line;
    for (int y = 0; y < 4; y++)
    {
        // read the line in, remove all whitespace
        line = stream.readLine().trimmed();

        // verify that there are 4 characters on the line
        if (line.length() != 4)
        {
            // clear the board
            on_actionSave_Board_triggered();

            QMessageBox::critical(this, "Invalid File", "Save file contains an invalid amount of characters on line " + QString::number(y) + ".");
            return;
        }

        // load the characters into the ui
        for (int x = 0; x < 4; x++)
            textEdits.at(y * 4 + x)->setText(line.at(x));
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, "About", "Created by Adam Spindler, special thanks to Stevie Hetelekides.\n\nThis program is not affiliated with Ruzzle, MAG, or Apple.");
}
