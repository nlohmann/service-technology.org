package org.st.sam.mine.datastructure;
/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSCEvent {
  short caller;
  short callee;
  short method;

  public LSCEventTable table;
  
  public LSCEvent(short caller, short callee, short method, LSCEventTable table){
    this.caller = caller;
    this.callee = callee;
    this.method = method;
    this.table = table;
  }

  public short getCaller(){
    return this.caller;
  }

  public short getCallee(){
    return this.callee;
  }

  public short getMethod(){
    return this.method;
  }
  
  public String getCallerName(){
    return table.component_names[caller];
  }

  public String getCalleeName(){
    return table.component_names[callee];
  }

  public String getMethodName(){
    return table.method_names[method];
  }


  @Override
  public boolean equals(Object lscEventObject){
    if (lscEventObject instanceof LSCEvent ){
      LSCEvent lscEvent = (LSCEvent)lscEventObject;
      return lscEvent instanceof LSCEvent
            && this.getCaller() == lscEvent.getCaller()
            && this.getCallee() == lscEvent.getCallee()
            && this.getMethod() == lscEvent.getMethod();
    } else {
      return false;
    }
  }

  @Override
  public int hashCode() {
    int hash = 5;
    hash = 67 * hash + this.caller;
    hash = 67 * hash + this.callee;
    hash = 67 * hash + this.method;
    return hash;
  }

  @Override
  public String toString(){
    return getCallerName() + "|" + getCalleeName() + "|" + getMethodName();
  }
}
