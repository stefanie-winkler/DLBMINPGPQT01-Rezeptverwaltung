#ifndef REZEPTMANAGER_H
#define REZEPTMANAGER_H

#include <QVector>
#include <QString>
#include "rezept.h"

class RezeptManager {
public:
    void addRezept(const Rezept& r);
    void removeRezept(int index);
    void editRezept(int index, const Rezept& neu);
    QVector<Rezept> getAlleRezepte() const;
    QVector<Rezept> filterRezepte(const QString& suchbegriff) const;
    QVector<int> filterIndizes(const QString& suchbegriff) const;

    // Persistenz (JSON-Datei)
    bool speichern(const QString& pfad, QString* fehler = nullptr) const;
    bool laden(const QString& pfad, QString* fehler = nullptr);

private:
    QVector<Rezept> rezepte;
};

#endif
