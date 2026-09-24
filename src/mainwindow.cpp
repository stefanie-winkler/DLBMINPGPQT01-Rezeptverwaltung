#include "mainwindow.h"
#include "rezeptdialog.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QStandardPaths>
#include <QColor>
#include <algorithm>

namespace {

QString farbButtonStil(const QString& farbe) {
    const QColor basis(farbe);
    return QString(
               "QPushButton {"
               "  background-color: %1;"
               "  color: #111111;"
               "  border: none;"
               "  border-radius: 14px;"
               "  padding: 8px;"
               "}"
               "QPushButton:hover { background-color: %2; }"
               "QPushButton:pressed { background-color: %3; }")
        .arg(basis.name(),
             basis.darker(110).name(),
             basis.darker(125).name());
}


void sortiereNachTitel(QVector<int>& indizes, const QVector<Rezept>& rezepte) {
    std::sort(indizes.begin(), indizes.end(), [&rezepte](int a, int b) {
        return QString::localeAwareCompare(rezepte.at(a).getTitel(),
                                           rezepte.at(b).getTitel()) < 0;
    });
}


QStringList alleTitel(const RezeptManager& manager) {
    QStringList titel;
    const QVector<Rezept> alle = manager.getAlleRezepte();
    for (const Rezept& r : alle)
        titel << r.getTitel();
    return titel;
}

}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Rezeptverwaltung");
    resize(1000, 680);
    setMinimumSize(800, 550);


    dateipfad = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                + "/rezepte.json";
    QString fehler;
    if (!manager.laden(dateipfad, &fehler)) {
        QMessageBox::warning(this, "Laden fehlgeschlagen",
                             "Die Rezeptdatei konnte nicht gelesen werden. "
                             "Falls sie beschädigt war, wurde sie als .bak gesichert.\n" + fehler);
    }

    auto *btnFiltern = new QPushButton("REZEPTE FILTERN\nnach Titel/Kategorie/Zutat");
    auto *btnAnlegen = new QPushButton("REZEPT\nANLEGEN");
    auto *btnBearbeiten = new QPushButton("REZEPT\nBEARBEITEN");
    auto *btnLoeschen = new QPushButton("REZEPT\nLÖSCHEN");
    btnZurueck = new QPushButton("ZURÜCK ZUM\nHAUPTMENÜ");
    btnZurueck->hide();


    btnFiltern->setStyleSheet(farbButtonStil("#8AB4F8"));
    btnAnlegen->setStyleSheet(farbButtonStil("#81C995"));
    btnBearbeiten->setStyleSheet(farbButtonStil("#FDD663"));
    btnLoeschen->setStyleSheet(farbButtonStil("#F28B82"));


    btnZurueck->setStyleSheet(
        "QPushButton {"
        "  background-color: #E5E7EB;"
        "  color: #111111;"
        "  border: none;"
        "  border-radius: 14px;"
        "  padding: 8px;"
        "}"
        "QPushButton:hover { background-color: #D1D5DB; }"
        "QPushButton:pressed { background-color: #C4C8CF; }");

    for (QPushButton *b : {btnFiltern, btnAnlegen, btnBearbeiten, btnLoeschen, btnZurueck}) {
        b->setMinimumHeight(80);   // deutlich höhere Buttons
        b->setCursor(Qt::PointingHandCursor);
        b->setFocusPolicy(Qt::TabFocus);
    }

    auto *linkeSpalte = new QVBoxLayout();

    auto cmZuPixel = [this](double cm) { return qRound(cm / 2.54 * logicalDpiY()); };
    linkeSpalte->setSpacing(cmZuPixel(0.5));
    linkeSpalte->addSpacing(cmZuPixel(0.5));
    linkeSpalte->addWidget(btnFiltern);
    linkeSpalte->addWidget(btnAnlegen);
    linkeSpalte->addWidget(btnBearbeiten);
    linkeSpalte->addWidget(btnLoeschen);
    linkeSpalte->addWidget(btnZurueck);
    linkeSpalte->addStretch();

    listWidget = new QListWidget();
    listWidget->setWordWrap(false);
    listWidget->setTextElideMode(Qt::ElideRight);
    listWidget->setMinimumWidth(250);
    listWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    listWidget->setStyleSheet(
        "QListWidget { border: 1px solid #999; }"
        "QListWidget::item { padding: 3px 6px; }"
        "QListWidget::item:selected { background-color: #4A90D9; color: white; }");

    detailsLabel = new QLabel();
    detailsLabel->setWordWrap(true);
    detailsLabel->setTextFormat(Qt::RichText);
    detailsLabel->setAlignment(Qt::AlignTop);
    detailsLabel->setStyleSheet("padding: 14px;");

    auto *detailsScroll = new QScrollArea();
    detailsScroll->setWidget(detailsLabel);
    detailsScroll->setWidgetResizable(true);
    detailsScroll->setStyleSheet("border: 1px solid #999;");
    detailsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    detailsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *rechteSpalte = new QVBoxLayout();
    rechteSpalte->setSpacing(10);
    rechteSpalte->addWidget(new QLabel("<h2>Rezepte</h2>"));
    rechteSpalte->addWidget(listWidget, 35);
    rechteSpalte->addWidget(new QLabel("<b>Details</b>"));
    rechteSpalte->addWidget(detailsScroll, 65);

    auto *hauptLayout = new QHBoxLayout();
    hauptLayout->setContentsMargins(20, 20, 20, 20);
    hauptLayout->setSpacing(20);
    hauptLayout->addLayout(linkeSpalte, 1);
    hauptLayout->addLayout(rechteSpalte, 3);

    auto *zentral = new QWidget();
    zentral->setLayout(hauptLayout);
    setCentralWidget(zentral);

    connect(btnFiltern, &QPushButton::clicked, this, &MainWindow::onRezepteFiltern);
    connect(btnAnlegen, &QPushButton::clicked, this, &MainWindow::onRezeptAnlegen);
    connect(btnLoeschen, &QPushButton::clicked, this, &MainWindow::onRezeptLoeschen);
    connect(btnZurueck, &QPushButton::clicked, this, &MainWindow::onZurueckZurListe);
    connect(btnBearbeiten, &QPushButton::clicked, this, &MainWindow::onRezeptBearbeiten);
    connect(listWidget, &QListWidget::currentRowChanged, this, &MainWindow::zeigeDetails);

    aktualisiereListe();
    listWidget->setCurrentRow(-1);
}

