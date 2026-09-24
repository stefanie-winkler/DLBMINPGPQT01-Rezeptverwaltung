#include "rezept.h"
#include <QJsonArray>

QJsonObject Rezept::toJson() const {
    return QJsonObject{
        {"titel", titel},
        {"kategorie", kategorie},
        {"zutaten", QJsonArray::fromStringList(zutaten)},
        {"zubereitung", zubereitung},
        {"zeit", zubereitungszeitMinuten}
    };
}

Rezept Rezept::fromJson(const QJsonObject& o) {
    QStringList z;
    const QJsonArray arr = o.value("zutaten").toArray();
    for (const QJsonValue& v : arr)
        z << v.toString();

    return Rezept(o.value("titel").toString(),
                  o.value("kategorie").toString(),
                  z,
                  o.value("zubereitung").toString(),
                  o.value("zeit").toInt(0));
}
