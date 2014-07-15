#ifndef COLLECTIVEEVENT_H
#define COLLECTIVEEVENT_H

#include "commevent.h"

class CollectiveEvent : public CommEvent
{
public:
    CollectiveEvent(unsigned long long _enter, unsigned long long _exit,
                    int _function, int _process, int _phase,
                    CollectiveRecord * _collective);
    ~CollectiveEvent();
    bool isP2P() { return false; }
    bool isReceive() { return false; }
    virtual bool isCollective() { return true; }
    void fixPhases();
    void initialize_strides(QList<CommEvent *> * stride_events,
                            QList<CommEvent *> * recv_events);

    void addComms(QSet<CommBundle *> * bundleset) { bundleset->insert(collective); }
    CollectiveRecord * getCollective() { return collective; }
    QSet<Partition *> * mergeForMessagesHelper();

    ClusterEvent * createClusterEvent(QString metric, long long divider);
    void addToClusterEvent(ClusterEvent * ce, QString metric,
                           long long divider);

    CollectiveRecord * collective;

private:
    void set_stride_relationships();
};

#endif // COLLECTIVEEVENT_H