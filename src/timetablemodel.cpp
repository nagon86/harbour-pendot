#include "timetablemodel.h"

// Default constructor
TimeTableModel::TimeTableModel(QObject *parent)
{
    Q_UNUSED(parent);
    m_jna = NULL;
    m_stn = NULL;
}

// Function to get list length
// Required by qml list model functionality
int TimeTableModel::rowCount(const QModelIndex & parent) const {
    Q_UNUSED(parent);
    return m_timeTable.count();
}

// Required by qml list model functionality
// QML utilizes this to get required data for each row and each parameter
QVariant TimeTableModel::data(const QModelIndex & index, int role) const {
    // Precaution to prevent indexing errors
    if (index.row() < 0 || index.row() > m_timeTable.count() ) {
        return QVariant();
    }

    // Get wanted information from proper classes with their getters
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
    else if ( role == AbsoluteIndex ) {
        return m_timeTable.at(index.row()).absoluteIndex;
    }
    return QVariant();
}

// Does the mapping for the data function
// Names in quotations are the ones used on qml side
QHash<int, QByteArray> TimeTableModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[FillerChar] = "fillerChar";
    roles[StationName] = "stationName";
    roles[ScheduledTime] = "scheduledTime";
    roles[ActualTime] = "actualTime";
    roles[EstimateTime] = "estimateTime";
    roles[DifferenceInMin] = "differenceInMin";
    roles[HasCause] = "hasCause";
    roles[AbsoluteIndex] = "absoluteIndex";

    return roles;
}

// Store pointer given by qml to class Junat
void TimeTableModel::setPointer(Junat* p) {
    if ( m_jna == NULL ) {
        // Connect time table changed signal to slot get new table
        // to trigger list model update when junat class refreshes data
        QObject::connect(p, &Junat::TimeTableChanged, this, &TimeTableModel::getNewTable);
        m_jna = p;
    }
}

// Store pointer given by qml to class Junat
void TimeTableModel::setStationPointer(StationHandler* p) {
    if ( m_stn == NULL ) {
        m_stn = p;
    }
}

void TimeTableModel::getNewTable() {
    int timeTableLength = m_jna->getTimeTableCount();
    TimeTable tTmp;

    if ( timeTableLength <= 0 ) {
        return;
    }

    // Delete old data from filtered table
    QAbstractListModel::beginRemoveRows(QModelIndex(),0,m_timeTable.count());
    m_timeTable.clear();
    QAbstractListModel::endRemoveRows();

    // Start inserting new rows to filtered timetable
    emit QAbstractListModel::beginInsertRows(QModelIndex(), m_timeTable.count(), timeTableLength-1);
    for ( int i = 0; i < timeTableLength; i++ ) {
        const Junat::timeTableRow* jTmp = m_jna->getTimeTableRow(i);

        // Something bad happened. Abort.
        if (jTmp == NULL) {
            return;
        }

        // Create new empty timetable entry
        tTmp = TimeTable();

        // Store all the data needed by the list model to internal struct
        tTmp.actualTime = jTmp->actualTime.toLocalTime().toString("HH:mm");
        tTmp.differenceInMin = QString::number(jTmp->differenceInMinutes);
        tTmp.estimateTime = jTmp->liveEstimateTime.toLocalTime().toString("HH:mm");
        tTmp.scheduledTime = jTmp->scheduledTime.toLocalTime().toString("HH:mm");

        // If station handler is defined. Try to get real station name.
        // Else use station short code
        if ( m_stn != NULL ) {
            tTmp.stationName = m_stn->getStationName( jTmp->stationShortCode );
        }
        else {
            tTmp.stationName = jTmp->stationShortCode;
        }

        // Convert boolean value to printable characters
        if (jTmp->commercialStop) {
            // Use 'O' for stations where train will stop
            tTmp.stopChar = "O";
        }
        else {
            // | signifies stations where train should not stop
            tTmp.stopChar = "|";
        }
        tTmp.hasCause = jTmp->causes.hasCause;
        tTmp.absoluteIndex = jTmp->absoluteIndex;

        // Add entry to internal data structure
        m_timeTable.append(tTmp);
    }

    emit QAbstractListModel::endInsertRows();
}
