#include "nsmb.hpp"

#if defined(gen_obj_table) || defined(IDE)

#define repl_obj(objID, class)
#define repl_stageObj(objID, stageObjID, class)

#include "actors/SpinDrift.hpp"

repl_stageObj(256, 171, SpinDrift)

#endif
