#ifndef LATTICE_NODE_H
#define LATTICE_NODE_H

#include "llvm/Support/raw_ostream.h"

class LatticeNode {

public:

	/*
	* CP is short for constant propagation
	* AE is short for Avaliable Expressionn
	* RA is short for Range Analysis
	* PA is short for intra-procedural pointer analysis 
	*/
	enum latticeKind{CP, AE, RA, PA};

	bool isbottom;
	bool istop;

	latticeKind lkind;

public:

	LatticeNode(latticeKind mykind) {
		lkind = mykind;
		isbottom = false;
		istop = false;
	}

	LatticeNode(latticeKind mykind, bool b, bool t) {
		isbottom = b;
		istop = t;
		lkind = mykind;
	}

	latticeKind getKind() const {
		return lkind;
	}

	bool getIsTop() const {
		return istop;
	}

	bool getIsBottom() const {
		return isbottom;
	}

	virtual LatticeNode* join(LatticeNode* in) {return in;}

	virtual bool equal (LatticeNode*) {return false;}

	virtual void PrintInfo() {}
};

#endif