void MainWindow::speichern() {
    QString fehler;
    if (!manager.speichern(dateipfad, &fehler)) {
        QMessageBox::critical(this, "Speichern fehlgeschlagen",
                              "Die Rezepte konnten nicht gespeichert werden:\n" + fehler);
    }
}

void MainWindow::aktualisiereListe() {
    listWidget->clear();
    auto alleRezepte = manager.getAlleRezepte();

    if (aktiverFilter.isEmpty()) {
        sichtbareIndizes.clear();
        for (int i = 0; i < alleRezepte.size(); ++i)
            sichtbareIndizes.append(i);
        btnZurueck->hide();
    } else {
        sichtbareIndizes = manager.filterIndizes(aktiverFilter);
        btnZurueck->show();
    }

    sortiereNachTitel(sichtbareIndizes, alleRezepte);
    for (int index : sichtbareIndizes)
        listWidget->addItem(alleRezepte.at(index).getTitel());
}

void MainWindow::onRezepteFiltern() {
    QDialog dialog(this);
    dialog.setWindowTitle("Rezepte filtern");
    dialog.setMinimumWidth(420);

    auto *label = new QLabel("Suchbegriff (Titel, Kategorie oder Zutat):");
    auto *lineEdit = new QLineEdit();
    lineEdit->setMinimumHeight(34);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("OK");
    buttons->button(QDialogButtonBox::Cancel)->setText("Abbrechen");
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    aktiverFilter = lineEdit->text().trimmed();
    aktualisiereListe();

    if (!aktiverFilter.isEmpty() && sichtbareIndizes.isEmpty()) {
        QMessageBox::information(this, "Keine Treffer",
                                  "Kein Rezept passt zu diesem Suchbegriff.");
    }
}

