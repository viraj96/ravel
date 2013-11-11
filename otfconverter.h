#ifndef OTFCONVERTER_H
#define OTFCONVERTER_H

#include "otfimporter.h"
#include "rawtrace.h"
#include "trace.h"
#include "general_util.h"
#include <QStack>
#include <QSet>
#include <cmath>

class OTFConverter
{
public:
    OTFConverter();
    ~OTFConverter();
    Trace * importOTF(QString filename);

private:
    void matchEvents();

    void matchMessages();
    Event * search_child_ranges(QVector<Event *> * children, unsigned long long int time);
    Event * find_comm_event(Event * evt, unsigned long long int time);

    void chainCommEvents();

    void initializePartitions();
    void initializePartitionsWaitall();
    void mergeByMessages();
    void mergeCycles();
    void mergeByLeap();
    class RecurseInfo {
    public:
        RecurseInfo(Partition * p, Partition * c, QList<QList<Partition *> *> * cc, int i)
            : part(p), child(c), children(cc), cIndex(i) {}
        Partition * part;
        Partition * child;
        QList<Partition *> * children;
        int cIndex;
    };
    void set_partition_dag();
    void set_dag_steps();
    Partition * mergePartitions(Partition * p1, Partition * p2);
    void strong_connect_loop(Partition * part, QStack<Partition *> * stack,
                            QList<QList<Partition *> *> * children, int cIndex,
                            QStack<RecurseInfo *> * recurse, QList<Partition *> * components);
    int strong_connect_iter(Partition * partition, QStack<Partition *> * stack,
                            QList<QList<Partition *> *> * components, int index);
    QList<QList<Partition *> *> * tarjan();


    RawTrace * rawtrace;
    Trace * trace;
    int mpi_group;

    QList<Partition * > * partitions;
    QList<Partition * > * dag_entries;
    QVector<QList<Event *> *> * mpi_events;
    QList<Event *> * send_events;
    QMap<int, QSet<Partition *> *> * dag_step_dict;
};

#endif // OTFCONVERTER_H