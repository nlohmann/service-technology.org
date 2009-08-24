#ifndef PEER_H
#define PEER_H

#include <set>
#include <string>


class Peer
{
public:
  Peer(const std::string &);

  void pushInput(const std::string &);
  void pushOutput(const std::string &);

  const std::string name() const;
  const std::set<std::string> & input() const;
  const std::set<std::string> & output() const;

private:
  std::string name_;
  std::set<std::string> input_;
  std::set<std::string> output_;

};

#endif /* PEER_H */
