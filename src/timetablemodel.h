#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QDateTime>
#include "stationhandler.h"
#include "junat.h"

class TimeTableModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum TimeTableMembers {
        FillerChar = Qt::UserRole + 1,
        StationName,
        ScheduledTime,
        ActualTime,
        EstimateTime,
        DifferenceInMin
    };

    struct TimeTable {
        QString stopChar;
        QString stationName;
        QString scheduledTime;
        QString actualTime;
        QString estimateTime;
        QString differenceInMin;
    };

    TimeTableModel(QObject *parent = 0);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE void setPointer(Junat* p);
    Q_INVOKABLE void setStationPointer(StationHandler* p);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<TimeTable> m_timeTable;
    void buildTestTable();
    Junat* m_jna;
    StationHandler* m_stn;

public slots:
    void getNewTable();
};

#endif // TIMETABLEMODEL_H
