#ifndef REZEPT_H
#define REZEPT_H

#include <QString>
#include <QStringList>
#include <QJsonObject>

class Rezept {
public:
    Rezept() = default;
    Rezept(const QString& titel, const QString& kategorie,
           const QStringList& zutaten, const QString& zubereitung,
           int zubereitungszeitMinuten)
        : titel(titel), kategorie(kategorie), zutaten(zutaten),
          zubereitung(zubereitung), zubereitungszeitMinuten(zubereitungszeitMinuten) {}

    QString getTitel() const { return titel; }
    QString getKategorie() const { return kategorie; }
    QStringList getZutaten() const { return zutaten; }
    QString getZubereitung() const { return zubereitung; }
    int getZubereitungszeit() const { return zubereitungszeitMinuten; }

    void setTitel(const QString& t) { titel = t; }
    void setKategorie(const QString& k) { kategorie = k; }
    void setZutaten(const QStringList& z) { zutaten = z; }
    void setZubereitung(const QString& z) { zubereitung = z; }
    void setZubereitungszeit(int min) { zubereitungszeitMinuten = min; }


    QJsonObject toJson() const;
    static Rezept fromJson(const QJsonObject& o);

private:
    QString titel;
    QString kategorie;
    QStringList zutaten;
    QString zubereitung;
    int zubereitungszeitMinuten = 0;
};

#endif
