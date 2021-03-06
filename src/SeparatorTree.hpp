/*
 * STRUMPACK -- STRUctured Matrices PACKage, Copyright (c) 2014, The
 * Regents of the University of California, through Lawrence Berkeley
 * National Laboratory (subject to receipt of any required approvals
 * from the U.S. Dept. of Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE. This software is owned by the U.S. Department of Energy. As
 * such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * Developers: Pieter Ghysels, Francois-Henry Rouet, Xiaoye S. Li.
 *             (Lawrence Berkeley National Lab, Computational Research
 *             Division).
 *
 */
#ifndef SEPARATOR_TREE_HPP
#define SEPARATOR_TREE_HPP

#include <vector>
#include <unordered_map>
#include <stack>
#include <fstream>
#include "HSS/HSSPartitionTree.hpp"
#include "ETree.hpp"

namespace strumpack {

  template<typename integer_t> class Separator {
  public:
    Separator(integer_t _sep_end, integer_t _pa,
              integer_t _lch, integer_t _rch)
      : sep_end(_sep_end), pa(_pa), lch(_lch), rch(_rch) {}
    integer_t sep_end, pa, lch, rch;
  };

  // TODO rename to SuperNodalTree
  /**
   * Simple class to store a separator tree. A node in this tree
   * should always have 0 or 2 children.
   */
  template<typename integer_t> class SeparatorTree {
  public:
    // construct based on number of separators
    SeparatorTree(integer_t nr_nodes);
    // construct from on a vector of Separators, see above
    SeparatorTree(std::vector<Separator<integer_t>>& seps);
    // construct from an elimination tree
    SeparatorTree(std::vector<integer_t>& etree);
    virtual ~SeparatorTree();
    void broadcast(MPI_Comm comm) const;
    integer_t levels();
    integer_t level(integer_t i) const;
    integer_t root();
    void print() const;
    void printm(std::string name);
    void check();
    std::unique_ptr<SeparatorTree<integer_t>> subtree(integer_t p,
                                                      integer_t P);
    std::unique_ptr<SeparatorTree<integer_t>> toptree(integer_t P);

    inline integer_t separators() const { return _nbsep; }
    inline integer_t* sizes() const { return _sizes; }
    inline integer_t* pa() const { return _pa; }
    inline integer_t* lch() const { return _lch; }
    inline integer_t* rch() const { return _rch; }
    inline std::unordered_map<integer_t, HSS::HSSPartitionTree>& HSS_trees()
    { return _HSS_trees; }

  protected:
    integer_t _nbsep;
    integer_t* _sizes;
    integer_t* _pa;
    integer_t* _lch;
    integer_t* _rch;

    std::unordered_map<integer_t, HSS::HSSPartitionTree> _HSS_trees;

    inline integer_t size() const { return 4*_nbsep+1; }

  private:
    integer_t _root;
  };

  template<typename integer_t> SeparatorTree<integer_t>::SeparatorTree
  (integer_t nr_nodes) :
    _nbsep(nr_nodes), _sizes(new integer_t[4*_nbsep+1]), _pa(_sizes+_nbsep+1),
    _lch(_pa+_nbsep), _rch(_lch+_nbsep), _root(-1) {
  }

  template<typename integer_t>
  SeparatorTree<integer_t>::SeparatorTree
  (std::vector<Separator<integer_t>>& seps)
    : SeparatorTree<integer_t>(seps.size()) {
    _sizes[0] = 0;
    integer_t i = 0;
    for (const auto& s : seps) {
      _sizes[i+1] = s.sep_end;
      _pa[i] = s.pa;
      _lch[i] = s.lch;
      _rch[i] = s.rch;
      i++;
    }
    _root = -1;
    check();
  }

