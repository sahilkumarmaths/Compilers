#include "AST.h"

using namespace std;

Address *BoolExpr::genCode() {
  Label yesLabel, noLabel, nextLabel;
  genJmpCode(yesLabel, noLabel);
  Address *result = new TempAddress;
  cout << yesLabel << ":\t" << *result << " = 1" << endl;
  cout << "\tgoto " << nextLabel << endl;
  cout << noLabel << ":\t" << *result << " = 0" << endl;
  cout << nextLabel << ":" << endl;
  return result;
}

void ArithExpr::genJmpCode(const Label& yesTarget, const Label& noTarget) {
  Address *result = genCode();
  cout << "\tif " << *result << " != 0 goto " << yesTarget << endl;
  cout << "\tgoto " << noTarget << endl;
  delete result;
}

void OrExpr::genJmpCode(const Label& yesTarget, const Label& noTarget) {
  Label rightLabel;
  left->genJmpCode(yesTarget,rightLabel);
  cout << rightLabel << ":" << endl;
  right->genJmpCode(yesTarget,noTarget);
}

void AndExpr::genJmpCode(const Label& yesTarget, const Label& noTarget) {
  Label rightLabel;
  left->genJmpCode(rightLabel,noTarget);
  cout << rightLabel << ":" << endl;
  right->genJmpCode(yesTarget,noTarget);
}

void CmpExpr::genJmpCode(const Label& yesTarget, const Label& noTarget) {
  Address *leftResult = left->genCode();
  Address *rightResult = right->genCode();
  cout << "\tif " << *leftResult << " " << op << " " << *rightResult 
       << " goto " << yesTarget << endl;
  cout << "\tgoto " << noTarget << endl;
  delete leftResult;
  delete rightResult;
}

Address *BinaryArithExpr::genCode() {
  Address *leftResult = left->genCode();
  Address *rightResult = right->genCode();
  Address *result = new TempAddress;
  cout << "\t" << *result << " = " << *leftResult 
       << " " << op << " " << *rightResult << endl;
  delete leftResult;
  delete rightResult;
  return result;
}

Address *UnaryArithExpr::genCode() {
  Address *operand = expr->genCode();
  Address *result = new TempAddress;
  cout << "\t" << *result << " = " << op << " " << *operand << endl;
  return result;
}

CallExpr::~CallExpr() {
  delete name;
  for (vector<Expr*>::iterator i = args->begin(); i != args->end(); ++i)
    delete *i;
  delete args;
}

Address *CallExpr::genCode() {
  for (vector<Expr*>::iterator i = args->begin(); 
       i != args->end(); ++i) {
    Address *addr = (*i)->genCode();
    cout << "\tparam " << *addr << endl;
    delete addr;
  }
  Address *result = new TempAddress;
  cout << "\t" << *result << " = call " << *name << ", " 
       << args->size() << endl;
  return result;
}

Address *ArrayLoadExpr::genCode() {
  Address *e = (*indices)[0]->genCode();
  Address *res = new TempAddress;
  cout << "\t" << *res << " = " << *name << "[" << *e << "]" << endl;
  return res;
  // XXX handle multi-dimensional array
}


void AssignStmt::genCode(const Label& next) {
  Address *result = expr->genCode();
  cout << "\t" << *name << " = " << *result << endl;
  delete result;
}

CallStmt::~CallStmt() {
  delete name;
  for (vector<Expr*>::iterator i = args->begin(); i != args->end(); ++i)
    delete *i;
  delete args;
}
 
void CallStmt::genCode(const Label& next) {
  for (vector<Expr*>::iterator i = args->begin(); i != args->end(); ++i) {
    Address *addr = (*i)->genCode();
    cout << "\tparam " << *addr << endl;
    delete addr;
  }
  cout << "\tcall " << *name << ", " << args->size() << endl;
}

BlockStmt::~BlockStmt() {
  for (vector<Stmt*>::iterator i = stmts->begin(); i != stmts->end(); ++i)
    delete *i;
  delete stmts;
}

void BlockStmt::genCode(const Label& next) {
  const unsigned n = stmts->size();
  for (unsigned i = 0; i < n; i++) {
    Stmt *stmt = (*stmts)[i];
    if (i < n-1) {
      Label nxt;
      stmt->genCode(nxt);
      cout << nxt << ":" << endl;
    } else
      stmt->genCode(next);
  }
}

void IfStmt::genCode(const Label& next) {
  Label bodyLabel;
  cond->genJmpCode(bodyLabel,next);
  cout << bodyLabel << ":" << endl;
  body->genCode(next);
}

void IfElseStmt::genCode(const Label& next) {
  Label thenLabel, elseLabel;
  cond->genJmpCode(thenLabel,elseLabel);
  cout << thenLabel << ":" << endl;
  thenBody->genCode(next);
  cout << "\tgoto " << next << endl;
  cout << elseLabel << ":" << endl;
  elseBody->genCode(next);
}

void WhileStmt::genCode(const Label& next) {
  Label topLabel, bodyLabel;
  cout << topLabel << ":" << endl;
  cond->genJmpCode(bodyLabel,next);
  cout << bodyLabel << ":" << endl;
  body->genCode(topLabel);
  cout << "\tgoto " << topLabel << endl;
}
