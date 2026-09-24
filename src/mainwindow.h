#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include "rezeptmanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void aktualisiereListe();
    void onRezeptAnlegen();
    void onRezeptLoeschen();
    void onRezeptBearbeiten();
    void onRezepteFiltern();
    void onZurueckZurListe();
    void zeigeDetails(int row);

private:
    void speichern();

    RezeptManager manager;
    QString dateipfad;
    QListWidget *listWidget;
    QLabel *detailsLabel;
    QPushButton *btnZurueck;
    QVector<int> sichtbareIndizes;
    QString aktiverFilter;
};

#endif