  // TODO combine small leafs
  template<typename integer_t>
  SeparatorTree<integer_t>::SeparatorTree(std::vector<integer_t>& etree) {
    integer_t n = etree.size();
    if (n == 0) {
      _nbsep = 0;
      _sizes = nullptr;
      _pa = nullptr;
      _lch = nullptr;
      _rch = nullptr;
      return;
    }
    // in etree a root has parent n, replace n with -1
    std::replace(etree.begin(), etree.end(), n, integer_t(-1));
    integer_t nr_roots =
      std::count(etree.begin(), etree.end(), integer_t(-1));
    // deal with multiple roots
    if (nr_roots > 1) {
      for (integer_t r=0; r<nr_roots-1; r++) {
        auto hi = etree.size() - 1;
        while (etree[hi] != -1) hi--;
        auto root_right = hi;
        hi--;
        while (etree[hi] != -1) hi--;
        integer_t max_p = etree.size();
        etree.push_back(-1);
        etree[root_right] = max_p;
        etree[hi] = max_p;
      }
    }

    // TODO the number of dummies can be computed as sum_node{max(0,
    // node.nr_children-2))

    // TODO this way of adding dummies creates chains!!

    auto new_n = etree.size();
    std::vector<integer_t> count(new_n, 0);
    std::vector<integer_t> etree_lchild(new_n, integer_t(-1));
    std::vector<integer_t> etree_rchild(new_n, integer_t(-1));
    for (size_t i=0; i<new_n; i++) {
      integer_t p = etree[i];
      if (p != -1) {
        count[p]++;
        switch (count[p]) {
        case 1:
          // node i is the first child of node p
          etree_lchild[p] = i;
          break;
        case 2:
          // node i is the second child of node p
          etree_rchild[p] = i;
          break;
        case 3:
          // node i is the third child of node p
          // make a new node with children the first two children of p,
          //     set this new node as the left child of p
          // make node i the right child of p
          integer_t max_p = etree.size();
          etree.push_back(max_p);
          etree_lchild.push_back(etree_lchild[p]);
          etree_rchild.push_back(etree_rchild[p]);
          etree_lchild[p] = max_p;
          etree_rchild[p] = i;
          count[p]--;
          break;
        }
      }
    }
    std::vector<Separator<integer_t>> seps;
    std::stack<integer_t,std::vector<integer_t> > s, l;
    s.push(std::distance(etree.begin(),
                         std::find(etree.begin(), etree.end(),
                                   integer_t(-1))));
    integer_t prev=-1;
    while (!s.empty()) {
      integer_t i = s.top();
      if (prev == -1 || etree_lchild[prev] == i || etree_rchild[prev] == i) {
        // moving down
        if (etree_lchild[i] != -1) // go down left
          s.push(etree_lchild[i]);
        else if (etree_rchild[i] != -1)
          s.push(etree_rchild[i]); // if no left, then go down right
      } else if (etree_lchild[i] == prev) {
        // moving up from the left,
        if (etree_rchild[i] != -1) {
          l.push(seps.size() - 1);
          s.push(etree_rchild[i]); // go down right
        }
      } else {
        // skip nodes that have only one child, this will group nodes
        // in fronts
        if ((etree_rchild[i] == -1 && etree_lchild[i] == -1) ||
            (etree_rchild[i] != -1 && etree_lchild[i] != -1)) {
          // two children, or no children
          auto pid = seps.size();
          seps.emplace_back((seps.empty()) ? 0 : seps.back().sep_end, -1,
                            (etree_lchild[i]!=-1) ? l.top() : -1,  // lch
                            (etree_rchild[i]!=-1) ? pid-1 : -1);   // rch
          if (etree_lchild[i] != -1) {
            seps[l.top()].pa = pid;
            l.pop();
          }
          if (etree_rchild[i] != -1) seps[pid-1].pa = pid;
        }
        // nodes >= n are empty separators introduced to avoid nodes
        // with three children, so do not count those when computing
        // separator size!
        if (i < n) seps.back().sep_end++;
        s.pop();
      }
      prev = i;
    }
    _nbsep = seps.size();
    _sizes = new integer_t[4*_nbsep+1];
    _pa = _sizes+_nbsep+1;
    _lch = _pa+_nbsep;
    _rch = _lch+_nbsep;
    _sizes[0] = 0;
    integer_t i = 0;
    for (const auto& s : seps) {
      _sizes[i+1] = s.sep_end;
      _pa[i] = s.pa;
      _lch[i] = s.lch;
      _rch[i] = s.rch;
      i++;
    }
    _root = -1;
    check();
  }

  template<typename integer_t> SeparatorTree<integer_t>::~SeparatorTree() {
    delete[] _sizes;
  }

  template<typename integer_t> void
  SeparatorTree<integer_t>::broadcast(MPI_Comm comm) const {
    MPI_Bcast(_sizes, size(), mpi_type<integer_t>(), 0, comm);
    // TODO broadcast the HSS_trees?
  }

