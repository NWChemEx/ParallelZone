#ifndef _Parallel_HPP_
#define _Parallel_HPP_

#include "mpi.h"

class Parallel {
    int npi[3], taskidi[3];
    int* reqcnt;
    int* procNd;
    // MPI::Intracomm comm_i[3];
    // MPI::Group     group_i[3];
    // MPI::Request  **request;
    MPI_Comm comm_world;
    MPI_Comm comm_i[3];
    MPI_Group group_i[3];
    MPI_Request** request;
    MPI_Status** statuses;

public:
    int dim;

    /* Constructors */
    Parallel(MPI_Comm mpi_comm);

    ~Parallel();

    /* 2d proc geom constructor */
    void init2d(const int, const int);

    int is_master() { return (taskidi[0] == MASTER); }
}

#endif
