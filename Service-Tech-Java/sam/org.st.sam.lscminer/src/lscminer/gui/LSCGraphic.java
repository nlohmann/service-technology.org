package lscminer.gui;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import lscminer.datastructure.*;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSCGraphic extends JComponent{
  public LSC lsc;
  public int defaultOrigCoord = 20;
  public int defaultRecWidth = 70;
  public int defaultRecHeight = 50;
  public int defaultRecDistance = 30;
  public int horLength  = 0;

  public LSCGraphic(LSC lsc){
    this.lsc = lsc;
    defaultRecWidth = defaultOrigCoord + lsc.getObjectStringMaxLength() * 6 + defaultOrigCoord;
    defaultRecDistance = Math.max(30, (lsc.getMethodStringMaxLength() - lsc.getObjectStringMaxLength()) * 6);
    horLength = (lsc.getPrechartEventNo() + lsc.getMainchartEventNo() + 1)* 40;
  }

  public void paintObject(Graphics g, String object, int drawedObject){
    g.setColor(Color.BLACK);
    g.fillRect(defaultOrigCoord+(drawedObject)*(defaultRecWidth+defaultRecDistance)-2,
               defaultOrigCoord-2,
               defaultRecWidth+4,
               defaultRecHeight+4);
    g.drawLine(defaultOrigCoord+(drawedObject)*(defaultRecWidth+defaultRecDistance)+defaultRecWidth/2,
               defaultRecHeight + defaultOrigCoord,
               defaultOrigCoord+(drawedObject)*(defaultRecWidth+defaultRecDistance)+defaultRecWidth/2,
                defaultRecHeight + defaultOrigCoord + horLength);
    g.setColor(Color.lightGray);
    g.fillRect(defaultOrigCoord+(drawedObject)*(defaultRecWidth+defaultRecDistance),
               defaultOrigCoord,
               defaultRecWidth,
               defaultRecHeight);
    g.setColor(Color.BLACK);
    g.drawString(object,
                 defaultOrigCoord + (drawedObject)*(defaultRecWidth+defaultRecDistance)+10,
                 defaultOrigCoord + (int)(defaultRecHeight*0.6));
  }

  public int paintEvent(Graphics g, LSCEvent lscEv, HashMap<String, Integer> objCoord, 
          int drawedObject, int drawedEvent, Color horLineColor){
    String caller = lscEv.getCaller();
    String callee = lscEv.getCallee();
    String method = lscEv.getMethod();

    /* draw caller */
    if (!objCoord.containsKey(caller)) {
      paintObject(g, caller, drawedObject);
      objCoord.put(caller, defaultOrigCoord + (drawedObject) * (defaultRecWidth + defaultRecDistance));
      drawedObject++;
    }

    /* draw callee */
    if (!objCoord.containsKey(callee)) {
      paintObject(g, callee, drawedObject);
      objCoord.put(callee, defaultOrigCoord + (drawedObject) * (defaultRecWidth + defaultRecDistance));
      drawedObject++;
    }

    /* draw method */
    g.setColor(horLineColor);
    g.drawLine(objCoord.get(caller)+((int)(defaultRecWidth*0.5)),
               40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent,
               objCoord.get(callee)+((int)(defaultRecWidth*0.5)),
               40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent);
    int direction = objCoord.get(caller) > objCoord.get(callee) ? -1 : 1;
    if (direction == 1){
      g.drawString(method,
                 objCoord.get(caller)+((int)(defaultRecWidth*0.5))+5,
                 40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent-5);
    } else {
      g.drawString(method,
                 objCoord.get(callee)+((int)(defaultRecWidth*0.5))+5,
                 40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent-5);
    }

    Polygon triangle = new Polygon();
    triangle.addPoint(objCoord.get(callee)+((int)(defaultRecWidth*0.5)),
            40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent);
    triangle.addPoint(objCoord.get(callee)+((int)(defaultRecWidth*0.5)-direction*5),
            40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent-5);
    triangle.addPoint(objCoord.get(callee)+((int)(defaultRecWidth*0.5)-direction*5),
            40 + defaultOrigCoord + defaultRecHeight + 40*drawedEvent+5);
    g.fillPolygon(triangle);
    return drawedObject;
  }

  @Override
  public void paint(Graphics g){
    LSCEvent[] preChart = lsc.getPreChart();
    LSCEvent[] mainChart = lsc.getMainChart();
    /* this hash maps a object with its y coordinate (x coordinate is fixed) */
    HashMap<String, Integer> objCoord = new HashMap<String, Integer>();

    int drawedEvent = 0;
    int drawedObject = 0;

    g.setFont(new Font("Tamoha", Font.BOLD, 13));

    for (LSCEvent lscEv: preChart){
     drawedObject = paintEvent(g, lscEv, objCoord, drawedObject, drawedEvent, Color.BLUE);
     drawedEvent++;
    }

    for (LSCEvent lscEv: mainChart){
      drawedObject = paintEvent(g, lscEv, objCoord, drawedObject, drawedEvent, Color.RED);
      drawedEvent++;
    }

  }

  public static void main(String[] args){
    LSCEvent event01 = new LSCEvent("A", "B", "met1");
    LSCEvent event02 = new LSCEvent("C", "D", "met2");
    LSCEvent event03 = new LSCEvent("C", "B", "met3");

    LSCEvent[] preChart = {event01};
    LSCEvent[] mainChart = {event02, event03};

    LSC lsc = new LSC(preChart, mainChart, 5, 1);

    JFrame window = new JFrame();
    window.setResizable(false);
    window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    window.getContentPane().add(new LSCGraphic(lsc));
    window.setBounds(30, 30, (4*(70+2) + 3*30 + 2*20) , 300);
    window.setVisible(true);
  }
}
