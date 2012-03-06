package org.st.sam.mine.collect;

import java.util.Iterator;

public class SimpleArrayList<T>  implements Iterable<T> {
private static final int INIT_SIZE = 8;
  
  protected Object[] contents;
  private int size;
  
  public SimpleArrayList(){
    contents = new Object[INIT_SIZE];
    size = 0;
  }
  
  public void add(T x) {
    if( size() == contents.length )
      resize();
    contents[size] = x;
    size++;
  }
  
  protected void resize(){
    Object[] temp = new Object[size() * 2];
    System.arraycopy(contents, 0, temp, 0, size());
    contents = temp;
  }
  
  public int size() {
    return size;
  }

  public T get(int loc) {
    return (T)contents[loc];
  }

  public T remove(int loc) {
    for(int i = loc; i < size - 1; i++){
      assert 0 <= i && i < contents.length;
      contents[i] = contents[i+1];
    }
    return (T)contents[loc];
  }
  
  public void clear(){
    contents = new Object[INIT_SIZE];
    size = 0;
  }

  @Override
  public Iterator<T> iterator() {
    return new SimpleArrayListIterator();
  }

  public class SimpleArrayListIterator implements Iterator<T>  {
    
    private int counter = 0;

    @Override
    public boolean hasNext() {
      return (counter < contents.length && contents[counter] != null);
    }

    @Override
    public T next() {
      return (T)contents[counter++];
    }

    @Override
    public void remove() {
    }
    
  }
}