  template<typename integer_t> integer_t SeparatorTree<integer_t>::levels() {
    if (_nbsep) {
      return level(root());
    } else return 0;
  }

  template<typename integer_t> integer_t
  SeparatorTree<integer_t>::level(integer_t i) const {
    assert(0 <= i && i <= _nbsep);
    integer_t lvl = 0;
    if (_lch[i] != -1) lvl = level(_lch[i]);
    if (_rch[i] != -1) lvl = std::max(lvl, level(_rch[i]));
    return lvl+1;
  }

  template<typename integer_t> integer_t SeparatorTree<integer_t>::root() {
    if (_root == -1) _root = std::find(_pa, _pa+_nbsep, -1) - _pa;
    return _root;
  }

  template<typename integer_t> void SeparatorTree<integer_t>::print() const {
    std::cout << "i\tpa\tlch\trch\tsep" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    for (integer_t i=0; i<_nbsep; i++)
      std::cout << i << "\t" << _pa[i] << "\t"
                << _lch[i] << "\t" << _rch[i] << "\t"
                << _sizes[i] << "/" << _sizes[i+1] << std::endl;
    std::cout << std::endl;
  }

  template<typename integer_t> void
  SeparatorTree<integer_t>::printm(std::string name) {
    check();
    float avg = 0;
    for (integer_t i=0; i<_nbsep; i++) avg += _sizes[i+1]-_sizes[i];
    avg /= _nbsep;
    integer_t empty = 0;
    for (integer_t i=0; i<_nbsep; i++)
      if (_sizes[i+1]-_sizes[i] == 0) empty++;
    int* subtree = new int[_nbsep];
    float* inbalance = new float[_nbsep];
    std::function<void(integer_t)> compute_subtree_size =
      [&](integer_t node) {
      subtree[node] = _sizes[node+1]-_sizes[node];
      if (_lch[node] != -1) {
        compute_subtree_size(_lch[node]);
        subtree[node] += subtree[_lch[node]];
      }
      if (_rch[node] != -1) {
        compute_subtree_size(_rch[node]);
        subtree[node] += subtree[_rch[node]];
      }
      inbalance[node] = 1.;
      if (_lch[node] != -1 && _rch[node] != -1)
        inbalance[node] =
          float(std::max(subtree[_rch[node]], subtree[_lch[node]])) /
          float(std::min(subtree[_rch[node]], subtree[_lch[node]]));
    };
    compute_subtree_size(root());
    float avg_inbalance = 0, max_inbalance = 0;
    for (integer_t i=0; i<_nbsep; i++) {
      avg_inbalance += inbalance[i];
      max_inbalance = std::max(max_inbalance, inbalance[i]);
    }
    avg_inbalance /= _nbsep;
    delete[] subtree;
    delete[] inbalance;
    std::ofstream file(name + ".m");
    file << "% Separator tree " << name << std::endl
         << "%   - nr nodes = " << _nbsep << std::endl
         << "%   - levels = " << levels() << std::endl
         << "%   - average node size = " << avg << std::endl
         << "%   - empty nodes = " << empty << std::endl
         << "%   - average inbalance = " << avg_inbalance << std::endl
         << "%   - max inbalance = " << max_inbalance << std::endl
         << std::endl;
    file << name << "_parent = [";
    for (integer_t i=0; i<_nbsep; i++) file << _pa[i]+1 << " ";
    file << "];" << std::endl;
    file << name << "_sizes = [";
    for (integer_t i=0; i<_nbsep; i++) file << _sizes[i+1]-_sizes[i] << " ";
    file << "];" << std::endl;
    file.close();
  }

