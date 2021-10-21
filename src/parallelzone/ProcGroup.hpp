#ifndef _PROCGROUP_HPP
#define _PROCGROUP_HPP

#include "ParallelZoneMPI.hpp"

#include <map>
#include <string>
#include <vector>

namespace ParallelZone {

class Parallel;

class ProcGroup {

public:

  ~ProcGroup();

  std::string myNodeName() const;

  // Returns the total number of MPI nodes in this MPI session.
  int nNodes() const { return m_all_proc_names.size(); }
  int myNode() const { return m_all_proc_indexs[m_rank]; }

  // Returns the total number of MPI rank in the node MPI session.
  int myNode_nRanks() const { return m_node_nRanks; }
  int myNode_myRank() const { return m_node_rank; }
  
  // Returns the total number of MPI ranks in this MPI session.
  int nRanks() const { return m_nRanks; }
  int myRank() const { return m_rank; }

  // Communicators
  MPI_Comm getComm() const { return m_comm; }
  MPI_Comm getNodeComm() const { return m_node_comm; }

  MPI_Comm getGlobalComm( int comm_idx ) const
  {
    if (comm_idx == -1 || m_threads <= 1) {
      return m_comm;
    } else {
      return m_global_comms[comm_idx];
    }
  }

  void setGlobalComm( int num_comms ) const;

  // Utilities for getting node based information.
  int         getNodeIndexFromRank( unsigned int rank ) const;
  std::string getNodeNameFromRank( unsigned int rank ) const;
  std::string getNodeName( unsigned int node ) const;

private:

  friend class Parallel;

  // Can only be called from Parallel
  ProcGroup( const ProcGroup * parent
                ,       MPI_Comm         comm
                ,       int              rank
                ,       int              size
                ,       int              threads
                );

  // Eliminate copy, assignment and move
  ProcGroup( const ProcGroup & )            = delete;
  ProcGroup& operator=( const ProcGroup & ) = delete;
  ProcGroup( ProcGroup && )                 = delete;
  ProcGroup& operator=( ProcGroup && )      = delete;

  // Parent processor.
  const ProcGroup          * m_parent_group{nullptr};

  // Communicators.
  MPI_Comm                        m_comm{0};
  MPI_Comm                        m_node_comm{0};
  mutable std::vector<MPI_Comm>   m_global_comms;

  int m_node_rank{-1};  // MPI rank of this process relative to the node.
  int m_node_nRanks{0}; // Total number of MPI Ranks relative to the node.

  // Rank, node, thread, and name details.
  int m_rank{-1};   // MPI rank of this process.
  int m_nRanks{0};  // Total number of MPI Ranks.

  int m_threads{0};

  // For storing all the processor names so to provide a mapping from
  // the name to an index from any rank.
  typedef char procName_t[MPI_MAX_PROCESSOR_NAME+1];

  // For each rank store an index to it's node.
  std::vector< unsigned int > m_all_proc_indexs;
  // For each node store it's processor name.
  std::vector< std::string > m_all_proc_names;
};

}  // namespace ParallelZone

#endif // end _PROCGROUP_HPP