void MainWindow::onZurueckZurListe() {
    aktiverFilter.clear();
    aktualisiereListe();
}

void MainWindow::zeigeDetails(int row) {
    if (row < 0 || row >= sichtbareIndizes.size()) {
        detailsLabel->setText("");
        return;
    }

    auto alleRezepte = manager.getAlleRezepte();
    int echterIndex = sichtbareIndizes.at(row);
    const Rezept& r = alleRezepte.at(echterIndex);

    QString kategorie = r.getKategorie().isEmpty() ? "-" : r.getKategorie().toHtmlEscaped();
    QString zubereitungUmbrochen = r.getZubereitung().toHtmlEscaped();
    zubereitungUmbrochen.replace("\n", "<br>");

    QStringList zutaten;
    for (const QString& z : r.getZutaten())
        zutaten << z.toHtmlEscaped();

    QString text = QString(
                       "<div style='line-height:140%'>"
                       "<b>Kategorie:</b> %1<br>"
                       "<b>Zubereitungszeit:</b> %2 min<br><br>"
                       "<b>Zutaten:</b><br>%3<br><br>"
                       "<b>Zubereitung:</b><br>%4"
                       "</div>"
                       )
                       .arg(kategorie,
                            QString::number(r.getZubereitungszeit()),
                            zutaten.join("<br>"),
                            zubereitungUmbrochen);

    detailsLabel->setText(text);
}

void MainWindow::onRezeptAnlegen() {
    RezeptDialog dialog(this);
    dialog.setBestehendeTitel(alleTitel(manager));
    if (dialog.exec() == QDialog::Accepted) {
        manager.addRezept(dialog.getRezept());
        speichern();
        aktualisiereListe();
    }
}

void MainWindow::onRezeptLoeschen() {
    int zeile = listWidget->currentRow();
    if (zeile < 0 || zeile >= sichtbareIndizes.size()) {
        QMessageBox::information(this, "Kein Rezept ausgewählt",
                                  "Bitte zuerst ein Rezept in der Liste auswählen.");
        return;
    }
    int echterIndex = sichtbareIndizes.at(zeile);

    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("Rezept löschen");
    confirmBox.setText("Wollen Sie das Rezept wirklich löschen?");
    QPushButton *jaButton = confirmBox.addButton("Ja", QMessageBox::YesRole);
    confirmBox.addButton("Nein", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() != jaButton) {
        return;
    }

    manager.removeRezept(echterIndex);
    speichern();
    aktualisiereListe();
    detailsLabel->setText("");
}

void MainWindow::onRezeptBearbeiten() {
    int zeile = listWidget->currentRow();
    if (zeile < 0 || zeile >= sichtbareIndizes.size()) {
        QMessageBox::information(this, "Kein Rezept ausgewählt",
                                  "Bitte zuerst ein Rezept in der Liste auswählen.");
        return;
    }
    int echterIndex = sichtbareIndizes.at(zeile);

    RezeptDialog dialog(this);
    dialog.setBestehendeTitel(alleTitel(manager));
    dialog.setRezeptDaten(manager.getAlleRezepte().at(echterIndex));
    if (dialog.exec() == QDialog::Accepted) {
        manager.editRezept(echterIndex, dialog.getRezept());
        speichern();
        aktualisiereListe();
        listWidget->setCurrentRow(sichtbareIndizes.indexOf(echterIndex));
    }
}
