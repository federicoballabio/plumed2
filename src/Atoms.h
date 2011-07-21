#ifndef __PLUMED_Atoms_h
#define __PLUMED_Atoms_h

#include "PlumedCommunicator.h"
#include "Tensor.h"
#include "Units.h"
#include <vector>
#include <set>
#include <cassert>
#include <map>
#include <string>

namespace PLMD{

class MDAtomsBase;
class PlumedMain;
class ActionAtomistic;
class ActionWithVirtualAtom;

/// Class containing atom related quantities from the MD code.
/// IT IS STILL UNDOCUMENTED. IT PROBABLY NEEDS A STRONG CLEANUP
class Atoms
{
  friend class ActionAtomistic;
  friend class GenericWholeMolecules;
  friend class ActionWithVirtualAtom;
  int natoms;
  std::vector<Vector> positions;
  std::vector<Vector> forces;
  std::vector<double> masses;
  std::vector<double> charges;
  std::vector<ActionWithVirtualAtom*> virtualAtomsActions;
  Tensor box;
  Tensor virial;
  double energy;
  bool   collectEnergy;

  std::map<std::string,std::vector<unsigned> > groups;

  void resizeVectors(unsigned);

  std::vector<int> fullList;
  
  MDAtomsBase* mdatoms;

  PlumedMain & plumed;

  Units MDUnits;
  Units units;

  double timestep;
  double forceOnEnergy;

  std::vector<const ActionAtomistic*> actions;
  std::vector<int>    gatindex;

  class DomainDecomposition:
    public PlumedCommunicator
  {
  public:
    bool on;
    std::vector<int>    g2l;

    std::vector<PlumedCommunicator::Request> mpi_request_positions;
    std::vector<PlumedCommunicator::Request> mpi_request_index;
    
    std::vector<double> positionsToBeSent;
    std::vector<double> positionsToBeReceived;
    std::vector<int>    indexToBeSent;
    std::vector<int>    indexToBeReceived;
    operator bool(){return on;};
    DomainDecomposition():
      on(false)
      {};
    void enable(PlumedCommunicator& c);
  };

  DomainDecomposition dd;

public:

  Atoms(PlumedMain&plumed);
  ~Atoms();

  void init();

  void share();
  void wait();
  void updateForces();

  void setRealPrecision(int);
  int  getRealPrecision()const;

  void setTimeStep(void*);
  double getTimeStep()const;

  void setNatoms(int);
  const int & getNatoms()const;

  void setCollectEnergy(bool b){collectEnergy=b;};

  void setDomainDecomposition(PlumedCommunicator&);
  void setAtomsGatindex(int*);
  void setAtomsContiguous(int);
  void setAtomsNlocal(int);

  void setEnergy(void*);
  void setBox(void*);
  void setVirial(void*);
  void setPositions(void*);
  void setPositions(void*,int);
  void setForces(void*);
  void setForces(void*,int);
  void setMasses(void*);
  void setCharges(void*);

  void MD2double(const void*m,double&d)const;
  void double2MD(const double&d,void*m)const;

  void createFullList(int*);
  void getFullList(int**);
  void clearFullList();

  void add(const ActionAtomistic*);
  void remove(const ActionAtomistic*);

  double getEnergy()const{assert(collectEnergy);return energy;};

  void setMDEnergyUnits(double d){MDUnits.energy=d;};
  void setMDLengthUnits(double d){MDUnits.length=d;};
  void setMDTimeUnits(double d){MDUnits.time=d;};
  const Units& getMDUnits(){return MDUnits;};
  void setUnits(const Units&u){units=u;};
  const Units& getUnits(){return units;};
  void updateUnits();

  unsigned int addVirtualAtom(ActionWithVirtualAtom*);
  void removeVirtualAtom(ActionWithVirtualAtom*);
  void insertGroup(const std::string&name,const std::vector<unsigned>&a);
  void removeGroup(const std::string&name);
};

inline
const int & Atoms::getNatoms()const{
  return natoms;
}


}
#endif
