#include "tripmanager.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>

bool TripManager::saveTrip(QString source,
                           QString destination,
                           QString distance,
                           QString mileage,
                           QString fuelPrice,
                           QString fuelCost,
                           QString time,
                           QString route)
{
    QFile file("trip_history.txt");

    if(!file.open(QIODevice::Append | QIODevice::Text))
        return false;

    QTextStream out(&file);

    out<<"=========================\n";
    out<<"Date : "<<QDateTime::currentDateTime().toString()<<"\n";
    out<<"Source : "<<source<<"\n";
    out<<"Destination : "<<destination<<"\n";
    out<<distance<<"\n";
    out<<"Mileage : "<<mileage<<"\n";
    out<<"Fuel Price : "<<fuelPrice<<"\n";
    out<<fuelCost<<"\n";
    out<<time<<"\n";
    out<<"Route\n"<<route<<"\n\n";

    file.close();

    return true;
}