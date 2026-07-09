#ifndef TRIPMANAGER_H
#define TRIPMANAGER_H

#include <QString>

class TripManager
{
public:
    bool saveTrip(QString source,
                  QString destination,
                  QString distance,
                  QString mileage,
                  QString fuelPrice,
                  QString fuelCost,
                  QString time,
                  QString route);
};

#endif