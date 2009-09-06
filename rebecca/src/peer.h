#ifndef PEER_H
#define PEER_H

#include "types.h"
#include <set>
#include <string>


class Peer
{
public:
  Peer(const std::string &);

  void pushIn(const std::string &);
  void pushOut(const std::string &);

  void pushInput(const std::string &);
  void pushOutput(const std::string &);

  const std::string name() const;

  const std::set<std::string> & in() const;
  const std::set<std::string> & out() const;

  const std::set<std::string> & input() const;
  const std::set<std::string> & output() const;

  void deriveEvent(const std::string &, CommunicationType);

private:
  // peer name
  std::string name_;

  // channel sets
  std::set<std::string> in_;
  std::set<std::string> out_;

  // event sets
  std::set<std::string> input_;
  std::set<std::string> output_;

};

#endif /* PEER_H */
