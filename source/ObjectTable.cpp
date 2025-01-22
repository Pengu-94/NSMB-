#include "nsmb.hpp"

#if defined(gen_obj_table) || defined(IDE)

#define repl_obj(objID, class)
#define repl_stageObj(objID, stageObjID, class)

#include "actors/ExclamationBox/ExclamationBox.hpp"
#include "actors/Spindrift/SpinDrift.hpp"

repl_stageObj(253, 145, ExclamationBox)
repl_stageObj(256, 171, SpinDrift)

#endif
