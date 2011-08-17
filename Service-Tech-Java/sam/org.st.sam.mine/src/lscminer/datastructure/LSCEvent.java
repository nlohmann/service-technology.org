package lscminer.datastructure;
/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSCEvent {
  String caller;
  String callee;
  String method;

  public LSCEvent(String caller, String callee, String method){
    this.caller = caller;
    this.callee = callee;
    this.method = method;
  }

  public String getCaller(){
    return this.caller;
  }

  public String getCallee(){
    return this.callee;
  }

  public String getMethod(){
    return this.method;
  }

  @Override
  public boolean equals(Object lscEventObject){
    if (lscEventObject instanceof LSCEvent ){
      LSCEvent lscEvent = (LSCEvent)lscEventObject;
      return lscEvent instanceof LSCEvent
            && this.getCaller().equals(lscEvent.getCaller())
            && this.getCallee().equals(lscEvent.getCallee())
            && this.getMethod().equals(lscEvent.getMethod());
    } else {
      return false;
    }
  }

  @Override
  public int hashCode() {
    int hash = 5;
    hash = 67 * hash + (this.caller != null ? this.caller.hashCode() : 0);
    hash = 67 * hash + (this.callee != null ? this.callee.hashCode() : 0);
    hash = 67 * hash + (this.method != null ? this.method.hashCode() : 0);
    return hash;
  }

  @Override
  public String toString(){
    return caller + "|" + callee + "|" + method;
  }
}
