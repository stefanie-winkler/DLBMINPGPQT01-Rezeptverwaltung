#ifndef REZEPTDIALOG_H
#define REZEPTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include "rezept.h"

class RezeptDialog : public QDialog {
    Q_OBJECT
public:
    RezeptDialog(QWidget *parent = nullptr);
    Rezept getRezept() const;
    void setRezeptDaten(const Rezept& r);
    void setBestehendeTitel(const QStringList& titel);

private slots:
    void onAccept();

private:
    QLineEdit *titelEdit;
    QLineEdit *kategorieEdit;
    QTextEdit *zutatenEdit;
    QTextEdit *zubereitungEdit;
    QSpinBox *zeitSpin;

    QStringList bestehendeTitel;
    QString urspruenglicherTitel;
    bool bearbeitenModus = false;
};

#endif
