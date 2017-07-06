#include "timetablemodel.h"

TimeTableModel::TimeTableModel(QObject *parent)
{
    Q_UNUSED(parent);
    modelReady = false;
    m_jna = NULL;
    m_stn = NULL;
}

int TimeTableModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_timeTable.count();
}

QVariant TimeTableModel::data(const QModelIndex & index, int role) const {
    if (index.row() < 0 || index.row() > m_timeTable.count() ) {
        return QVariant();
    }

    if ( role == FillerChar ) {
        return m_timeTable.at(index.row()).stopChar;
    }
    else if ( role == StationName ) {
        return m_timeTable.at(index.row()).stationName;
    }
    else if ( role == ScheduledTime ) {
        return m_timeTable.at(index.row()).scheduledTime;
    }
    else if ( role == ActualTime ) {
        return m_timeTable.at(index.row()).actualTime;
    }
    else if ( role == EstimateTime ) {
        return m_timeTable.at(index.row()).estimateTime;
    }
    else if ( role == DifferenceInMin ) {
        return m_timeTable.at(index.row()).differenceInMin;
    }
    else if ( role == HasCause ) {
        return m_timeTable.at(index.row()).hasCause;
    }
    return QVariant();
}

QHash<int, QByteArray> TimeTableModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[FillerChar] = "fillerChar";
    roles[StationName] = "stationName";
    roles[ScheduledTime] = "scheduledTime";
    roles[ActualTime] = "actualTime";
    roles[EstimateTime] = "estimateTime";
    roles[DifferenceInMin] = "differenceInMin";
    roles[HasCause] = "hasCause";

    return roles;
}

void TimeTableModel::setPointer(Junat* p) {
    if ( m_jna == NULL ) {
#ifdef QT_QML_DEBUG
        qDebug() << "Setting Juna Pointer";
#endif
        QObject::connect(p, &Junat::TimeTableChanged, this, &TimeTableModel::getNewTable);
        m_jna = p;
#ifdef QT_QML_DEBUG
        qDebug() << "Pointer: " << m_jna->getTrainNr();
#endif
    }
}

void TimeTableModel::setStationPointer(StationHandler* p) {
    if ( m_stn == NULL ) {
        m_stn = p;
    }
}

int TimeTableModel::getPointers(void) {
    int tmp = 0;
    if ( m_stn != NULL ) {
        tmp = tmp + 1;
    }
    if ( m_jna != NULL ) {
        tmp = tmp + 2;
    }
    qDebug() << "Current pointers: " << tmp << " model ready: " << getModelReady();
    return tmp;
}

void TimeTableModel::getNewTable() {
    qDebug() << "timeTableModel: getNewTable()";
    int timeTableLength = m_jna->getTimeTableCount();
    TimeTable tTmp;

    if ( timeTableLength <= 0 ) {
        return;
    }

    QAbstractListModel::beginRemoveRows(QModelIndex(),0,m_timeTable.count());
    m_timeTable.clear();

    QAbstractListModel::endRemoveRows();

    emit QAbstractListModel::beginInsertRows(QModelIndex(), m_timeTable.count(), timeTableLength-1);
    for ( int i = 0; i < timeTableLength; i++ ) {
        const Junat::timeTableRow* jTmp = m_jna->getTimeTableRow(i);
        if (jTmp == NULL) {
            return;
        }
        tTmp = TimeTable();
        tTmp.actualTime = jTmp->actualTime.toLocalTime().toString("HH:mm");
        tTmp.differenceInMin = QString::number(jTmp->differenceInMinutes);
        tTmp.estimateTime = jTmp->liveEstimateTime.toLocalTime().toString("HH:mm");
        tTmp.scheduledTime = jTmp->scheduledTime.toLocalTime().toString("HH:mm");
        if ( m_stn != NULL ) {
            tTmp.stationName = m_stn->getStationName( jTmp->stationShortCode );
        }
        else {
            tTmp.stationName = jTmp->stationShortCode;
        }
        if (jTmp->commercialStop) {
            tTmp.stopChar = "O";
        }
        else {
            tTmp.stopChar = "|";
        }
        tTmp.hasCause = jTmp->causes.hasCause;

        m_timeTable.append(tTmp);
    }

    emit QAbstractListModel::endInsertRows();
}

QString TimeTableModel::getModelReady(void) const {
    if ( modelReady ) {
        return "true";
    }
    else {
        return "false";
    }
}

void TimeTableModel::setModelReady(QString s) {
    if ( s.toLower() == "true" ) {
        modelReady = true;
    }
    else {
        modelReady = false;
    }
    qDebug() << "timeTableModel set to " << modelReady;
    emit modelReadyChanged();
}
