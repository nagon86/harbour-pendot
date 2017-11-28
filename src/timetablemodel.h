#ifndef TIMETABLEMODEL_H
#define TIMETABLEMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QDateTime>
#include "stationhandler.h"
#include "junat.h"

// QML List model to easily display Train timetable in a list

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
        DifferenceInMin,
        HasCause,
        AbsoluteIndex
    };

    struct TimeTable {
        QString stopChar;
        QString stationName;
        QString scheduledTime;
        QString actualTime;
        QString estimateTime;
        QString differenceInMin;
        int absoluteIndex;
        bool hasCause;
    };

    // Constructor
    TimeTableModel(QObject *parent = 0);

    // Function to get list length
    // Required by qml list model functionality
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    // Required by qml list model functionality
    // QML utilizes this to get required data for each row and each parameter
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    // Functions to link this class to junat and station handler classes
    // to get all required information out.
    Q_INVOKABLE void setPointer(Junat* p);
    Q_INVOKABLE void setStationPointer(StationHandler* p);

protected:
    // QML List model way of mapping names to values
    QHash<int, QByteArray> roleNames() const;

signals:

private:
    QList<TimeTable> m_timeTable;
    Junat* m_jna;
    StationHandler* m_stn;

public slots:
    // Function to get and filter train time table
    void getNewTable();
};

#endif // TIMETABLEMODEL_H
