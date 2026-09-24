#include "rezeptmanager.h"
#include <QSaveFile>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace {

bool passtZuSuchbegriff(const Rezept& r, const QString& suchbegriff) {
    if (r.getTitel().contains(suchbegriff, Qt::CaseInsensitive) ||
        r.getKategorie().contains(suchbegriff, Qt::CaseInsensitive))
        return true;

    const QStringList zutaten = r.getZutaten();
    for (const QString& zutat : zutaten) {
        if (zutat.contains(suchbegriff, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

} // namespace

void RezeptManager::addRezept(const Rezept& r) {
    rezepte.append(r);
}

void RezeptManager::removeRezept(int index) {
    if (index >= 0 && index < rezepte.size())
        rezepte.removeAt(index);
}

void RezeptManager::editRezept(int index, const Rezept& neu) {
    if (index >= 0 && index < rezepte.size())
        rezepte[index] = neu;
}

QVector<Rezept> RezeptManager::getAlleRezepte() const {
    return rezepte;
}

QVector<Rezept> RezeptManager::filterRezepte(const QString& suchbegriff) const {
    QVector<Rezept> ergebnis;
    for (const Rezept& r : rezepte) {
        if (passtZuSuchbegriff(r, suchbegriff))
            ergebnis.append(r);
    }
    return ergebnis;
}

QVector<int> RezeptManager::filterIndizes(const QString& suchbegriff) const {
    QVector<int> ergebnis;
    for (int i = 0; i < rezepte.size(); ++i) {
        if (passtZuSuchbegriff(rezepte.at(i), suchbegriff))
            ergebnis.append(i);
    }
    return ergebnis;
}

bool RezeptManager::speichern(const QString& pfad, QString* fehler) const {
    QDir().mkpath(QFileInfo(pfad).absolutePath());

    QJsonArray arr;
    for (const Rezept& r : rezepte)
        arr.append(r.toJson());
    QJsonObject root{{"version", 1}, {"rezepte", arr}};


    QSaveFile f(pfad);
    if (!f.open(QIODevice::WriteOnly) ||
        f.write(QJsonDocument(root).toJson(QJsonDocument::Indented)) < 0 ||
        !f.commit()) {
        if (fehler) *fehler = f.errorString();
        return false;
    }
    return true;
}

bool RezeptManager::laden(const QString& pfad, QString* fehler) {
    QFile f(pfad);
    if (!f.exists())
        return true;

    if (!f.open(QIODevice::ReadOnly)) {
        if (fehler) *fehler = f.errorString();
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    f.close();

    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        QFile::remove(pfad + ".bak");
        f.rename(pfad + ".bak");
        if (fehler) *fehler = err.errorString();
        return false;
    }

    QVector<Rezept> geladen;
    const QJsonArray arr = doc.object().value("rezepte").toArray();
    for (const QJsonValue& v : arr) {
        if (v.isObject())
            geladen.append(Rezept::fromJson(v.toObject()));
    }
    rezepte = geladen;
    return true;
}
