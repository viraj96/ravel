//////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014, Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory.
//
// This file is part of Ravel.
// Written by Kate Isaacs, kisaacs@acm.org, All rights reserved.
// LLNL-CODE-663885
//
// For details, see https://github.com/scalability-llnl/ravel
// Please also see the LICENSE file for our notice and the LGPL.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License (as published by
// the Free Software Foundation) version 2.1 dated February 1999.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms and
// conditions of the GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//////////////////////////////////////////////////////////////////////////////
#ifndef TRACE_H
#define TRACE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QVector>
#include <QQueue>
#include <QStack>
#include <QSharedPointer>
#include <QElapsedTimer>

#include "importoptions.h"

class Partition;
class Gnome;
class Event;
class CommEvent;
class Function;
class EntityGroup;
class PrimaryEntityGroup;
class OTFCollective;
class CollectiveRecord;

class Trace : public QObject
{
    Q_OBJECT
public:
    Trace(int nt, int np);
    ~Trace();

    void preprocess(ImportOptions * _options);
    void preprocessFromSaved();
    void partition();
    void assignSteps();
    void gnomify();
    void mergePartitions(QList<QList<Partition *> *> * components);
    Event * findEvent(int entity, unsigned long long time);

    QString name;
    QString fullpath;
    int num_entities;
    int num_application_entities; // debug
    int num_pes;
    int units;
    bool use_aggregates;
    qint64 totalTime;

    QList<Partition *> * partitions;
    QList<QString> * metrics;
    QMap<QString, QString> * metric_units;
    QList<Gnome *> * gnomes;

    // Processing options
    ImportOptions options;

    // Below set by OTFConverter
    QMap<int, QString> * functionGroups;
    QMap<int, Function *> * functions;

    QMap<int, PrimaryEntityGroup *> * primaries;
    PrimaryEntityGroup * processingElements;
    QMap<int, EntityGroup *> * entitygroups;
    QMap<int, OTFCollective *> * collective_definitions;

    QMap<unsigned long long, CollectiveRecord *> * collectives;
    QVector<QMap<unsigned long long, CollectiveRecord *> *> * collectiveMap;

    QVector<QVector<Event *> *> * events; // This is going to be by entities
    QVector<QVector<Event *> *> * roots; // Roots of call trees per pe

    int mpi_group; // functionGroup index of "MPI" functions

    int global_max_step; // largest global step
    QList<Partition * > * dag_entries; // Leap 0 in the dag
    QMap<int, QSet<Partition *> *> * dag_step_dict; // Map leap to partition

    // This is for aggregate event reporting... lists all functions
    // and how much time was spent in each
    class FunctionPair {
    public:
        FunctionPair(int _f, long long int _t)
            : fxn(_f), time(_t) {}
        FunctionPair()
            : fxn(0), time(0) {}
        FunctionPair(const FunctionPair &fp)
        {
            fxn = fp.fxn;
            time = fp.time;
        }

        // Backwards for greatest to lease
        bool operator<(const FunctionPair &fp) const { return time > fp.time; }

        int fxn;
        long long int time;
    };
    QList<FunctionPair> getAggregateFunctions(CommEvent *evt);

signals:
    // This is for progress bars
    void updatePreprocess(int, QString);
    void updateClustering(int);
    void startClustering();

private:
    // Link the comm events together by order
    void chainCommEvents();

    // Partition Dag
    void set_partition_dag();
    void set_dag_steps();
    void set_dag_entries();
    void clear_dag_step_dict();

    // Partitioning process
    void mergeForMessages();
    void mergeForMessagesHelper(Partition * part, QSet<Partition *> * to_merge,
                                QQueue<Partition *> * to_process);
    void mergeCycles();
    void mergeByLeap();
    void mergeGlobalSteps(); // Use after global steps are set, needs fixing
    class RecurseInfo {  // For Tarjan
    public:
        RecurseInfo(Partition * p, Partition * c, QList<Partition *> * cc, int i)
            : part(p), child(c), children(cc), cIndex(i) {}

        Partition * part;
        Partition * child;
        QList<Partition *> * children; // Used later
        int cIndex;
    };

    // Tarjan
    void strong_connect_loop(Partition * part, QStack<Partition *> * stack,
                            QList<Partition *> * children, int cIndex,
                            QStack<QSharedPointer<RecurseInfo> > * recurse,
                             QList<QList<Partition *> *> * components);
    int strong_connect_iter(Partition * partition, QStack<Partition *> * stack,
                            QList<QList<Partition *> *> * components, int index);
    QList<QList<Partition *> *> * tarjan();

    // Steps and metrics
    void mergeForEntryRepair(bool entries = true);
    void mergeForCharmLeaps();
    void forcePartitionDag();
    void finalizeEntityEventOrder();
    void set_global_steps();
    void calculate_lateness();
    void calculate_differential_lateness(QString metric_name, QString base_name);
    void calculate_partition_lateness();
    void calculate_partition_duration();
    void calculate_partition_metrics();

    // For debugging
    void output_graph(QString filename, bool byparent = false);
    void verify_partitions();
    void print_partition_info(QString message, QString graph_name = "",
                              bool partition_verify = false,
                              bool partition_count = false);

    // Extra metrics somewhat for debugging
    void setGnomeMetric(Partition * part, int gnome_index);
    void addPartitionMetric();

    // Find functions inside aggregate function
    long long int getAggregateFunctionRecurse(Event * evt,
                                              QMap<int, FunctionPair> * fpMap,
                                              unsigned long long start,
                                              unsigned long long stop);

    bool isProcessed; // Partitions exist

    QElapsedTimer totalTimer;

    static const bool debug = false;
    static const int partition_portion = 25;
    static const int lateness_portion = 45;
    static const int steps_portion = 30;
    static const QString collectives_string;
};

#endif // TRACE_H