  template<typename integer_t> void SeparatorTree<integer_t>::check() {
#if !defined(NDEBUG)
    if (_nbsep == 0) return;
    assert(std::count(_pa, _pa+_nbsep, -1) == 1); // 1 root
    auto mark = new bool[_nbsep];
    std::fill(mark, mark+_nbsep, false);
    std::function<void(integer_t)> traverse = [&](integer_t node) {
      mark[node] = true;
      if (_lch[node] != -1) traverse(_lch[node]);
      if (_rch[node] != -1) traverse(_rch[node]);
    };
    traverse(root());
    assert(std::count(mark, mark+_nbsep, false) == 0);
    delete[] mark;
    integer_t nr_leafs = 0;
    for (integer_t i=0; i<_nbsep; i++) {
      assert(_pa[i]==-1 || _pa[i] >= 0);
      assert(_pa[i] < _nbsep);
      assert(_lch[i] < _nbsep);
      assert(_rch[i] < _nbsep);
      assert(_lch[i]>=0 || _lch[i]==-1);
      assert(_rch[i]>=0 || _rch[i]==-1);
      if (_lch[i]==-1) { assert(_rch[i]==-1); }
      if (_rch[i]==-1) { assert(_lch[i]==-1); }
      if (_pa[i]!=-1) { assert(_lch[_pa[i]]==i || _rch[_pa[i]]==i); }
      if (_lch[i]==-1 && _rch[i]==-1) nr_leafs++;
    }
    assert(2*nr_leafs - 1 == _nbsep);
    for (integer_t i=0; i<_nbsep; i++) {
      if (_sizes[i+1] < _sizes[i]) {
        std::cout << i << std::endl;
        assert(false);
      };
    }
#endif
  }

  /** extract subtree p of P */
  template<typename integer_t> std::unique_ptr<SeparatorTree<integer_t>>
  SeparatorTree<integer_t>::subtree(integer_t p, integer_t P) {
    if (_nbsep == 0)
      return std::unique_ptr<SeparatorTree<integer_t>>
        (new SeparatorTree<integer_t>(0));
    auto mark = new bool[_nbsep];
    std::fill(mark, mark+_nbsep, false);
    mark[root()] = true;
    integer_t nr_subtrees = 1;
    std::function<void(integer_t)> find_subtree_roots = [&](integer_t node) {
      if (mark[node]) {
        if (nr_subtrees < P && _lch[node]!=-1 && _rch[node]!=-1) {
          mark[_lch[node]] = true;
          mark[_rch[node]] = true;
          mark[node] = false;
          nr_subtrees++;
        }
      } else {
        if (_lch[node]!=-1) find_subtree_roots(_lch[node]);
        if (_rch[node]!=-1) find_subtree_roots(_rch[node]);
      }
    };
    while (nr_subtrees < P && nr_subtrees < _nbsep)
      find_subtree_roots(root());
    integer_t sub_root = -1;
    nr_subtrees = 0;
    for (integer_t i=0; i<_nbsep; i++) {
      if (mark[i]) {
        if (nr_subtrees == p) {
          sub_root = i;
          break;
        }
        nr_subtrees++;
      }
    }
    delete[] mark;
    if (sub_root == -1)
      return std::unique_ptr<SeparatorTree<integer_t>>
        (new SeparatorTree<integer_t>(0));
    std::function<integer_t(integer_t)> count = [&](integer_t node) {
      integer_t c = 1;
      if (_lch[node] != -1) c += count(_lch[node]);
      if (_rch[node] != -1) c += count(_rch[node]);
      return c;
    };
    auto sub_size = count(sub_root);
    auto sub = std::unique_ptr<SeparatorTree<integer_t>>
      (new SeparatorTree<integer_t>(sub_size));
    if (sub_size == 0) return sub;
    std::function<void(integer_t,integer_t&)> fill_sub =
      [&](integer_t node, integer_t& id) {
      integer_t left_root = 0;
      if (_lch[node] != -1) {
        fill_sub(_lch[node], id);
        left_root = id-1;
      } else sub->_lch[id] = -1;
      if (_rch[node] != -1) {
        fill_sub(_rch[node], id);
        sub->_rch[id] = id-1;
        sub->_pa[id-1] = id;
      } else sub->_rch[id] = -1;
      if (_lch[node] != -1) {
        sub->_lch[id] = left_root;
        sub->_pa[left_root] = id;
      }
      sub->_sizes[id+1] = sub->_sizes[id] + _sizes[node+1] - _sizes[node];
      id++;
    };
    integer_t id = 0;
    sub->_sizes[0] = 0;
    fill_sub(sub_root, id);
    sub->_pa[sub_size-1] = -1;
    return sub;
  }

