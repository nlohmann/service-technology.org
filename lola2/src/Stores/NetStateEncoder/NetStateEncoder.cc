
#include <config.h>
#include <Stores/NetStateEncoder/NetStateEncoder.h>
#include <cstdio>
#include <cstdlib>
#include <InputOutput/Reporter.h>

void NetStateEncoder::decodeNetState(NetState& ns, vectordata_t* data)
{
  rep->status("this encoder can not decode states");
  rep->abort(ERROR_COMMANDLINE);
}
