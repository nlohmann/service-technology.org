package lscminer.datastructure;
/**
 *
 * @author Anh Cuong Nguyen
 */
public class Project {
  String name;
  String location;

  public Project(String name, String location){
    this.name = name;
    this.location = location.replace('\\', '/');
  }

  public String getName(){
    return name;
  }

  public String getLocation(){
    return location;
  }

  @Override
  public String toString(){
    return name;
  }

  @Override
  public int hashCode() {
    int hash = 3;
    hash = 67 * hash + (this.name != null ? this.name.hashCode() : 0);
    hash = 67 * hash + (this.location != null ? this.location.hashCode() : 0);
    return hash;
  }

  @Override
  public boolean equals(Object projectObj){
    if (projectObj instanceof Project){
      Project project = (Project)projectObj;
      return name.equals(project.getName()) && location.equals(project.getLocation());
    } else {
      return false;
    }
  }
}