  /** extract the tree with the top 2*P-1 nodes, ie a tree with P leafs */
  template<typename integer_t> std::unique_ptr<SeparatorTree<integer_t>>
  SeparatorTree<integer_t>::toptree(integer_t P) {
    integer_t top_nodes = std::min(std::max(integer_t(0), 2*P-1), _nbsep);
    auto top = std::unique_ptr<SeparatorTree<integer_t>>
      (new SeparatorTree<integer_t>(top_nodes));
    auto mark = new bool[_nbsep];
    std::fill(mark, mark+_nbsep, false);
    mark[root()] = true;
    integer_t nr_leafs = 1;
    std::function<void(integer_t)> mark_top_tree = [&](integer_t node) {
      if (nr_leafs < P) {
        if (_lch[node]!=-1 && _rch[node]!=-1 &&
            !mark[_lch[node]] && !mark[_rch[node]]) {
          mark[_lch[node]] = true;
          mark[_rch[node]] = true;
          nr_leafs++;
        } else {
          if (_lch[node]!=-1) mark_top_tree(_lch[node]);
          if (_rch[node]!=-1) mark_top_tree(_rch[node]);
        }
      }
    };
    while (nr_leafs < P && nr_leafs < _nbsep)
      mark_top_tree(root());

    std::function<integer_t(integer_t)> sep_subtree = [&](integer_t node) {
      return (_lch[node] != -1) ? sep_subtree(_lch[node])
        : _sizes[node];
    };

    std::function<void(integer_t,integer_t&)> fill_top =
      [&](integer_t node, integer_t& tid) {
      auto mytid = tid;
      tid--;
      if (_rch[node]!=-1 && mark[_rch[node]]) {
        top->_rch[mytid] = tid;
        top->_pa[top->_rch[mytid]] = mytid;
        fill_top(_rch[node], tid);
      } else top->_rch[mytid] = -1;
      if (_lch[node]!=-1 && mark[_lch[node]]) {
        top->_lch[mytid] = tid;
        top->_pa[top->_lch[mytid]] = mytid;
        fill_top(_lch[node], tid);
      } else top->_lch[mytid] = -1;
      if (top->_rch[mytid] == -1) {
        top->_sizes[mytid] = sep_subtree(node);
        top->_sizes[mytid+1] = _sizes[node+1];
      }
      else
        top->_sizes[mytid+1] = top->_sizes[mytid]
          + _sizes[node+1] - _sizes[node];
    };
    integer_t tid = top_nodes-1;
    fill_top(root(), tid);
    top->_pa[top_nodes-1] = -1;
    top->_sizes[top_nodes] = _sizes[root()+1];
    delete[] mark;
    return top;
  }

  /**
   * Create a separator tree based on a matrix and a
   * permutation. First build the elimination tree, then postorder the
   * elimination tree. Then combine the postordering of the
   * elimination tree with the permutation. Build a separator tree
   * from the elimination tree. Set the inverse permutation.
   */
  template<typename integer_t> std::unique_ptr<SeparatorTree<integer_t>>
  build_sep_tree_from_perm(integer_t n, integer_t* ptr, integer_t* ind,
                           integer_t* perm, integer_t* iperm) {
    auto rlo = new integer_t[2*n+ptr[n]];
    auto rhi = rlo + n;
    auto pind = rhi + n;
    for (integer_t i=0; i<n; i++) {
      rlo[perm[i]] = ptr[i];
      rhi[perm[i]] = ptr[i+1];
    }
    for (integer_t j=0; j<n; j++)
      for (integer_t i=rlo[j]; i<rhi[j]; i++)
        pind[i] = perm[ind[i]];
    auto etree = spsymetree(rlo, rhi, pind, n);
    auto post = etree_postorder<integer_t>(etree);
    auto iwork = iperm;
    for (integer_t i=0; i<n; i++) iwork[post[i]] = post[etree[i]];
    for (integer_t i=0; i<n; i++) etree[i] = iwork[i];
    // product of perm and post
    for (integer_t i=0; i<n; i++) iwork[i] = post[perm[i]];
    for (integer_t i=0; i<n; i++) perm[i] = iwork[i];
    for (integer_t i=0; i<n; i++) iperm[perm[i]] = i;
    delete[] rlo;
    return std::unique_ptr<SeparatorTree<integer_t>>
      (new SeparatorTree<integer_t>(etree));  // build separator tree
  }

} // end namespace strumpack

#endif
