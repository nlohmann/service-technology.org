template <class V>
class Dimension {
  public:
    V (*getNeutralElement)();
    V (*getResult) (V, V);
    Dimension(V (*getNeutralElementP)(), V (*getResultP)(V,V)) {
      getNeutralElement = getNeutralElementP;
      getResult = getResultP;
    }


};


