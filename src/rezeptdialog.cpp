#include "rezeptdialog.h"
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>

RezeptDialog::RezeptDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Rezept anlegen");
    setMinimumWidth(560);

    titelEdit = new QLineEdit();
    titelEdit->setMinimumHeight(34);
    kategorieEdit = new QLineEdit();
    kategorieEdit->setMinimumHeight(34);
    zutatenEdit = new QTextEdit();
    zutatenEdit->setPlaceholderText("Eine Zutat pro Zeile");
    zutatenEdit->setMinimumHeight(130);
    zubereitungEdit = new QTextEdit();
    zubereitungEdit->setMinimumHeight(150);
    zubereitungEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    zeitSpin = new QSpinBox();
    zeitSpin->setRange(0, 999);
    zeitSpin->setSuffix(" min");
    zeitSpin->setMinimumHeight(34);

    auto *form = new QFormLayout();
    form->setVerticalSpacing(14);
    form->setHorizontalSpacing(16);
    form->addRow("Titel*:", titelEdit);
    form->addRow("Kategorie:", kategorieEdit);
    form->addRow("Zutaten*:", zutatenEdit);
    form->addRow("Zubereitung*:", zubereitungEdit);
    form->addRow("Zubereitungszeit:", zeitSpin);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("OK");
    buttons->button(QDialogButtonBox::Cancel)->setText("Abbrechen");
    for (QAbstractButton *b : buttons->buttons())
        b->setMinimumSize(110, 38);
    connect(buttons, &QDialogButtonBox::accepted, this, &RezeptDialog::onAccept);
    connect(buttons, &QDialogButtonBox::rejected, this, &RezeptDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(18);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

void RezeptDialog::onAccept() {
    // Jedes Pflichtfeld einzeln prüfen: Meldung nennt das Feld, der Fokus springt dorthin
    const QString titel = titelEdit->text().trimmed();

    if (titel.isEmpty()) {
        QMessageBox::warning(this, "Fehlende Angaben", "Bitte einen Titel eingeben.");
        titelEdit->setFocus();
        return;
    }
    if (zutatenEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Fehlende Angaben",
                              "Bitte mindestens eine Zutat eingeben (eine Zutat pro Zeile).");
        zutatenEdit->setFocus();
        return;
    }
    if (zubereitungEdit->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Fehlende Angaben", "Bitte die Zubereitung beschreiben.");
        zubereitungEdit->setFocus();
        return;
    }

    // Doppelte Titel abfangen (beim Bearbeiten zählt der eigene, alte Titel nicht mit)
    QStringList andere = bestehendeTitel;
    if (bearbeitenModus)
        andere.removeOne(urspruenglicherTitel);

    for (const QString& vorhanden : andere) {
        if (vorhanden.trimmed().compare(titel, Qt::CaseInsensitive) == 0) {
            QMessageBox box(this);
            box.setWindowTitle("Titel bereits vorhanden");
            box.setTextFormat(Qt::PlainText);
            box.setText(QString("Es gibt bereits ein Rezept mit dem Titel „%1“.\n"
                                "Trotzdem speichern?").arg(titel));
            QPushButton *trotzdem = box.addButton("Trotzdem speichern", QMessageBox::AcceptRole);
            box.addButton("Zurück", QMessageBox::RejectRole);
            box.exec();

            if (box.clickedButton() != trotzdem) {
                titelEdit->setFocus();
                titelEdit->selectAll();
                return;
            }
            break;
        }
    }

    accept();
}

Rezept RezeptDialog::getRezept() const {
    QStringList zutaten = zutatenEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    return Rezept(titelEdit->text().trimmed(),
                  kategorieEdit->text().trimmed(),
                  zutaten,
                  zubereitungEdit->toPlainText().trimmed(),
                  zeitSpin->value());
}

void RezeptDialog::setRezeptDaten(const Rezept& r) {
    titelEdit->setText(r.getTitel());
    kategorieEdit->setText(r.getKategorie());
    zutatenEdit->setPlainText(r.getZutaten().join('\n'));
    zubereitungEdit->setPlainText(r.getZubereitung());
    zeitSpin->setValue(r.getZubereitungszeit());
    setWindowTitle("Rezept bearbeiten");
    bearbeitenModus = true;
    urspruenglicherTitel = r.getTitel();
}

void RezeptDialog::setBestehendeTitel(const QStringList& titel) {
    bestehendeTitel = titel;
}
