#include "../../include/flowfunction/AEFlowFunction.h"

vector<LatticeNode*> AEFlowFunction::operator()(Instruction *ins, vector<LatticeNode*> in) {
	//errs() << "Enter AE flow function\n";
	ins->print(errs());
	errs() << "\n";

	Realin.clear();
	for(vector<LatticeNode*>::iterator it = in.begin(); it != in.end(); it++) {
		AELatticeNode* node = dyn_cast<AELatticeNode>(*it);
		Realin.push_back(node);
	}

	out.clear();

	this->visit(ins);

	//errs() << "Leave AE flow function\n";
	return out;
}

void AEFlowFunction::visitCallInst(CallInst &ci) {
	errs() << "Enter Call Instruction\n";
	AELatticeNode *ae = new AELatticeNode(true, false);
	out.push_back(ae);
	errs() << "Leave Call Instruction\n";
}

void AEFlowFunction::visitBranchInst(BranchInst &bi) {
	errs() << "Enter Branch Instruction\n";
	AELatticeNode *ae = new AELatticeNode(*(Realin.back()));
	out.push_back(ae);
	errs() << "Leave Branch Instruction\n";
}

void AEFlowFunction::visitCmpInst(CmpInst &ci) {
	errs() << "Enter cmp Instruction\n";
	AELatticeNode *ae = new AELatticeNode(*(Realin.back()));
	out.push_back(ae);
	errs() << "Leave cmp Instruction\n";
}

void AEFlowFunction::visitBinaryOperator(BinaryOperator &bo) {
	errs() << "Enter Binary Operator\n";
	map<Value*, Instruction*> node = Realin.back() -> node;
	if(node.count(&bo) > 0) {
		//While , For will trigger second and more visiting.
		errs() << "AE Binary Operator, redefined\n";
		AELatticeNode *ae = new AELatticeNode(*(Realin.back()));
		out.push_back(ae);
		return;
	}

	//continue process
	Value* key = &bo;
	Instruction* value;
	bool found = false;
	string opname = bo.getOpcodeName();
	
	for(map<Value*, Instruction*>::iterator it = node.begin(); it != node.end(); ++it) {
		Instruction* cache_instruction = it->second;
		if(cache_instruction->isIdenticalToWhenDefined(&bo)) {
			found = true;
			value = cache_instruction;
			break;
		}
		
		/* 
		 * special case
		 * handle case like 1+2 / 2+1
		 */
		if(opname == "add") {
			bo.swapOperands();
			if(cache_instruction->isIdenticalToWhenDefined(&bo)) {
				found = true;
				value = cache_instruction;
				bo.swapOperands();
				break;
			}
			bo.swapOperands();
		}
	}

	if(found) {
		node[key] = value;
	} else {
		node[key] = dyn_cast<Instruction>(key);
	}

	AELatticeNode *ae = new AELatticeNode(false, false, node);
 	out.push_back(ae);
	errs() << "Leave Binary Operator\n";
}

void AEFlowFunction::visitPHINode(PHINode &phi) {
	errs() << "Enter PHI Node\n";
	
/*
	int in_edges_num = phi.getNumIncomingValues();
	
	assert(in_edges_num == 2);

	Value* left = phi.getIncomingValue(0);
	Value* right = phi.getIncomingValue(1);
	
	AELatticeNode *node1 = Realin[0];
	AELatticeNode *node2 = Realin[1];
	
	map<Value*, Instruction*> in1 = node1->node;
	map<Value*, Instruction*> in2 = node2->node;
	
	assert(in1.count(left) > 0 && in2.count(right) > 0);
	
	bool found(false);
	Instruction *ins;
	//special case in teacher's slides
	if(in1[left]->isIdenticalToWhenDefined(in2[right])) {
		ins = in1[left];
		found = true;
	}
*/
	//At first, out = in_1 join in_2 join ... join in_n
	while(Realin.size() > 1) {
		AELatticeNode *node1 = Realin.back();
		Realin.pop_back();
		AELatticeNode *node2 = Realin.back();
		Realin.pop_back();
		AELatticeNode *joinnode = dyn_cast<AELatticeNode>(node1->join(node2));
		Realin.push_back(joinnode);
	}

	/*
	if(found) {
		Value* key = &phi;
		Realin[0]->node[key] = ins;
	}*/

	AELatticeNode *ae = new AELatticeNode(*(Realin.back()));
	out.push_back(ae);
	errs() << "Leave PHI Node\n";
}
