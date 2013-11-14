#ifndef EVENT_H
#define EVENT_H

#include <QWidget>
#include <QMap>
#include "message.h"

class Partition;

class Event
{
public:
    Event(unsigned long long _enter, unsigned long long _exit, int _function,
          int _process, int _step);
    ~Event();
    void addMetric(QString name, long long event_value, long long aggregate_value = 0);

    bool operator<(const Event &);
    bool operator>(const Event &);
    bool operator<=(const Event &);
    bool operator>=(const Event &);
    bool operator==(const Event &);

    class MetricPair {
    public:
        MetricPair(long long _e, long long _a)
            : event(_e), aggregate(_a) {}

        long long event;
        long long aggregate;
    };

    QVector<Event *> * parents;
    QVector<Event *> * children;
    QVector<Message *> * messages;
    QMap<QString, MetricPair *> * metrics; // Lateness or Counters etc

    Event * caller;
    QVector<Event *> * callees;

    Partition * partition;
    Event * comm_next;
    Event * comm_prev;
    bool is_recv;

    Event * last_send;
    Event * next_send;
    QList<Event *> * last_recvs;

    unsigned long long enter;
    unsigned long long exit;
    int function;
    int process;
    int step;
    int depth;

    //enum mpi_class_t { WAITALL, COLLECTIVE, OTHER };

};

#endif // EVENT_H
