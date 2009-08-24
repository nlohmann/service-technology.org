#include "helpers.h"
#include "peer.h"
#include "verbose.h"

using std::multiset;


bool sanityCheck(const vector<Peer *> peers)
{
  int max = (int) peers.size();
  multiset<const string> channels;
  for (int i = 0; i < (int) peers.size(); i++)
  {
    for (set<const string>::iterator in = peers[i]->input().begin(); in
        != peers[i]->input().end(); in++)
      if (channels.count(*in) > 0)
        abort(
            1,
            "sanity check failed: input channels of peer '%s' are not disjoint (channel '%s')",
            peers[i]->name(), in->c_str());
      else
        channels.insert(*in);
    for (set<const string>::iterator out = peers[i]->output().begin(); out
        != peers[i]->output().end(); out++)
      if (channels.count(*out) > 1)
        abort(
            2,
            "sanity check failed: output channels of peer '%s' are not disjoint (channel '%s')",
            peers[i]->name(), out->c_str());
      else
        channels.insert(*out);
  }
  for (multiset<const string>::iterator label = channels.begin(); label
      != channels.end(); label++)
  {

  }
}
